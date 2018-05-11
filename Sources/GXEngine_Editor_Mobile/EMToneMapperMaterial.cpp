#include <GXEngine_Editor_Mobile/EMToneMapperMaterial.h>


#define DEFAULT_GAMMA						2.2f
#define DEFAULT_ABSOLUTE_WHITE_INTENSITY	1.0e+5f

#define DEFAULT_AVERAGE_LUMINANCE			0.5f
#define DEFAULT_MINIMUM_LUMINANCE			0.5f
#define DEFAULT_MAXIMUM_LUMINANCE			0.5f

#define DEFAULT_EYE_SENSETIVITY				0.0072f

#define EPSILON								1.0e-4f

#define LIMINANCE_RED_FACTOR				0.27f
#define LIMINANCE_GREEN_FACTOR				0.67f
#define LIMINANCE_BLUE_FACTOR				0.06f

#define TEXTURE_SLOT						0u

#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/ToneMapper_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMToneMapperMaterial::EMToneMapperMaterial ():
	linearSpaceTexture ( nullptr ),
	sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
	gamma ( DEFAULT_GAMMA ),
	inverseGamma ( 1.0f / DEFAULT_GAMMA ),
	eyeSensitivity ( DEFAULT_EYE_SENSETIVITY )
{
	static const GLchar* samplerNames[ 1 ] = { "linearSpaceSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 1u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	inverseGammaLocation = shaderProgram.GetUniform ( "inverseGamma" );
	prescaleFactorLocation = shaderProgram.GetUniform ( "prescaleFactor" );
	inverseAbsoluteWhiteSquareIntensityLocation = shaderProgram.GetUniform ( "inverseAbsoluteWhiteSquareIntensity" );

	SetLuminanceTriplet ( DEFAULT_AVERAGE_LUMINANCE, DEFAULT_MINIMUM_LUMINANCE, DEFAULT_MAXIMUM_LUMINANCE );
	SetAbsoluteWhiteIntensity ( DEFAULT_ABSOLUTE_WHITE_INTENSITY );
}

EMToneMapperMaterial::~EMToneMapperMaterial ()
{
	// NOTHING
}

GXVoid EMToneMapperMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !linearSpaceTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	glUniform1f ( inverseGammaLocation, inverseGamma );
	glUniform1f ( prescaleFactorLocation, prescaleFactor );
	glUniform1f ( inverseAbsoluteWhiteSquareIntensityLocation, inverseAbsoluteWhiteSquareIntensity );

	linearSpaceTexture->Bind ( TEXTURE_SLOT );
	sampler.Bind ( TEXTURE_SLOT );
}

GXVoid EMToneMapperMaterial::Unbind ()
{
	if ( !linearSpaceTexture ) return;

	sampler.Unbind ( TEXTURE_SLOT );
	linearSpaceTexture->Unbind ();

	glUseProgram ( 0 );
}

GXVoid EMToneMapperMaterial::SetLinearSpaceTexture ( GXTexture2D &texture )
{
	linearSpaceTexture = &texture;
}

GXVoid EMToneMapperMaterial::SetLuminanceTriplet ( GXFloat averageLuminance, GXFloat minimumLuminance, GXFloat maximumLuminance )
{
	GXFloat alpha = ( maximumLuminance - averageLuminance );
	GXFloat betta = ( averageLuminance - minimumLuminance );
	GXFloat yotta = eyeSensitivity * powf ( 2.0f, 2.0f * ( betta - alpha ) / ( alpha + betta ) );

	prescaleFactor = yotta / ( averageLuminance + EPSILON );
}

GXVoid EMToneMapperMaterial::SetGamma ( GXFloat newGamma )
{
	gamma = newGamma;
	inverseGamma = 1.0f / gamma;
}

GXFloat EMToneMapperMaterial::GetGamma () const
{
	return gamma;
}

GXVoid EMToneMapperMaterial::SetEyeSensitivity ( GXFloat sensitivity )
{
	eyeSensitivity = sensitivity;
}

GXFloat EMToneMapperMaterial::GetEyeSensitivity () const
{
	return eyeSensitivity;
}

GXVoid EMToneMapperMaterial::SetAbsoluteWhiteIntensity ( GXFloat intensity )
{
	absoluteWhiteIntensity = intensity;
	inverseAbsoluteWhiteSquareIntensity = 1.0f / ( intensity * intensity + EPSILON );
}

GXFloat EMToneMapperMaterial::GetAbsoluteWhiteIntensity () const
{
	return absoluteWhiteIntensity;
}
