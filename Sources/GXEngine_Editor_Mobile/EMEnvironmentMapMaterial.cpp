#include <GXEngine_Editor_Mobile/EMEnvironmentMapMaterial.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>


#define ENVIRONMENT_SLOT                        0u
#define DEPTH_SLOT                              1u

#define DEFAULT_SCREEN_WIDTH                    1280.0f
#define DEFAULT_SCREEN_HEIGHT                   720.0f
#define DEFAULT_DELTA_TIME                      1.0f
#define DEFAULT_ENVIRONMENT_QUASI_DISTANCE      77.7f

#define ZERO_VELOCITY_BLUR_X                    0.0f
#define ZERO_VELOCITY_BLUR_Y                    0.0f

#define VERTEX_SHADER                           L"Shaders/Editor Mobile/EnvironmentMap_vs.txt"
#define GEOMETRY_SHADER                         nullptr
#define FRAGMENT_SHADER                         L"Shaders/Editor Mobile/EnvironmentMap_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMEnvironmentMapMaterial::EMEnvironmentMapMaterial () :
    _environmentTexture ( nullptr ),
    _depthTexture ( nullptr ),
    _environmentSampler ( GL_CLAMP_TO_EDGE, eGXResampling::Linear, 1.0f ),
    _depthSampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
    _screenResolution ( DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT ),
    _inverseScreenResolution ( 1.0f / DEFAULT_SCREEN_WIDTH, 1.0f / DEFAULT_SCREEN_HEIGHT ),
    _inverseDeltaTime ( 1.0f / DEFAULT_DELTA_TIME ),
    _environmentQuasiDistance ( DEFAULT_ENVIRONMENT_QUASI_DISTANCE )
{
    constexpr GLchar* samplerNames[ 2u ] = { "environmentSampler", "depthSampler" };
    constexpr GLuint samplerLocations[ 2u ] = { ENVIRONMENT_SLOT, DEPTH_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 2u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _modelViewProjectionMatrixLocation = _shaderProgram.GetUniform ( "modelViewProjectionMatrix" );
    _inverseScreenResolutionLocation = _shaderProgram.GetUniform ( "inverseScreenResolution" );
    _velocityBlurLocation = _shaderProgram.GetUniform ( "velocityBlur" );
}

EMEnvironmentMapMaterial::~EMEnvironmentMapMaterial ()
{
    // NOTHING
}

GXVoid EMEnvironmentMapMaterial::Bind ( const GXTransform &transform )
{
    if ( !_environmentTexture || !_depthTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    GXCamera* camera = GXCamera::GetActiveCamera ();
    const GXMat4& currentFrameViewProjectionMatrix = camera->GetCurrentFrameViewProjectionMatrix ();

    GXMat4 modelViewProjectionMatrix;
    modelViewProjectionMatrix.Multiply ( transform.GetCurrentFrameModelMatrix (), currentFrameViewProjectionMatrix );

    GXVec2 currentFrameEnvironmentQuasiLocationScreen;
    currentFrameEnvironmentQuasiLocationScreen.Multiply ( _screenResolution, 0.5f );

    GXVec3 currentFrameCameraLocationWorld;
    camera->GetLocation ( currentFrameCameraLocationWorld );

    const GXMat4& lastFrameCameraModelMatrix = camera->GetLastFrameModelMatrix ();
    GXVec3 lastFrameEnvironmentQuasiLocationWorld;
    lastFrameCameraModelMatrix.GetZ ( lastFrameEnvironmentQuasiLocationWorld );

    lastFrameEnvironmentQuasiLocationWorld.Multiply ( lastFrameEnvironmentQuasiLocationWorld, _environmentQuasiDistance );
    lastFrameEnvironmentQuasiLocationWorld.Sum ( lastFrameEnvironmentQuasiLocationWorld, currentFrameCameraLocationWorld );
    
    GXVec4 v ( lastFrameEnvironmentQuasiLocationWorld, 1.0f );
    GXVec4 temp;
    currentFrameViewProjectionMatrix.MultiplyVectorMatrix ( temp, v );

    GXVec2 lastFrameEnvironmentQuasiLocationScreen ( temp.GetX (), temp.GetY () );
    lastFrameEnvironmentQuasiLocationScreen.Multiply ( lastFrameEnvironmentQuasiLocationScreen, 1.0f / temp.GetW () );

    constexpr GXVec2 biasScale ( 0.5f, 0.5f );
    constexpr GXVec2 biasOffset ( 0.5f, 0.5f );

    lastFrameEnvironmentQuasiLocationScreen.Multiply ( lastFrameEnvironmentQuasiLocationScreen, biasScale );
    lastFrameEnvironmentQuasiLocationScreen.Sum ( lastFrameEnvironmentQuasiLocationScreen, biasOffset );

    lastFrameEnvironmentQuasiLocationScreen.Multiply ( lastFrameEnvironmentQuasiLocationScreen, _screenResolution );

    GXVec2 velocityImage;
    velocityImage.Substract ( currentFrameEnvironmentQuasiLocationScreen, lastFrameEnvironmentQuasiLocationScreen );
    velocityImage.Multiply ( velocityImage, _inverseDeltaTime );

    GXVec2 halfSpreadVelocityImage;
    EMRenderer& renderer = EMRenderer::GetInstance ();
    halfSpreadVelocityImage.Multiply ( velocityImage, 0.5f * renderer.GetMotionBlurExposure () );

    GXVec2 velocityBlur;

    if ( halfSpreadVelocityImage.GetX () == 0.0f && halfSpreadVelocityImage.GetY () == 0.0f )
    {
        velocityBlur.Init ( ZERO_VELOCITY_BLUR_X, ZERO_VELOCITY_BLUR_Y );
    }
    else
    {
        GXFloat maximumMotionBlurSamples = static_cast<GXFloat> ( renderer.GetMaximumMotionBlurSamples () );
        GXFloat halfSpreadVelocityMagnitudeImage = GXMinf ( halfSpreadVelocityImage.Length (), maximumMotionBlurSamples );

        velocityBlur = velocityImage;
        velocityBlur.Normalize ();
        velocityBlur.Multiply ( velocityBlur, halfSpreadVelocityMagnitudeImage / maximumMotionBlurSamples );
    }

    glUniformMatrix4fv ( _modelViewProjectionMatrixLocation, 1, GL_FALSE, modelViewProjectionMatrix._data );
    glUniform2fv ( _inverseScreenResolutionLocation, 1, _inverseScreenResolution._data );
    glUniform2fv ( _velocityBlurLocation, 1, velocityBlur._data );

    _environmentTexture->Bind ( ENVIRONMENT_SLOT );
    _environmentSampler.Bind ( ENVIRONMENT_SLOT );

    _depthTexture->Bind ( DEPTH_SLOT );
    _depthSampler.Bind ( DEPTH_SLOT );
}

GXVoid EMEnvironmentMapMaterial::Unbind ()
{
    if ( !_environmentTexture || !_depthTexture ) return;

    _environmentSampler.Unbind ( ENVIRONMENT_SLOT );
    _environmentTexture->Unbind ();

    _depthSampler.Unbind ( DEPTH_SLOT );
    _depthTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMEnvironmentMapMaterial::SetEnvironmentMap ( GXTextureCubeMap &cubeMap )
{
    _environmentTexture = &cubeMap;
}

GXVoid EMEnvironmentMapMaterial::SetDepthTexture ( GXTexture2D &texture )
{
    _depthTexture = &texture;
}

GXVoid EMEnvironmentMapMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
    _screenResolution.Init ( static_cast<GXFloat> ( width ), static_cast<GXFloat> ( height ) );
    _inverseScreenResolution.Init ( 1.0f / _screenResolution._data[ 0u ], 1.0f / _screenResolution._data[ 1u ] );
}

GXVoid EMEnvironmentMapMaterial::SetDeltaTime ( GXFloat seconds )
{
    _inverseDeltaTime = 1.0f / seconds;
}

GXVoid EMEnvironmentMapMaterial::SetEnvironmentQuasiDistance ( GXFloat meters )
{
    _environmentQuasiDistance = meters;
}
