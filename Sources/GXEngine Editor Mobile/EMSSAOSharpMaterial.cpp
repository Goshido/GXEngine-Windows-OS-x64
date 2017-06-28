#include <GXEngine_Editor_Mobile/EMSSAOSharpMaterial.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXLogger.h>
#include <time.h>


#define VERTEX_SHADER						L"Shaders/Editor Mobile/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/SSAOSharp_fs.txt"

#define DEFAULT_MAX_CHECK_RADIUS			0.1f
#define DEFAULT_SAMPLES						64
#define DEFAULT_NOISE_TEXTURE_RESOLUTION	2048
#define DEFAULT_MAX_DISTANCE				1000.0f

#define MINIMUM_KERNEL_SCALE				0.01f
#define NOISE_TEXTURE_BYTES_PER_PIXEL		2
#define CORRECTED_NOISE_X					1.0f

#define DEPTH_SLOT							0
#define NORMAL_SLOT							1
#define NOISE_SLOT							2


EMSSAOSharpMaterial::EMSSAOSharpMaterial ()
{
	depthTexture = nullptr;
	normalTexture = nullptr;

	static const GLchar* samplerNames[ 3 ] = { "depthSampler", "normalSampler", "noiseSampler" };
	static const GLuint samplerLocations[ 3 ] = { DEPTH_SLOT, NORMAL_SLOT, NOISE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 3;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	kernelLocation = shaderProgram.GetUniform ( "kernel" );
	checkRadiusLocation = shaderProgram.GetUniform ( "checkRadius" );
	samplesLocation = shaderProgram.GetUniform ( "samples" );
	inverseSamplesLocation = shaderProgram.GetUniform ( "inverseSamples" );
	noiseScaleLocation = shaderProgram.GetUniform ( "noiseScale" );
	maxDistanceLocation = shaderProgram.GetUniform ( "maxDistance" );
	projectionMatrixLocation = shaderProgram.GetUniform ( "projectionMatrix" );
	inverseProjectionMatrixLocation = shaderProgram.GetUniform ( "inverseProjectionMatrix" );

	checkRadius = DEFAULT_MAX_CHECK_RADIUS;
	SetSampleNumber ( DEFAULT_SAMPLES );
	SetNoiseTextureResolution ( DEFAULT_NOISE_TEXTURE_RESOLUTION );
	SetMaximumDistance ( DEFAULT_MAX_DISTANCE );
}

EMSSAOSharpMaterial::~EMSSAOSharpMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
	noiseTexture.FreeResources ();
}

GXVoid EMSSAOSharpMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !depthTexture || !normalTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* camera = GXCamera::GetActiveCamera ();

	const GXMat4& projectionMatrix = camera->GetCurrentFrameProjectionMatrix ();
	const GXMat4& inverseProjectionMatrix = camera->GetCurrentFrameInverseProjectionMatrix ();

	glUniformMatrix4fv ( projectionMatrixLocation, 1, GL_FALSE, projectionMatrix.arr );
	glUniformMatrix4fv ( inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix.arr );
	glUniform3fv ( kernelLocation, EM_MAX_SSAO_SAMPLES, (const GLfloat*)kernel );
	glUniform1f ( checkRadiusLocation, checkRadius );
	glUniform1i ( samplesLocation, samples );
	glUniform1f ( inverseSamplesLocation, inverseSamples );
	glUniform2f ( noiseScaleLocation, noiseScale.x, noiseScale.y );
	glUniform1f ( maxDistanceLocation, maxDistance );

	depthTexture->Bind ( DEPTH_SLOT );
	normalTexture->Bind ( NORMAL_SLOT );
	noiseTexture.Bind ( NOISE_SLOT );
}

GXVoid EMSSAOSharpMaterial::Unbind ()
{
	if ( !depthTexture || !normalTexture ) return;

	glUseProgram ( 0 );

	depthTexture->Unbind ();
	normalTexture->Unbind ();
	noiseTexture.Unbind ();
}

