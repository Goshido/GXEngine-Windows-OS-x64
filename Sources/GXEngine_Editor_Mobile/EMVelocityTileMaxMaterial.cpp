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
	sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
	maxBlurSamples ( DEFAULT_MAX_BLUR_SAMPLES ),
	inverseScreenResolution ( 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_RESOLUTION_WIDTH ), 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_RESOLUTION_HEIGHT ) )
{
	static const GLchar* samplerNames[ 1 ] = { "velocityBlurSampler" };
	static const GLuint samplerLocations[ 1 ] = { VELOCITY_BLUR_SLOT };

	GXShaderProgramInfo si;
	si._vertexShader = VERTEX_SHADER;
	si._geometryShader = GEOMETRY_SHADER;
	si._fragmentShader = FRAGMENT_SHADER;
	si._samplers = 1u;
	si._samplerNames = samplerNames;
	si._samplerLocations = samplerLocations;
	si._transformFeedbackOutputs = 0;
	si._transformFeedbackOutputNames = nullptr;

	_shaderProgram.Init ( si );

	maxBlurSamplesLocation = _shaderProgram.GetUniform ( "maxBlurSamples" );
	inverseScreenResolutionLocation = _shaderProgram.GetUniform ( "inverseScreenResolution" );
}

EMVelocityTileMaxMaterial::~EMVelocityTileMaxMaterial ()
{
	// NOTHING
}

GXVoid EMVelocityTileMaxMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !velocityBlurTexture ) return;
	
	glUseProgram ( _shaderProgram.GetProgram () );
	glUniform1i ( maxBlurSamplesLocation, maxBlurSamples );
	glUniform2fv ( inverseScreenResolutionLocation, 1, inverseScreenResolution.data );

	velocityBlurTexture->Bind ( VELOCITY_BLUR_SLOT );
	sampler.Bind ( VELOCITY_BLUR_SLOT );
}

GXVoid EMVelocityTileMaxMaterial::Unbind ()
{
	if ( !velocityBlurTexture ) return;
	
	sampler.Unbind ( VELOCITY_BLUR_SLOT );
	velocityBlurTexture->Unbind ();

	glUseProgram ( 0u );
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
