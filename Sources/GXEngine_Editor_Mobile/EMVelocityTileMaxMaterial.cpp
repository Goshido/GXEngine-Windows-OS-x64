#include <GXEngine_Editor_Mobile/EMVelocityTileMaxMaterial.h>


#define DEFAULT_MAX_BLUR_SAMPLES            15
#define DEFAULT_SCREEN_RESOLUTION_WIDTH     1280u
#define DEFAULT_SCREEN_RESOLUTION_HEIGHT    720u

#define VELOCITY_BLUR_SLOT                  0u

#define VERTEX_SHADER                       L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER                     nullptr
#define FRAGMENT_SHADER                     L"Shaders/Editor Mobile/VelocityTileMax_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMVelocityTileMaxMaterial::EMVelocityTileMaxMaterial ():
    _velocityBlurTexture ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
    _maxBlurSamples ( DEFAULT_MAX_BLUR_SAMPLES ),
    _inverseScreenResolution ( 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_RESOLUTION_WIDTH ), 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_RESOLUTION_HEIGHT ) )
{
    constexpr GLchar* samplerNames[ 1u ] = { "velocityBlurSampler" };
    constexpr GLuint samplerLocations[ 1u ] = { VELOCITY_BLUR_SLOT };

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

    _maxBlurSamplesLocation = _shaderProgram.GetUniform ( "maxBlurSamples" );
    _inverseScreenResolutionLocation = _shaderProgram.GetUniform ( "inverseScreenResolution" );
}

EMVelocityTileMaxMaterial::~EMVelocityTileMaxMaterial ()
{
    // NOTHING
}

GXVoid EMVelocityTileMaxMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_velocityBlurTexture ) return;
    
    glUseProgram ( _shaderProgram.GetProgram () );
    glUniform1i ( _maxBlurSamplesLocation, _maxBlurSamples );
    glUniform2fv ( _inverseScreenResolutionLocation, 1, _inverseScreenResolution._data );

    _velocityBlurTexture->Bind ( VELOCITY_BLUR_SLOT );
    _sampler.Bind ( VELOCITY_BLUR_SLOT );
}

GXVoid EMVelocityTileMaxMaterial::Unbind ()
{
    if ( !_velocityBlurTexture ) return;
    
    _sampler.Unbind ( VELOCITY_BLUR_SLOT );
    _velocityBlurTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMVelocityTileMaxMaterial::SetVelocityBlurTexture ( GXTexture2D &texture )
{
    _velocityBlurTexture = &texture;
}

GXVoid EMVelocityTileMaxMaterial::SetMaxBlurSamples ( GXUByte maxSamples )
{
    _maxBlurSamples = static_cast<GXInt> ( maxSamples );
}

GXVoid EMVelocityTileMaxMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
    _inverseScreenResolution._data[ 0u ] = 1.0f / static_cast<GXFloat> ( width );
    _inverseScreenResolution._data[ 1u ] = 1.0f / static_cast<GXFloat> ( height );
}