GXVoid EMSSAOSharpMaterial::SetDepthTexture ( GXTexture &texture )
{
	depthTexture = &texture;
}

GXVoid EMSSAOSharpMaterial::SetNormalTexture ( GXTexture &texture )
{
	normalTexture = &texture;
}

GXVoid EMSSAOSharpMaterial::SetCheckRadius ( GXFloat meters )
{
	if ( meters <= 0.0f ) return;

	srand ( (GXUInt)time ( 0 ) );

	for ( GXInt i = 0; i < samples; i++ )
	{
		GXFloat scale = i * inverseSamples;
		scale *= scale;
		scale = MINIMUM_KERNEL_SCALE + scale * ( 1.0f - MINIMUM_KERNEL_SCALE );

		GXVec3 v;
		v.x = GXRandomBetween ( -1.0f, 1.0f );
		v.y = GXRandomBetween ( -1.0f, 1.0f );
		v.z = GXRandomNormalize ();

		if ( GXLengthVec3 ( v ) == 0.0f )
			v.z = 1.0f;
		else
			GXNormalizeVec3 ( v );

		GXMulVec3Scalar ( kernel[ i ], v, scale * meters );
	}

	checkRadius = meters;
}

GXFloat EMSSAOSharpMaterial::GetCheckRadius () const
{
	return checkRadius;
}

GXVoid EMSSAOSharpMaterial::SetSampleNumber ( GXUByte samples )
{
	if ( samples > EM_MAX_SSAO_SAMPLES )
	{
		GXLogW ( L"EMSSAOSharpMaterial::SetSampleNumber::Warning - �� ���� ���������� ������� ������� (������� %hu). ����� ������������ ���������� ������� ������ %i.\n", samples, EM_MAX_SSAO_SAMPLES );
		this->samples = EM_MAX_SSAO_SAMPLES;
	}
	else
	{
		this->samples = (GXInt)samples;
	}

	inverseSamples = 1.0f / (GXFloat)this->samples;

	SetCheckRadius ( checkRadius );
}

GXUByte EMSSAOSharpMaterial::GetSampleNumber () const
{
	return (GXUByte)samples;
}

GXVoid EMSSAOSharpMaterial::SetNoiseTextureResolution ( GXUShort resolution )
{
	if ( resolution == 0 )
	{
		GXLogW ( L"EMSSAOSharpMaterial::SetNoiseTextureResolution::Error - ������� ���������� � 0 ��������!\n" );
		return;
	}

	noiseTexture.FreeResources ();
	
	GXUInt totalPixels = resolution * resolution;
	GXUByte* noiseData = (GXUByte*)malloc ( totalPixels * NOISE_TEXTURE_BYTES_PER_PIXEL );

	srand ( (GXUInt)time ( 0 ) );

	GXUInt offset = 0;
	for ( GXUInt i = 0; i < totalPixels; i++ )
	{
		GXVec2 v;
		v.x = GXRandomBetween ( -1.0f, 1.0f );
		v.y = GXRandomBetween ( -1.0f, 1.0f );

		if ( GXLengthVec2 ( v ) == 0.0f )
			v.x = CORRECTED_NOISE_X;
		else
			GXNormalizeVec2 ( v );

		noiseData[ offset ] = (GXUByte)( v.x * 128 + 127 );
		offset++;

		noiseData[ offset ] = (GXUByte)( v.y * 128 + 127 );
		offset++;
	}

	noiseTexture.InitResources ( resolution, resolution, GL_RG8, GL_FALSE, GL_REPEAT );
	noiseTexture.FillWholePixelData ( noiseData );

	free ( noiseData );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	GXFloat inverseResolution = 1.0f / (GXFloat)resolution;

	noiseScale.x = renderer.GetWidth () * inverseResolution;
	noiseScale.y = renderer.GetHeight () * inverseResolution;
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