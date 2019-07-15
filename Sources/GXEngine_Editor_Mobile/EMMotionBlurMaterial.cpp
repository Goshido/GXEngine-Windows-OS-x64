#include <GXEngine_Editor_Mobile/EMMotionBlurMaterial.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_DEPTH_LIMIT             1.0f
#define DEFAULT_MAX_BLUR_SAMPLES        15u
#define DEFAULT_SCREEN_WIDTH            1280u
#define DEFAULT_SCREEN_HEIGHT           720u

#define VELOCITY_NEIGHBOR_MAX_SLOT      0u
#define VELOCITY_SLOT                   1u
#define DEPTH_SLOT                      2u
#define IMAGE_SLOT                      3u

#define VERTEX_SHADER                   L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER                 nullptr
#define FRAGMENT_SHADER                 L"Shaders/Editor Mobile/MotionBlur_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMMotionBlurMaterial::EMMotionBlurMaterial ():
    _velocityNeighborMaxTexture ( nullptr ),
    _velocityTexture ( nullptr ),
    _depthTexture ( nullptr ),
    _imageTexture ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
    _depthLimit ( DEFAULT_DEPTH_LIMIT ),
    _inverseDepthLimit ( 1.0f / DEFAULT_DEPTH_LIMIT ),
    _maxBlurSamples ( static_cast<GXFloat> ( DEFAULT_MAX_BLUR_SAMPLES ) ),
    _inverseScreenResolution ( 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_WIDTH ), 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_WIDTH ) )
{
    constexpr GLchar* samplerNames[ 4u ] = { "velocityNeighborMaxSampler", "velocitySampler", "depthSampler", "imageSampler" };
    constexpr GLuint samplerLocations[ 4u ] = { VELOCITY_NEIGHBOR_MAX_SLOT, VELOCITY_SLOT, DEPTH_SLOT, IMAGE_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 4u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _inverseDepthLimitLocation = _shaderProgram.GetUniform ( "inverseDepthLimit" );
    _maxBlurSamplesLocation = _shaderProgram.GetUniform ( "maxBlurSamples" );
    _inverseScreenResolutionLocation = _shaderProgram.GetUniform ( "inverseScreenResolution" );
    _inverseProjectionMatrixLocation = _shaderProgram.GetUniform ( "inverseProjectionMatrix" );
}

EMMotionBlurMaterial::~EMMotionBlurMaterial ()
{
    // NOTHING
}

GXVoid EMMotionBlurMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_velocityNeighborMaxTexture || !_velocityTexture || !_depthTexture || !_imageTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    const GXMat4& inverseProjectionMatrix = GXCamera::GetActiveCamera ()->GetCurrentFrameInverseProjectionMatrix ();

    glUniform1f ( _inverseDepthLimitLocation, _inverseDepthLimit );
    glUniform1f ( _maxBlurSamplesLocation, _maxBlurSamples );
    glUniform2fv ( _inverseScreenResolutionLocation, 1, _inverseScreenResolution._data );
    glUniformMatrix4fv ( _inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix._data );

    _velocityNeighborMaxTexture->Bind ( VELOCITY_NEIGHBOR_MAX_SLOT );
    _sampler.Bind ( VELOCITY_NEIGHBOR_MAX_SLOT );

    _velocityTexture->Bind ( VELOCITY_SLOT );
    _sampler.Bind ( VELOCITY_SLOT );

    _depthTexture->Bind ( DEPTH_SLOT );
    _sampler.Bind ( DEPTH_SLOT );

    _imageTexture->Bind ( IMAGE_SLOT );
    _sampler.Bind ( IMAGE_SLOT );
}

GXVoid EMMotionBlurMaterial::Unbind ()
{
    if ( !_velocityNeighborMaxTexture || !_velocityTexture || !_depthTexture || !_imageTexture ) return;

    _sampler.Unbind ( VELOCITY_NEIGHBOR_MAX_SLOT );
    _velocityNeighborMaxTexture->Unbind ();

    _sampler.Unbind ( VELOCITY_SLOT );
    _velocityTexture->Unbind ();

    _sampler.Unbind ( DEPTH_SLOT );
    _depthTexture->Unbind ();

    _sampler.Unbind ( IMAGE_SLOT );
    _imageTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMMotionBlurMaterial::SetVelocityNeighborMaxTexture ( GXTexture2D &texture )
{
    _velocityNeighborMaxTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetVelocityTexture ( GXTexture2D &texture )
{
    _velocityTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetDepthTexture ( GXTexture2D &texture )
{
    _depthTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetImageTexture ( GXTexture2D &texture )
{
    _imageTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetDepthLimit ( GXFloat meters )
{
    if ( meters <= 0.0f )
    {
        GXLogA ( "EMMotionBlurMaterial::SetDepthLimit::Error - ”казана не положительна€ отсечка по глубине!\n" );
        return;
    }

    _depthLimit = meters;
    _inverseDepthLimit = 1.0f / meters;
}

GXFloat EMMotionBlurMaterial::GetDepthLimit () const
{
    return _depthLimit;
}

GXVoid EMMotionBlurMaterial::SetMaxBlurSamples ( GXUByte samples )
{
    if ( samples == 0u )
    {
        GXLogA ( "EMMotionBlurMaterial::SetMaxBlurSamples::Error - ”казано нулевое максимальное количество выборок!\n" );
        return;
    }

    _maxBlurSamples = static_cast<GXFloat> ( samples );
}

GXUByte EMMotionBlurMaterial::GetMaxBlurSamples () const
{
    return static_cast<GXUByte> ( _maxBlurSamples );
}

GXVoid EMMotionBlurMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
    _inverseScreenResolution._data[ 0u ] = 1.0f / static_cast<GXFloat> ( width );
    _inverseScreenResolution._data[ 1u ] = 1.0f / static_cast<GXFloat> ( height );
}
