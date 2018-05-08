#include <GXEngine_Editor_Mobile/EMVelocityTileMaxMaterial.h>


#define DEFAULT_MAX_BLUR_SAMPLES			15
#define DEFAULT_SCREEN_RESOLUTION_WIDTH		1280u
#define DEFAULT_SCREEN_RESOLUTION_HEIGHT	720u

#define VELOCITY_BLUR_SLOT					0u

#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/VelocityTileMax_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMVelocityTileMaxMaterial::EMVelocityTileMaxMaterial ():
	velocityBlurTexture ( nullptr ),
	maxBlurSamples ( DEFAULT_MAX_BLUR_SAMPLES ),
	inverseScreenResolution ( 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_RESOLUTION_WIDTH ), 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_RESOLUTION_HEIGHT ) )
{
	static const GLchar* samplerNames[ 1 ] = { "velocityBlurSampler" };
	static const GLuint samplerLocations[ 1 ] = { VELOCITY_BLUR_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 1u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	maxBlurSamplesLocation = shaderProgram.GetUniform ( "maxBlurSamples" );
	inverseScreenResolutionLocation = shaderProgram.GetUniform ( "inverseScreenResolution" );
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
	glUniform2fv ( inverseScreenResolutionLocation, 1, inverseScreenResolution.data );
	velocityBlurTexture->Bind ( VELOCITY_BLUR_SLOT );
}

GXVoid EMVelocityTileMaxMaterial::Unbind ()
{
	if ( !velocityBlurTexture ) return;
	
	glUseProgram ( 0u );
	velocityBlurTexture->Unbind ();
}

GXVoid EMVelocityTileMaxMaterial::SetVelocityBlurTexture ( GXTexture2D &texture )
{
	velocityBlurTexture = &texture;
}

GXVoid EMVelocityTileMaxMaterial::SetMaxBlurSamples ( GXUByte maxSamples )
{
	maxBlurSamples = static_cast<GXInt> ( maxSamples );
}

GXVoid EMVelocityTileMaxMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
	inverseScreenResolution.data[ 0 ] = 1.0f / static_cast<GXFloat> ( width );
	inverseScreenResolution.data[ 1 ] = 1.0f / static_cast<GXFloat> ( height );
}
