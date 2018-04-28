#include <GXEngine_Editor_Mobile/EMImportantAreaFilterMaterial.h>
#include <GXCommon/GXMemory.h>


#define VERTEX_SHADER			L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/Editor Mobile/ImportantAreaFilter_fs.txt"

#define IMAGE_SLOT				0u
#define RETINA_FILTER_SLOT		1u

#define INVALID_TEXTURE_OBJECT	0u
#define MINIMUM_U				0.0
#define MAXIMUM_U				1.0
#define MINIMUM_V				0.0
#define MAXIMUM_V				1.0
#define THREE_SIGMA_RULE_FACTOR	0.16666666666666666666666666666667
#define DOUBLE_PI				6.283185307179586476925286766559

//---------------------------------------------------------------------------------------------

EMImportantAreaFilterMaterial::EMImportantAreaFilterMaterial ():
	imageTexture ( nullptr )
{
	static const GLchar* samplerNames[ 3 ] = { "imageSampler", "retinaFilterSampler" };
	static const GLuint samplerLocations[ 3 ] = { IMAGE_SLOT, RETINA_FILTER_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 2u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );
}

EMImportantAreaFilterMaterial::~EMImportantAreaFilterMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );

	if ( retinaFilterTexture.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		retinaFilterTexture.FreeResources ();
}

GXVoid EMImportantAreaFilterMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !imageTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	imageTexture->Bind ( IMAGE_SLOT );
	retinaFilterTexture.Bind ( RETINA_FILTER_SLOT );
}

GXVoid EMImportantAreaFilterMaterial::Unbind ()
{
	if ( !imageTexture ) return;

	glUseProgram ( 0u );

	imageTexture->Unbind ();
	retinaFilterTexture.Unbind ();
}

GXVoid EMImportantAreaFilterMaterial::SetImageTexture ( GXTexture2D &texture )
{
	imageTexture = &texture;

	GXUShort width = texture.GetHeight ();
	GXUShort height = texture.GetWidth ();

	GXUShort effectiveLength = height < width ? height : width;

	if ( effectiveLength == retinaFilterTexture.GetHeight () ) return;

	GenerateRetinaFilterTexture ( effectiveLength );
}

GXVoid EMImportantAreaFilterMaterial::GenerateRetinaFilterTexture ( GXUShort effectiveLength )
{
	GXDouble step = ( MAXIMUM_V - MINIMUM_V ) / static_cast<GXDouble> ( effectiveLength - 1u );

	GXDouble differenceU = MAXIMUM_U - MINIMUM_U;
	GXDouble differenceV = MAXIMUM_V - MINIMUM_V;
	GXDouble meanU = MINIMUM_U + differenceU * 0.5;
	GXDouble meanV = MINIMUM_V + differenceV * 0.5;
	GXDouble standartDeviationU = differenceU * THREE_SIGMA_RULE_FACTOR;
	GXDouble standartDeviationV = differenceV * THREE_SIGMA_RULE_FACTOR;

	GXDouble alpha = step * 0.5;
	GXDouble limitU = MAXIMUM_U + alpha;
	GXDouble limitV = MAXIMUM_V + alpha;

	alpha = 1.0 / ( standartDeviationU * standartDeviationU );
	GXDouble omega = 1.0 / ( standartDeviationV * standartDeviationV );
	GXDouble yotta = 1.0 / ( DOUBLE_PI * standartDeviationU * standartDeviationV );

	GXDouble totalGaussSum = 0.0;
	GXDouble v = MINIMUM_V;

	GXUInt samples = static_cast<GXUInt> ( effectiveLength * effectiveLength );
	GXUPointer sampleOffset = 0u;
	GXDouble* probabilityDensitySamples = static_cast<GXDouble*> ( malloc ( samples * sizeof ( GXDouble ) ) );

	while ( v < limitV )
	{
		GXDouble gamma = v - meanV;
		GXDouble phi = omega * gamma * gamma;

		GXDouble u = MINIMUM_U;

		while ( u < limitU )
		{
			GXDouble zetta = u - meanU;
			
			GXDouble sample = yotta * exp ( -0.5 * ( alpha * zetta * zetta + phi ) );
			totalGaussSum += sample;
			probabilityDensitySamples[ sampleOffset ] = sample;

			sampleOffset++;
			u += step;
		}

		v += step;
	}

	GXDouble hardwareReducingCompensationFactor = static_cast<GXDouble> ( samples ) / totalGaussSum;
	GXFloat* retinaFilterSamples = static_cast<GXFloat*> ( malloc ( samples * sizeof ( GXFloat ) ) );

	for ( sampleOffset = 0u; sampleOffset < samples; sampleOffset++ )
		retinaFilterSamples[ sampleOffset ] = static_cast<GXFloat> ( hardwareReducingCompensationFactor * probabilityDensitySamples[ sampleOffset ] );

	free ( probabilityDensitySamples );

	if ( retinaFilterTexture.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		retinaFilterTexture.FreeResources ();

	retinaFilterTexture.InitResources ( effectiveLength, effectiveLength, GL_R32F, GX_FALSE, GL_CLAMP_TO_EDGE );
	retinaFilterTexture.FillWholePixelData ( retinaFilterSamples );

	free ( retinaFilterSamples );
}
