#include <GXEngine_Editor_Mobile/EMSSAOSharpMaterial.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_MAX_CHECK_RADIUS			0.1f
#define DEFAULT_SAMPLES						64
#define DEFAULT_NOISE_TEXTURE_RESOLUTION	128u
#define DEFAULT_MAX_DISTANCE				1000.0f

#define MINIMUM_KERNEL_SCALE				0.01f
#define NOISE_TEXTURE_BYTES_PER_PIXEL		2u
#define CORRECTED_NOISE_X					1.0f

#define DEPTH_SLOT							0u
#define NORMAL_SLOT							1u
#define NOISE_SLOT							2u

#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/SSAOSharp_fs.txt"

//------------------------------------------------------------------------------------

EMSSAOSharpMaterial::EMSSAOSharpMaterial ():
	depthTexture ( nullptr ),
	normalTexture ( nullptr ),
	maxDistance ( DEFAULT_MAX_DISTANCE ),
	checkRadius ( DEFAULT_MAX_CHECK_RADIUS )
{
	static const GLchar* samplerNames[ 3 ] = { "depthSampler", "normalSampler", "noiseSampler" };
	static const GLuint samplerLocations[ 3 ] = { DEPTH_SLOT, NORMAL_SLOT, NOISE_SLOT };

	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 3u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	kernelLocation = shaderProgram.GetUniform ( "kernel" );
	checkRadiusLocation = shaderProgram.GetUniform ( "checkRadius" );
	samplesLocation = shaderProgram.GetUniform ( "samples" );
	inverseSamplesLocation = shaderProgram.GetUniform ( "inverseSamples" );
	noiseScaleLocation = shaderProgram.GetUniform ( "noiseScale" );
	maxDistanceLocation = shaderProgram.GetUniform ( "maxDistance" );
	projectionMatrixLocation = shaderProgram.GetUniform ( "projectionMatrix" );
	inverseProjectionMatrixLocation = shaderProgram.GetUniform ( "inverseProjectionMatrix" );

	SetSampleNumber ( DEFAULT_SAMPLES );
	SetNoiseTextureResolution ( DEFAULT_NOISE_TEXTURE_RESOLUTION );
}

EMSSAOSharpMaterial::~EMSSAOSharpMaterial ()
{
	noiseTexture.FreeResources ();
}

GXVoid EMSSAOSharpMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !depthTexture || !normalTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* camera = GXCamera::GetActiveCamera ();

	const GXMat4& projectionMatrix = camera->GetCurrentFrameProjectionMatrix ();
	const GXMat4& inverseProjectionMatrix = camera->GetCurrentFrameInverseProjectionMatrix ();

	glUniformMatrix4fv ( projectionMatrixLocation, 1, GL_FALSE, projectionMatrix.data );
	glUniformMatrix4fv ( inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix.data );
	glUniform3fv ( kernelLocation, EM_MAX_SSAO_SAMPLES, reinterpret_cast<const GLfloat*> ( kernel ) );
	glUniform1f ( checkRadiusLocation, checkRadius );
	glUniform1i ( samplesLocation, samples );
	glUniform1f ( inverseSamplesLocation, inverseSamples );
	glUniform2fv ( noiseScaleLocation, 1, noiseScale.data );
	glUniform1f ( maxDistanceLocation, maxDistance );

	depthTexture->Bind ( DEPTH_SLOT );
	normalTexture->Bind ( NORMAL_SLOT );
	noiseTexture.Bind ( NOISE_SLOT );
}

GXVoid EMSSAOSharpMaterial::Unbind ()
{
	if ( !depthTexture || !normalTexture ) return;

	glUseProgram ( 0u );

	depthTexture->Unbind ();
	normalTexture->Unbind ();
	noiseTexture.Unbind ();
}

GXVoid EMSSAOSharpMaterial::SetDepthTexture ( GXTexture2D &texture )
{
	depthTexture = &texture;
}

GXVoid EMSSAOSharpMaterial::SetNormalTexture ( GXTexture2D &texture )
{
	normalTexture = &texture;
}

