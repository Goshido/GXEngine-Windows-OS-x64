#include <GXEngine_Editor_Mobile/EMVelocityTileMaxMaterial.h>


#define DEFAULT_MAX_BLUR_SAMPLES			15
#define DEFAULT_SCREEN_RESOLUTION_WIDTH		1280
#define DEFAULT_SCREEN_RESOLUTION_HEIGHT	720

#define VELOCITY_BLUR_SLOT					0

#define VERTEX_SHADER						L"Shaders/Editor Mobile/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/VelocityTileMax_fs.txt"


EMVelocityTileMaxMaterial::EMVelocityTileMaxMaterial ()
{
	velocityBlurTexture = nullptr;

	static const GLchar* samplerNames[ 1 ] = { "velocityBlurSampler" };
	static const GLuint samplerLocations[ 1 ] = { VELOCITY_BLUR_SLOT };

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

	maxBlurSamplesLocation = shaderProgram.GetUniform ( "maxBlurSamples" );
	inverseScreenResolutionLocation = shaderProgram.GetUniform ( "inverseScreenResolution" );

	SetMaxBlurSamples ( DEFAULT_MAX_BLUR_SAMPLES );
	SetScreenResolution ( DEFAULT_SCREEN_RESOLUTION_WIDTH, DEFAULT_SCREEN_RESOLUTION_HEIGHT );
}

EMVelocityTileMaxMaterial::~EMVelocityTileMaxMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMVelocityTileMaxMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !velocityBlurTexture ) return;
	
	glUseProgram ( shaderProgram.GetProgram () );
	glUniform1i ( maxBlurSamplesLocation, maxBlurSamples );
	glUniform2fv ( inverseScreenResolutionLocation, 1, inverseScreenResolution.arr );
	velocityBlurTexture->Bind ( VELOCITY_BLUR_SLOT );
}

GXVoid EMVelocityTileMaxMaterial::Unbind ()
{
	if ( !velocityBlurTexture ) return;
	
	glUseProgram ( 0 );
	velocityBlurTexture->Unbind ();
}

GXVoid EMVelocityTileMaxMaterial::SetVelocityBlurTexture ( GXTexture &texture )
{
	velocityBlurTexture = &texture;
}

GXVoid EMVelocityTileMaxMaterial::SetMaxBlurSamples ( GXUByte maxSamples )
{
	maxBlurSamples = (GXInt)maxSamples;
}

GXVoid EMVelocityTileMaxMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
	inverseScreenResolution.x = 1.0f / (GXFloat)width;
	inverseScreenResolution.y = 1.0f / (GXFloat)height;
}
