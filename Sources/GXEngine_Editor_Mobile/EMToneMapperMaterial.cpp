#include <GXEngine_Editor_Mobile/EMToneMapperMaterial.h>


#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/ToneMapper_fs.txt"

#define TEXTURE_SLOT						0

#define DEFAULT_GAMMA						2.2f

#define EPSILON								1.0e-4f

#define LIMINANCE_RED_FACTOR				0.27f
#define LIMINANCE_GREEN_FACTOR				0.67f
#define LIMINANCE_BLUE_FACTOR				0.06f

#define DEFAULT_ABSOLUTE_WHITE_INTENSITY	1.0e+5f

#define DEFAULT_AVERAGE_COLOR_RED			0.5f
#define DEFAULT_AVERAGE_COLOR_GREEN			0.5f
#define DEFAULT_AVERAGE_COLOR_BLUE			0.5f


EMToneMapperMaterial::EMToneMapperMaterial ()
{
	static const GLchar* samplerNames[ 1 ] = { "linearSpaceSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 1;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	inverseGammaLocation = shaderProgram.GetUniform ( "inverseGamma" );
	inverseAverageLuminanceLocation = shaderProgram.GetUniform ( "inverseAverageLuminance" );
	inverseAbsoluteWhiteSquareIntensityLocation = shaderProgram.GetUniform ( "inverseAbsoluteWhiteSquareIntensity" );

	linearSpaceTexture = nullptr;
	SetGamma ( DEFAULT_GAMMA );

	GXVec3 defaultAverageColor ( DEFAULT_AVERAGE_COLOR_RED, DEFAULT_AVERAGE_COLOR_GREEN, DEFAULT_AVERAGE_COLOR_BLUE );
	SetAverageColor ( defaultAverageColor );

	SetAbsoluteWhiteIntensity ( DEFAULT_ABSOLUTE_WHITE_INTENSITY );
}

EMToneMapperMaterial::~EMToneMapperMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMToneMapperMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !linearSpaceTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	glUniform1f ( inverseGammaLocation, inverseGamma );
	glUniform1f ( inverseAverageLuminanceLocation, inverseAverageLuminance );
	glUniform1f ( inverseAbsoluteWhiteSquareIntensityLocation, inverseAbsoluteWhiteSquareIntensity );
	linearSpaceTexture->Bind ( TEXTURE_SLOT );
}

GXVoid EMToneMapperMaterial::Unbind ()
{
	if ( !linearSpaceTexture ) return;

	glUseProgram ( 0 );
	linearSpaceTexture->Unbind ();
}

GXVoid EMToneMapperMaterial::SetLinearSpaceTexture ( GXTexture2D &texture )
{
	linearSpaceTexture = &texture;
}

GXVoid EMToneMapperMaterial::SetGamma ( GXFloat gamma )
{
	this->gamma = gamma;
	inverseGamma = 1.0f / gamma;
}

GXFloat EMToneMapperMaterial::GetGamma () const
{
	return gamma;
}

GXVoid EMToneMapperMaterial::SetAverageColor ( const GXVec3 &averageColor )
{
	GXFloat luminance = LIMINANCE_RED_FACTOR * averageColor.r + LIMINANCE_GREEN_FACTOR* averageColor.g + LIMINANCE_BLUE_FACTOR * averageColor.b;
	inverseAverageLuminance = 1.0f / ( luminance + EPSILON );
}

GXVoid EMToneMapperMaterial::SetAbsoluteWhiteIntensity ( GXFloat intensity )
{
	inverseAbsoluteWhiteSquareIntensity = 1.0f / ( intensity * intensity + EPSILON );
}