GXVoid EMSSAOSharpMaterial::SetCheckRadius ( GXFloat meters )
{
	if ( meters <= 0.0f ) return;

	GXRandomize ();

	for ( GXInt i = 0; i < samples; i++ )
	{
		GXFloat scale = i * inverseSamples;
		scale *= scale;
		scale = MINIMUM_KERNEL_SCALE + scale * ( 1.0f - MINIMUM_KERNEL_SCALE );

		GXVec3 v;
		v.SetX ( GXRandomBetween ( -1.0f, 1.0f ) );
		v.SetY ( GXRandomBetween ( -1.0f, 1.0f ) );
		v.SetZ ( GXRandomNormalize () );

		if ( v.Length () == 0.0f )
			v.SetZ ( 1.0f );
		else
			v.Normalize ();

		kernel[ i ].Multiply ( v, scale * meters );
	}

	checkRadius = meters;
}

GXFloat EMSSAOSharpMaterial::GetCheckRadius () const
{
	return checkRadius;
}

GXVoid EMSSAOSharpMaterial::SetSampleNumber ( GXUByte samplesPerPixel )
{
	if ( samplesPerPixel > EM_MAX_SSAO_SAMPLES )
	{
		GXLogW ( L"EMSSAOSharpMaterial::SetSampleNumber::Warning - Ќе могу обработать столько выборок (указано %hu). Ѕудет использовано количество выборок равное %i.\n", samples, EM_MAX_SSAO_SAMPLES );
		samples = EM_MAX_SSAO_SAMPLES;
	}
	else
	{
		samples = static_cast<GXInt> ( samplesPerPixel );
	}

	inverseSamples = 1.0f / static_cast<GXFloat> ( samples );

	SetCheckRadius ( checkRadius );
}

GXUByte EMSSAOSharpMaterial::GetSampleNumber () const
{
	return static_cast<GXUByte> ( samples );
}

GXVoid EMSSAOSharpMaterial::SetNoiseTextureResolution ( GXUShort resolution )
{
	if ( resolution == 0u )
	{
		GXLogW ( L"EMSSAOSharpMaterial::SetNoiseTextureResolution::Error - ”казано разрешение в 0 пикселей!\n" );
		return;
	}

	noiseTexture.FreeResources ();
	
	GXUInt totalPixels = static_cast<GXUInt> ( resolution * resolution );
	GXUByte* noiseData = static_cast<GXUByte*> ( malloc ( totalPixels * NOISE_TEXTURE_BYTES_PER_PIXEL ) );

	GXRandomize ();

	GXUInt offset = 0u;

	for ( GXUInt i = 0u; i < totalPixels; i++ )
	{
		GXVec2 v;
		v.SetX ( GXRandomBetween ( -1.0f, 1.0f ) );
		v.SetY ( GXRandomBetween ( -1.0f, 1.0f ) );

		if ( v.Length () == 0.0f )
			v.SetX ( CORRECTED_NOISE_X );
		else
			v.Normalize ();

		noiseData[ offset ] = static_cast<GXUByte> ( v.data[ 0 ] * 128.0f + 127.0f );
		offset++;

		noiseData[ offset ] = (GXUByte)( v.data[ 1 ] * 128.0f + 127.0f );
		offset++;
	}

	noiseTexture.InitResources ( resolution, resolution, GL_RG8, GL_FALSE, GL_REPEAT );
	noiseTexture.FillWholePixelData ( noiseData );

	free ( noiseData );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	GXFloat inverseResolution = 1.0f / static_cast<GXFloat> ( resolution );

	noiseScale.data[ 0 ] = renderer.GetWidth () * inverseResolution;
	noiseScale.data[ 1 ] = renderer.GetHeight () * inverseResolution;
}

GXUShort EMSSAOSharpMaterial::GetNoiseTextureResolution () const
{
	return noiseTexture.GetWidth ();
}

GXVoid EMSSAOSharpMaterial::SetMaximumDistance ( GXFloat meters )
{
	maxDistance = meters;
}

GXFloat EMSSAOSharpMaterial::GetMaximumDistance () const
{
	return maxDistance;
}
