#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine_Editor_Mobile/EMLightProbe.h>
#include <GXEngine_Editor_Mobile/EMUIMotionBlurSettings.h>
#include <GXEngine_Editor_Mobile/EMUISSAOSettings.h>
#include <GXEngine_Editor_Mobile/EMUIToneMapperSettings.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXLogger.h>


#define EM_OBJECT_HI_INDEX                              14
#define EM_OBJECT_LOW_INDEX                             15

#define OUT_TEXTURE_SLOT                                0u

#define DIFFUSE_SLOT                                    0u
#define NORMAL_SLOT                                     1u
#define SPECULAR_SLOT                                   2u
#define EMISSION_SLOT                                   3u
#define DEPTH_SLOT                                      4u

#define Z_NEAR                                          0.0f
#define Z_FAR                                           77.0f
#define Z_RENDER                                        1.0f

#define CVV_WIDTH                                       2.0f
#define CVV_HEIGHT                                      2.0f

#define DEFAULT_MAX_MOTION_BLUR_SAMPLES                 15u
#define DEFAULT_MOTION_BLUR_DEPTH_LIMIT                 0.1f
#define DEFAULT_MOTION_BLUR_EXPLOSURE                   0.03f

#define DEFAULT_SSAO_MAX_CHECK_RADIUS                   0.3f
#define DEFAULT_SSAO_SAMPLES                            32u
#define DEFAULT_SSAO_NOISE_TEXTURE_RESOLUTION           9u
#define DEFAULT_SSAO_MAX_DISTANCE                       1000.0f

#define DEFAULT_TONE_MAPPER_GAMMA                       2.2f
#define DEFAULT_TONE_MAPPER_EYE_ADAPTATION_SPEED        1.25f
#define DEFAULT_TONE_MAPPER_EYE_SENSITIVITY             7.2e-3f
#define DEFAULT_TONE_MAPPER_ABSOLUTE_WHITE_INTENSITY    1.0e+4f

#define CLEAR_DIFFUSE_R                                 0.0f
#define CLEAR_DIFFUSE_G                                 0.0f
#define CLEAR_DIFFUSE_B                                 0.0f
#define CLEAR_DIFFUSE_A                                 1.0f

#define CLEAR_NORMAL_R                                  0.5f
#define CLEAR_NORMAL_G                                  0.5f
#define CLEAR_NORMAL_B                                  0.5f
#define CLEAR_NORMAL_A                                  0.0f

#define CLEAR_EMISSION_R                                0.0f
#define CLEAR_EMISSION_G                                0.0f
#define CLEAR_EMISSION_B                                0.0f
#define CLEAR_EMISSION_A                                0.0f

#define CLEAR_PARAMETER_ROUGNESS                        0.5f
#define CLEAR_PARAMETER_IOR                             0.5f
#define CLEAR_PARAMETER_SPECULAR_INTENCITY              0.0f
#define CLEAR_PARAMETER_METALLIC                        0.0f

#define CLEAR_VELOCITY_BLUR_R                           0.5f
#define CLEAR_VELOCITY_BLUR_G                           0.5f
#define CLEAR_VELOCITY_BLUR_B                           0.5f
#define CLEAR_VELOCITY_BLUR_A                           0.5f

#define CLEAR_OBJECT_0_R                                0.0f
#define CLEAR_OBJECT_0_G                                0.0f
#define CLEAR_OBJECT_0_B                                0.0f
#define CLEAR_OBJECT_0_A                                0.0f

#define CLEAR_OBJECT_1_R                                0.0f
#define CLEAR_OBJECT_1_G                                0.0f
#define CLEAR_OBJECT_1_B                                0.0f
#define CLEAR_OBJECT_1_A                                0.0f

#define OVERLAY_TRANSPARENCY                            180u

#define INVALID_LUMINANCE                               -1.0f

//---------------------------------------------------------------------------------------------------------------------

EMRenderer* EMRenderer::_instance = nullptr;

EMRenderer::~EMRenderer ()
{
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    glBindFramebuffer ( GL_FRAMEBUFFER, 0u );
    glDeleteFramebuffers ( 1, &_fbo );

    delete &( EMUIToneMapperSettings::GetInstance () );
    delete &( EMUISSAOSettings::GetInstance () );
    delete &( EMUIMotionBlurSettings::GetInstance () );

    _albedoTexture.FreeResources ();
    _normalTexture.FreeResources ();
    _emissionTexture.FreeResources ();
    _parameterTexture.FreeResources ();
    _velocityBlurTexture.FreeResources ();
    _velocityTileMaxTexture.FreeResources ();
    _velocityNeighborMaxTexture.FreeResources ();
    _ssaoOmegaTexture.FreeResources ();
    _ssaoYottaTexture.FreeResources ();
    _objectTextures[ 0u ].FreeResources ();
    _objectTextures[ 1u ].FreeResources ();
    _importantAreaTexture.FreeResources ();
    _depthStencilTexture.FreeResources ();
    _omegaTexture.FreeResources ();
    _yottaTexture.FreeResources ();

    _instance = nullptr;
}

GXVoid EMRenderer::StartCommonPass ()
{
    if ( ( _mouseX != -1 || _mouseY != -1 ) && _onObject )
    {
        _onObject ( _onObjectContext, SampleObject () );
        _mouseX = _mouseY = -1;
    }

    if ( _isMotionBlurSettingsChanged )
        UpdateMotionBlurSettings ();

    if ( _isSSAOSettingsChanged )
        UpdateSSAOSettings ();

    if ( _isToneMapperSettingsChanged )
        UpdateToneMapperSettings ();

    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _albedoTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _normalTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, _emissionTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, _parameterTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, _velocityBlurTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, _objectTextures[ 0u ].GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, _objectTextures[ 1u ].GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, _depthStencilTexture.GetTextureObject (), 0 );

    constexpr GLenum buffers[ 7u ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
    glDrawBuffers ( 7, buffers );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

    glDepthMask ( GL_TRUE );
    glStencilMask ( 0xFFu );

    glClearDepth ( 1.0f );
    glClearStencil ( 0x00u );

    glEnable ( GL_CULL_FACE );
    glEnable ( GL_DEPTH_TEST );
    glDisable ( GL_BLEND );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::StartCommonPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    constexpr GXFloat clearDiffuseValue[ 4u ] = { CLEAR_DIFFUSE_R, CLEAR_DIFFUSE_G, CLEAR_DIFFUSE_B, CLEAR_DIFFUSE_A };
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glClearBufferfv ( GL_COLOR, 0, clearDiffuseValue );

    constexpr GXFloat clearNormalValue[ 4u ] = { CLEAR_NORMAL_R, CLEAR_NORMAL_G, CLEAR_NORMAL_B, CLEAR_NORMAL_A };
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
    glClearBufferfv ( GL_COLOR, 1, clearNormalValue );

    constexpr GXFloat clearEmissionValue[ 4u ] = { CLEAR_EMISSION_R, CLEAR_EMISSION_G, CLEAR_EMISSION_B, CLEAR_EMISSION_A };
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
    glClearBufferfv ( GL_COLOR, 2, clearEmissionValue );

    constexpr GXFloat clearSpecularValue[ 4u ] = { CLEAR_PARAMETER_ROUGNESS, CLEAR_PARAMETER_IOR, CLEAR_PARAMETER_SPECULAR_INTENCITY, CLEAR_PARAMETER_METALLIC };
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glClearBufferfv ( GL_COLOR, 3, clearSpecularValue );

    constexpr GXFloat clearVelocityBlurValue[ 4u ] = { CLEAR_VELOCITY_BLUR_R, CLEAR_VELOCITY_BLUR_G, CLEAR_VELOCITY_BLUR_B, CLEAR_VELOCITY_BLUR_A };
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
    glClearBufferfv ( GL_COLOR, 4, clearVelocityBlurValue );

    constexpr GXFloat clearObject0Value[ 4u ] = { CLEAR_OBJECT_0_R, CLEAR_OBJECT_0_G, CLEAR_OBJECT_0_B, CLEAR_OBJECT_0_A };
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glClearBufferfv ( GL_COLOR, 5, clearObject0Value );

    constexpr GXFloat clearObject1Value[ 4u ] = { CLEAR_OBJECT_1_R, CLEAR_OBJECT_1_G, CLEAR_OBJECT_1_B, CLEAR_OBJECT_1_A };
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glClearBufferfv ( GL_COLOR, 6, clearObject1Value );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
}

GXVoid EMRenderer::StartEnvironmentPass ()
{
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _emissionTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _velocityBlurTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
    glDepthMask ( GX_FALSE );
    glStencilMask ( 0x00u );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

    constexpr GLenum buffers[ 2u ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers ( 2, buffers );

    glDisable ( GL_BLEND );
    glDisable ( GL_DEPTH_TEST );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status == GL_FRAMEBUFFER_COMPLETE ) return;

    GXLogA ( "EMRenderer::StartEnvironmentPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );
}

GXVoid EMRenderer::StartLightPass ()
{
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _omegaTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
    glDepthMask ( GX_FALSE );
    glStencilMask ( 0x00u );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    glDisable ( GL_CULL_FACE );
    glDisable ( GL_DEPTH_TEST );
    glEnable ( GL_BLEND );
    glBlendFunc ( GL_ONE, GL_ONE );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::StartLightPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    glClear ( GL_COLOR_BUFFER_BIT );

    LightUp ();
}

GXVoid EMRenderer::StartHudColorPass ()
{
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _yottaTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, _depthStencilTexture.GetTextureObject (), 0 );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
    glDepthMask ( GX_TRUE );
    glStencilMask ( 0xFFu );

    glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable ( GL_DEPTH_TEST );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status == GL_FRAMEBUFFER_COMPLETE ) return;

    GXLogA ( "EMRenderer::StartHudDepthDependentPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );
}

GXVoid EMRenderer::StartHudMaskPass ()
{
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _objectTextures[ 0u ].GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _objectTextures[ 1u ].GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, _depthStencilTexture.GetTextureObject (), 0 );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDepthMask ( GX_TRUE );
    glStencilMask ( 0xFFu );

    glDisable ( GL_BLEND );
    glEnable ( GL_CULL_FACE );
    glEnable ( GL_DEPTH_TEST );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

    constexpr GLenum buffers[ 2u ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers ( 2, buffers );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::StartHudDepthDependentPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

GXVoid EMRenderer::SetObjectMask ( GXVoid* object )
{
    GXUPointer obj = reinterpret_cast<GXUPointer> ( object );
    _objectMask[ 0u ] = static_cast<GXUByte> ( ( obj & 0xFF00000000000000u ) >> 56 );
    _objectMask[ 1u ] = static_cast<GXUByte> ( ( obj & 0x00FF000000000000u ) >> 48 );
    _objectMask[ 2u ] = static_cast<GXUByte> ( ( obj & 0x0000FF0000000000u ) >> 40 );
    _objectMask[ 3u ] = static_cast<GXUByte> ( ( obj & 0x000000FF00000000u ) >> 32 );
    _objectMask[ 4u ] = static_cast<GXUByte> ( ( obj & 0x00000000FF000000u ) >> 24 );
    _objectMask[ 5u ] = static_cast<GXUByte> ( ( obj & 0x0000000000FF0000u ) >> 16 );
    _objectMask[ 6u ] = static_cast<GXUByte> ( ( obj & 0x000000000000FF00u ) >> 8 );
    _objectMask[ 7u ] = static_cast<GXUByte> ( obj & 0x00000000000000FFu );

    glVertexAttrib4Nub ( EM_OBJECT_HI_INDEX, _objectMask[ 0u ], _objectMask[ 1u ], _objectMask[ 2u ], _objectMask[ 3u ] );
    glVertexAttrib4Nub ( EM_OBJECT_LOW_INDEX, _objectMask[ 4u ], _objectMask[ 5u ], _objectMask[ 6u ], _objectMask[ 7u ] );
}

GXVoid EMRenderer::ApplySSAO ()
{
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _ssaoOmegaTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    glColorMask ( GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE );
    glDepthMask ( GL_FALSE );
    glStencilMask ( 0x00 );

    glDisable ( GL_BLEND );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );

    glViewport ( 0, 0, static_cast<GLsizei> ( _ssaoOmegaTexture.GetWidth () ), static_cast<GLsizei> ( _ssaoOmegaTexture.GetHeight () ) );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::ApplySSAO::Error - Что-то не так с FBO на первом проходе (ошибка 0x%08x)\n", status );

    const GXTransform& nullTransform = GXTransform::GetNullTransform ();

    _ssaoSharpMaterial.Bind ( nullTransform );
    _screenQuadMesh.Render ();
    _ssaoSharpMaterial.Unbind ();

    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _ssaoYottaTexture.GetTextureObject (), 0 );

    status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::ApplySSAO::Error - Что-то не так с FBO на втором проходе (ошибка 0x%08x)\n", status );

    _gaussHorizontalBlurMaterial.Bind ( nullTransform );
    _screenQuadMesh.Render ();
    _gaussHorizontalBlurMaterial.Unbind ();

    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _ssaoOmegaTexture.GetTextureObject (), 0 );

    status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::ApplySSAO::Error - Что-то не так с FBO на третьем проходе (ошибка 0x%08x)\n", status );

    _gaussVerticalBlurMaterial.Bind ( nullTransform );
 _screenQuadMesh.Render ();
    _gaussVerticalBlurMaterial.Unbind ();

    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _yottaTexture.GetTextureObject (), 0 );
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );

    status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::ApplySSAO::Error - Что-то не так с FBO на четвёртом проходе (ошибка 0x%08x)\n", status );

    _ssaoApplyMaterial.Bind ( nullTransform );
 _screenQuadMesh.Render ();
    _ssaoApplyMaterial.Unbind ();
}

GXVoid EMRenderer::ApplyMotionBlur ()
{
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _velocityTileMaxTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    glColorMask ( GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE );
    glDepthMask ( GL_FALSE );
    glStencilMask ( 0x00u );

    glDisable ( GL_BLEND );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );

    glViewport ( 0, 0, static_cast<GLsizei> ( _velocityTileMaxTexture.GetWidth () ), static_cast<GLsizei> ( _velocityTileMaxTexture.GetHeight () ) );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::ApplyMotionBlur::Error - Что-то не так с FBO на первом проходе (ошибка 0x%08x)\n", status );

    _velocityTileMaxMaterial.Bind ( GXTransform::GetNullTransform () );
 _screenQuadMesh.Render ();
    _velocityTileMaxMaterial.Unbind ();

    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _velocityNeighborMaxTexture.GetTextureObject (), 0 );

    status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::ApplyMotionBlur::Error - Что-то не так с FBO на втором проходе (ошибка 0x%08x)\n", status );

    _velocityNeighborMaxMaterial.Bind ( GXTransform::GetNullTransform () );
 _screenQuadMesh.Render ();
    _velocityNeighborMaxMaterial.Unbind ();

    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _omegaTexture.GetTextureObject (), 0 );
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
    glViewport ( 0, 0, static_cast<GLsizei> ( _omegaTexture.GetWidth () ), static_cast<GLsizei> ( _omegaTexture.GetHeight () ) );

    status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::ApplyMotionBlur::Error - Что-то не так с FBO на третьем проходе (ошибка 0x%08x)\n", status );

    _motionBlurMaterial.Bind ( GXTransform::GetNullTransform () );
 _screenQuadMesh.Render ();
    _motionBlurMaterial.Unbind ();
}

GXVoid EMRenderer::ApplyToneMapping ( GXFloat deltaTime )
{
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _importantAreaTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
    glDepthMask ( GL_FALSE );
    glStencilMask ( 0x00 );

    glDisable ( GL_BLEND );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );

    GLsizei width = static_cast<GLsizei> ( _importantAreaTexture.GetWidth () );
    GLsizei height = static_cast<GLsizei> ( _importantAreaTexture.GetHeight () );
    glViewport ( 0, 0, width, height );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::ApplyToneMapping::Error - Что-то не так с FBO на проходе фильтрации важных областей (ошибка 0x%08x)\n", status );

    const GXTransform& nullTransform = GXTransform::GetNullTransform ();

    _importantAreaFilterMaterial.Bind ( nullTransform );
 _screenQuadMesh.Render ();
    _importantAreaFilterMaterial.Unbind ();

    const GXUByte reducingSteps = _importantAreaTexture.GetLevelOfDetailNumber ();

    for ( GXUByte i = 1u; i < reducingSteps; ++i )
    {
        width /= 2;

        if ( width == 0 )
            width = 1;

        height /= 2;

        if ( height == 0 )
            height = 1;

        glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _importantAreaTexture.GetTextureObject (), static_cast<GLint> ( i ) );

        glViewport ( 0, 0, width, height );

        status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

        if ( status != GL_FRAMEBUFFER_COMPLETE )
        {
            GXLogA ( "EMRenderer::ApplyToneMapping::Error - Что-то не так с FBO на проходе редуцирования %hhu (ошибка 0x%08x)\n", i, status );
            continue;
        }

        _toneMapperLuminanceTripletReducerMaterial.SetLevelOfDetailToReduce ( static_cast<GXUByte> ( i - 1u ) );
        _toneMapperLuminanceTripletReducerMaterial.Bind ( nullTransform );
     _screenQuadMesh.Render ();
        _toneMapperLuminanceTripletReducerMaterial.Unbind ();
    }

    glBindFramebuffer ( GL_READ_FRAMEBUFFER, _fbo );
    glPixelStorei ( GL_PACK_ALIGNMENT, 2 );
    glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _importantAreaTexture.GetTextureObject (), (GLint)( _importantAreaTexture.GetLevelOfDetailNumber () - 1 ) );
    glReadBuffer ( GL_COLOR_ATTACHMENT0 );

    GXVec3 luminanceTriplet;
    glReadPixels ( 0, 0, 1, 1, GL_RGB, GL_FLOAT, luminanceTriplet._data );

    glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0u, 0 );
    glReadBuffer ( GL_NONE );
    glBindFramebuffer ( GL_READ_FRAMEBUFFER, 0 );

    if ( _toneMapperEffectiveLuminanceTriplet.GetX () == INVALID_LUMINANCE )
    {
        _toneMapperEffectiveLuminanceTriplet = luminanceTriplet;
    }
    else
    {
        GXVec3 delta;
        delta.Substract ( luminanceTriplet, _toneMapperEffectiveLuminanceTriplet );
        delta.Multiply ( delta, _toneMapperEyeAdaptationSpeed * deltaTime );
        _toneMapperEffectiveLuminanceTriplet.Sum ( _toneMapperEffectiveLuminanceTriplet, delta );
    }

    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _yottaTexture.GetTextureObject (), 0 );
    glViewport ( 0, 0, static_cast<GLsizei> ( _yottaTexture.GetWidth () ), static_cast<GLsizei> ( _yottaTexture.GetHeight () ) );
    status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::ApplyToneMapping::Error - Что-то не так с FBO на проходе тонирования (ошибка 0x%08x)\n", status );

    _toneMapperMaterial.SetLuminanceTriplet ( _toneMapperEffectiveLuminanceTriplet.GetX (), _toneMapperEffectiveLuminanceTriplet.GetY (), _toneMapperEffectiveLuminanceTriplet.GetZ () );
    _toneMapperMaterial.Bind ( nullTransform );
 _screenQuadMesh.Render ();
    _toneMapperMaterial.Unbind ();
}

GXVoid EMRenderer::PresentFrame ( eEMRenderTarget target )
{
    GXCamera* oldCamera = GXCamera::GetActiveCamera ();
    GXCamera::SetActiveCamera ( &_outCamera );

    glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );
    glDisable ( GL_BLEND );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDepthMask ( GL_TRUE );
    glStencilMask ( 0xFFu );
    glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );

    glDrawBuffer ( GL_BACK );

    glClear ( GL_COLOR_BUFFER_BIT );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

    _unlitMaterial.SetTexture ( _yottaTexture );
    _unlitMaterial.SetColor ( 255u, 255u, 255u, 255u );
    _unlitMaterial.Bind ( _screenQuadMesh );

 _screenQuadMesh.Render ();

    _unlitMaterial.Unbind ();

    if ( target == eEMRenderTarget::Combine )
    {
        GXCamera::SetActiveCamera ( oldCamera );
        return;
    }

    GXTexture2D* texture = nullptr;

    switch ( target )
    {
        case eEMRenderTarget::Albedo:
            texture = &_albedoTexture;
        break;

        case eEMRenderTarget::Normal:
            texture = &_normalTexture;
        break;

        case eEMRenderTarget::Emission:
            texture = &_emissionTexture;
        break;

        case eEMRenderTarget::Parameter:
            texture = &_parameterTexture;
        break;

        case eEMRenderTarget::VelocityBlur:
            texture = &_velocityBlurTexture;
        break;

        case eEMRenderTarget::VelocityTileMax:
            texture = &_velocityTileMaxTexture;
        break;

        case eEMRenderTarget::VelocityNeighborMax:
            texture = &_velocityNeighborMaxTexture;
        break;

        case eEMRenderTarget::Depth:
            texture = &_depthStencilTexture;
        break;

        case eEMRenderTarget::SSAO:
            texture = &_ssaoOmegaTexture;
        break;

        case eEMRenderTarget::Combine:
            // NOTHING
        break;
    }

    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    _unlitMaterial.SetTexture ( *texture );
    _unlitMaterial.SetColor ( 255u, 255u, 255u, OVERLAY_TRANSPARENCY );
    _unlitMaterial.Bind ( _screenQuadMesh );

 _screenQuadMesh.Render ();

    _unlitMaterial.Unbind ();

    GXCamera::SetActiveCamera ( oldCamera );
}

GXVoid EMRenderer::SetOnObjectCallback ( GXVoid* context, EMRendererOnObjectHandler callback )
{
    _onObject = callback;
    _onObjectContext = context;
}

GXVoid EMRenderer::GetObject ( GXUShort x, GXUShort y )
{
    _mouseX = x;
    _mouseY = y;
}

GXVoid EMRenderer::SetMaximumMotionBlurSamples ( GXUByte samples )
{
    _newMaxMotionBlurSamples = samples;

    if ( _motionBlurMaterial.GetMaxBlurSamples () == samples ) return;

    _isMotionBlurSettingsChanged = GX_TRUE;
}

GXUByte EMRenderer::GetMaximumMotionBlurSamples () const
{
    return _motionBlurMaterial.GetMaxBlurSamples ();
}

GXVoid EMRenderer::SetMotionBlurDepthLimit ( GXFloat meters )
{
    _newMotionBlurDepthLimit = meters;

    if ( _motionBlurMaterial.GetDepthLimit () == meters ) return;

    _isMotionBlurSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetMotionBlurDepthLimit () const
{
    return _motionBlurMaterial.GetDepthLimit ();
}

GXVoid EMRenderer::SetMotionBlurExposure ( GXFloat seconds )
{
    if ( _motionBlurExposure == seconds ) return;

    _newMotionBlurExposure = seconds;
    _isMotionBlurSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetMotionBlurExposure () const
{
    return _motionBlurExposure;
}

GXVoid EMRenderer::SetSSAOCheckRadius ( GXFloat meters )
{
    _newSSAOCheckRadius = meters;

    if ( _ssaoSharpMaterial.GetCheckRadius () == meters ) return;

    _isSSAOSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetSSAOCheckRadius () const
{
    return _ssaoSharpMaterial.GetCheckRadius ();
}

GXVoid EMRenderer::SetSSAOSampleNumber ( GXUByte samples )
{
    _newSSAOSamples = samples;

    if ( _ssaoSharpMaterial.GetSampleNumber () == samples ) return;

    _isSSAOSettingsChanged = GX_TRUE;
}

GXUByte EMRenderer::GetSSAOSampleNumber () const
{
    return _ssaoSharpMaterial.GetSampleNumber ();
}

GXVoid EMRenderer::SetSSAONoiseTextureResolution ( GXUShort resolution )
{
    _newSSAONoiseTextureResolution = resolution;

    if ( _ssaoSharpMaterial.GetNoiseTextureResolution () == resolution ) return;

    _isSSAOSettingsChanged = GX_TRUE;
}

GXUShort EMRenderer::GetSSAONoiseTextureResolution () const
{
    return _ssaoSharpMaterial.GetNoiseTextureResolution ();
}

GXVoid EMRenderer::SetSSAOMaximumDistance ( GXFloat meters )
{
    _newSSAOMaxDistance = meters;

    if ( _ssaoSharpMaterial.GetMaximumDistance () == meters ) return;

    _isSSAOSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetSSAOMaximumDistance () const
{
    return _ssaoSharpMaterial.GetMaximumDistance ();
}

GXVoid EMRenderer::SetToneMapperGamma ( GXFloat gamma )
{
    _newToneMapperGamma = gamma;

    if ( _toneMapperMaterial.GetGamma () == gamma ) return;

    _isToneMapperSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetToneMapperGamma () const
{
    return _toneMapperMaterial.GetGamma ();
}

GXVoid EMRenderer::SetToneMapperEyeAdaptationSpeed ( GXFloat speed )
{
    _toneMapperEyeAdaptationSpeed = speed;
}

GXFloat EMRenderer::GetToneMapperEyeAdaptationSpeed () const
{
    return _toneMapperEyeAdaptationSpeed;
}

GXVoid EMRenderer::SetToneMapperEyeSensitivity ( GXFloat sensitivity )
{
    _newToneMapperEyeSensitivity = sensitivity;

    if ( _toneMapperMaterial.GetEyeSensitivity () == sensitivity ) return;

    _isToneMapperSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetToneMapperEyeSensitivity () const
{
    return _toneMapperMaterial.GetEyeSensitivity ();
}

GXVoid EMRenderer::SetToneMapperAbsoluteWhiteIntensity ( GXFloat intensity )
{
    _newToneMapperAbsoluteWhiteIntensity = intensity;

    if ( _toneMapperMaterial.GetAbsoluteWhiteIntensity () == intensity ) return;

    _isToneMapperSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetToneMapperAbsoluteWhiteIntensity () const
{
    return _toneMapperMaterial.GetAbsoluteWhiteIntensity ();
}

EMRenderer& EMRenderer::GetInstance ()
{
    if ( !_instance )
        _instance = new EMRenderer ();

    return *_instance;
}

GXTexture2D& EMRenderer::GetDepthTexture ()
{
    return _depthStencilTexture;
}

EMRenderer::EMRenderer ():
    _mouseX ( -1 ),
    _mouseY ( -1 ),
    _onObject ( nullptr ),
    _onObjectContext ( nullptr ),
    _newMaxMotionBlurSamples ( static_cast<GXUByte> ( DEFAULT_MAX_MOTION_BLUR_SAMPLES ) ),
    _newMotionBlurDepthLimit ( DEFAULT_MOTION_BLUR_DEPTH_LIMIT ),
    _motionBlurExposure ( DEFAULT_MOTION_BLUR_EXPLOSURE ),
    _newMotionBlurExposure ( DEFAULT_MOTION_BLUR_EXPLOSURE ),
    _isMotionBlurSettingsChanged ( GX_FALSE ),
    _newSSAOCheckRadius ( DEFAULT_SSAO_MAX_CHECK_RADIUS ),
    _newSSAOSamples ( static_cast<GXUByte> ( DEFAULT_SSAO_SAMPLES ) ),
    _newSSAONoiseTextureResolution ( static_cast<GXUShort> ( DEFAULT_SSAO_NOISE_TEXTURE_RESOLUTION ) ),
    _newSSAOMaxDistance ( DEFAULT_SSAO_MAX_DISTANCE ),
    _isSSAOSettingsChanged ( GX_FALSE ),
    _gaussHorizontalBlurMaterial ( eEMGaussHorizontalBlurKernelType::OneChannelFivePixel ),
    _gaussVerticalBlurMaterial ( eEMGaussVerticalBlurKernelType::OneChannelFivePixel ),
    _screenQuadMesh ( "Meshes/System/ScreenQuad.stm" )
{
    SetObjectMask ( nullptr );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    const GXUShort width = static_cast<GXUShort> ( renderer.GetWidth () );
    const GXUShort height = static_cast<GXUShort> ( renderer.GetHeight () );

    _velocityTileMaxMaterial.SetMaxBlurSamples ( _newMaxMotionBlurSamples );
    _velocityTileMaxMaterial.SetScreenResolution ( width, height );

    _motionBlurMaterial.SetMaxBlurSamples ( _newMaxMotionBlurSamples );
    _motionBlurMaterial.SetScreenResolution ( width, height );
    _motionBlurMaterial.SetDepthLimit ( _newMotionBlurDepthLimit );

    _ssaoSharpMaterial.SetCheckRadius ( _newSSAOCheckRadius );
    _ssaoSharpMaterial.SetSampleNumber ( _newSSAOSamples );
    _ssaoSharpMaterial.SetNoiseTextureResolution ( _newSSAONoiseTextureResolution );
    _ssaoSharpMaterial.SetMaximumDistance ( _newSSAOMaxDistance );

    CreateFBO ();

    _outCamera.SetProjection ( CVV_WIDTH, CVV_HEIGHT, Z_NEAR, Z_FAR );
    _screenQuadMesh.SetLocation ( 0.0f, 0.0f, Z_RENDER );

    _directedLightMaterial.SetAlbedoTexture ( _albedoTexture );
    _directedLightMaterial.SetNormalTexture ( _normalTexture );
    _directedLightMaterial.SetEmissionTexture ( _emissionTexture );
    _directedLightMaterial.SetParameterTexture ( _parameterTexture );
    _directedLightMaterial.SetDepthTexture ( _depthStencilTexture );

    _lightProbeMaterial.SetAlbedoTexture ( _albedoTexture );
    _lightProbeMaterial.SetNormalTexture ( _normalTexture );
    _lightProbeMaterial.SetEmissionTexture ( _emissionTexture );
    _lightProbeMaterial.SetParameterTexture ( _parameterTexture );
    _lightProbeMaterial.SetDepthTexture ( _depthStencilTexture );

    _velocityTileMaxMaterial.SetVelocityBlurTexture ( _velocityBlurTexture );

    _velocityNeighborMaxMaterial.SetVelocityTileMaxTexture ( _velocityTileMaxTexture );
    _velocityNeighborMaxMaterial.SetVelocityTileMaxTextureResolution ( _velocityTileMaxTexture.GetWidth (), _velocityTileMaxTexture.GetHeight () );

    _motionBlurMaterial.SetVelocityNeighborMaxTexture ( _velocityNeighborMaxTexture );
    _motionBlurMaterial.SetVelocityTexture ( _velocityBlurTexture );
    _motionBlurMaterial.SetDepthTexture ( _depthStencilTexture );
    _motionBlurMaterial.SetImageTexture ( _yottaTexture );

    _ssaoSharpMaterial.SetDepthTexture ( _depthStencilTexture );
    _ssaoSharpMaterial.SetNormalTexture ( _normalTexture );

    _gaussHorizontalBlurMaterial.SetImageTexture ( _ssaoOmegaTexture );
    _gaussVerticalBlurMaterial.SetImageTexture ( _ssaoYottaTexture );

    _ssaoApplyMaterial.SetSSAOTexture ( _ssaoOmegaTexture );
    _ssaoApplyMaterial.SetImageTexture ( _omegaTexture );

    SetToneMapperGamma ( DEFAULT_TONE_MAPPER_GAMMA );
    SetToneMapperEyeAdaptationSpeed ( DEFAULT_TONE_MAPPER_EYE_ADAPTATION_SPEED );
    SetToneMapperEyeSensitivity ( DEFAULT_TONE_MAPPER_EYE_SENSITIVITY );
    SetToneMapperAbsoluteWhiteIntensity ( DEFAULT_TONE_MAPPER_ABSOLUTE_WHITE_INTENSITY );

    _toneMapperMaterial.SetLinearSpaceTexture ( _omegaTexture );

    _toneMapperLuminanceTripletReducerMaterial.SetLuminanceTripletTexture ( _importantAreaTexture );

    _toneMapperEffectiveLuminanceTriplet.Init ( INVALID_LUMINANCE, INVALID_LUMINANCE, INVALID_LUMINANCE );

    _importantAreaFilterMaterial.SetImageTexture ( _omegaTexture );

    EMUIMotionBlurSettings::GetInstance ();
    EMUISSAOSettings::GetInstance ();
    EMUIToneMapperSettings::GetInstance ();
}

GXVoid EMRenderer::CreateFBO ()
{
    GXRenderer& renderer = GXRenderer::GetInstance ();
    const GXUShort width = static_cast<GXUShort> ( renderer.GetWidth () );
    const GXUShort height = static_cast<GXUShort> ( renderer.GetHeight () );

    const GXUShort importantAreaSide = height < width ? height : width;

    _albedoTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE );
    _normalTexture.InitResources ( width, height, GL_RGB16F, GX_FALSE );
    _emissionTexture.InitResources ( width, height, GL_RGB16F, GX_FALSE );
    _parameterTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE );
    _velocityBlurTexture.InitResources ( width, height, GL_RG8, GX_FALSE );
    _ssaoOmegaTexture.InitResources ( width, height, GL_R8, GX_FALSE );
    _ssaoYottaTexture.InitResources ( width, height, GL_R8, GX_FALSE );
    _objectTextures[ 0u ].InitResources ( width, height, GL_RGBA8, GX_FALSE );
    _objectTextures[ 1u ].InitResources ( width, height, GL_RGBA8, GX_FALSE );
    _importantAreaTexture.InitResources ( importantAreaSide, importantAreaSide, GL_RGB16F, GX_TRUE );
    _depthStencilTexture.InitResources ( width, height, GL_DEPTH24_STENCIL8, GX_FALSE );
    _omegaTexture.InitResources ( width, height, GL_RGB16F, GX_FALSE );
    _yottaTexture.InitResources ( width, height, GL_RGB16F, GX_FALSE );

    const GXUShort maxSamples = static_cast<const GXUShort> ( _motionBlurMaterial.GetMaxBlurSamples () );
    const GXUShort w = static_cast<const GXUShort> ( width / maxSamples );
    const GXUShort h = static_cast<const GXUShort> ( height / maxSamples );
    _velocityTileMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE );
    _velocityNeighborMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE );

    glGenFramebuffers ( 1, &_fbo );
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _albedoTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _normalTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, _emissionTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, _parameterTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, _velocityBlurTexture.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, _objectTextures[ 0u ].GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, _objectTextures[ 1u ].GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, _depthStencilTexture.GetTextureObject (), 0 );

    constexpr GLenum buffers[ 7u ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
    glDrawBuffers ( 7, buffers );

    const GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMRenderer::CreateFBO::Error - Что-то не так с FBO (ошибка 0x%08X)\n", static_cast<GXUInt> ( status ) );

    glBindFramebuffer ( GL_FRAMEBUFFER, 0u );

    glDrawBuffer ( GL_BACK );
}

GXVoid EMRenderer::UpdateMotionBlurSettings ()
{
    if ( _newMaxMotionBlurSamples != _motionBlurMaterial.GetMaxBlurSamples () )
    {
        _velocityTileMaxTexture.FreeResources ();
        _velocityNeighborMaxTexture.FreeResources ();

        GXRenderer& renderer = GXRenderer::GetInstance ();
        const GXUShort width = static_cast<const GXUShort> ( renderer.GetWidth () );
        const GXUShort height = static_cast<const GXUShort> ( renderer.GetHeight () );
        const GXUShort w = static_cast<const GXUShort> ( width / _newMaxMotionBlurSamples );
        const GXUShort h = static_cast<const GXUShort> ( height / _newMaxMotionBlurSamples );

        _velocityTileMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE );
        _velocityNeighborMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE );

        _velocityTileMaxMaterial.SetMaxBlurSamples ( _newMaxMotionBlurSamples );
        _velocityNeighborMaxMaterial.SetVelocityTileMaxTextureResolution ( _velocityTileMaxTexture.GetWidth (), _velocityTileMaxTexture.GetHeight () );
        _motionBlurMaterial.SetMaxBlurSamples ( _newMaxMotionBlurSamples );
    }

    if ( _newMotionBlurDepthLimit != _motionBlurMaterial.GetDepthLimit () )
        _motionBlurMaterial.SetDepthLimit ( _newMotionBlurDepthLimit );

    if ( _newMotionBlurExposure != _motionBlurExposure )
        _motionBlurExposure = _newMotionBlurExposure;

    _isMotionBlurSettingsChanged = GX_FALSE;
}

GXVoid EMRenderer::UpdateSSAOSettings ()
{
    if ( _newSSAOCheckRadius != _ssaoSharpMaterial.GetCheckRadius () )
        _ssaoSharpMaterial.SetCheckRadius ( _newSSAOCheckRadius );

    if ( _newSSAOSamples != _ssaoSharpMaterial.GetSampleNumber () )
        _ssaoSharpMaterial.SetSampleNumber ( _newSSAOSamples );

    if ( _newSSAONoiseTextureResolution != _ssaoSharpMaterial.GetNoiseTextureResolution () )
        _ssaoSharpMaterial.SetNoiseTextureResolution ( _newSSAONoiseTextureResolution );

    if ( _newSSAOMaxDistance != _ssaoSharpMaterial.GetMaximumDistance () )
        _ssaoSharpMaterial.SetMaximumDistance ( _newSSAOMaxDistance );

    _isSSAOSettingsChanged = GX_FALSE;
}

GXVoid EMRenderer::UpdateToneMapperSettings ()
{
    if ( _newToneMapperGamma != _toneMapperMaterial.GetGamma () )
        _toneMapperMaterial.SetGamma ( _newToneMapperGamma );

    if ( _newToneMapperEyeSensitivity != _toneMapperMaterial.GetEyeSensitivity () )
        _toneMapperMaterial.SetEyeSensitivity ( _newToneMapperEyeSensitivity );

    if ( _newToneMapperAbsoluteWhiteIntensity != _toneMapperMaterial.GetAbsoluteWhiteIntensity () )
        _toneMapperMaterial.SetAbsoluteWhiteIntensity ( _newToneMapperAbsoluteWhiteIntensity );

    _isToneMapperSettingsChanged = GX_FALSE;
}

GXVoid EMRenderer::LightUp ()
{
    /*for ( EMLightEmitter* light = EMLightEmitter::GetEmitters (); light; light = light->next )
    {
        switch ( light->GetType () )
        {
            case eEMLightEmitterType::Directed:
                LightUpByDirected ( (EMDirectedLight*)light );
            break;

            case eEMLightEmitterType::Spot:
                LightUpBySpot ( (EMSpotlight*)light );
            break;

            case eEMLightEmitterType::Bulp:
                LightUpByBulp ( (EMBulp*)light );
            break;

            default:
                // NOTHING
            break;
        }
    }*/

    for ( EMLightProbe* probe = EMLightProbe::GetProbes (); probe; probe = probe->_next )
    {
        _lightProbeMaterial.SetDiffuseIrradianceTexture ( probe->GetDiffuseIrradiance () );
        _lightProbeMaterial.SetPrefilteredEnvironmentMapTexture ( probe->GetPrefilteredEnvironmentMap () );
        _lightProbeMaterial.SetBRDFIntegrationMapTexture ( probe->GetBRDFIntegrationMap () );

        _lightProbeMaterial.Bind ( _screenQuadMesh );
        _screenQuadMesh.Render ();
        _lightProbeMaterial.Unbind ();
    }
}

GXVoid EMRenderer::LightUpByDirected ( EMDirectedLight* light )
{
    glEnable ( GL_BLEND );
    glBlendEquation ( GL_FUNC_ADD );
    glBlendFunc ( GL_ONE, GL_ONE );

    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );

    GXVec3 lightDirectionView;
    const GXMat4& rotation = light->GetRotation ();
    GXVec3 tmp;
    rotation.GetZ ( tmp );
    GXCamera::GetActiveCamera ()->GetCurrentFrameViewMatrix ().MultiplyAsNormal ( lightDirectionView, tmp );
    _directedLightMaterial.SetLightDirectionView ( lightDirectionView );

    GXUByte colorRed;
    GXUByte colorGreen;
    GXUByte colorBlue;
    light->GetBaseColor ( colorRed, colorGreen, colorBlue );
    _directedLightMaterial.SetHue ( colorRed, colorGreen, colorBlue );

    _directedLightMaterial.SetIntencity ( light->GetIntensity () );
    _directedLightMaterial.SetAmbientColor ( light->GetAmbientColor () );

    _directedLightMaterial.Bind ( _screenQuadMesh );

 _screenQuadMesh.Render ();

    _directedLightMaterial.Unbind ();
}

GXVoid EMRenderer::LightUpBySpot ( EMSpotlight* /*light*/ )
{
    // TODO
}

GXVoid EMRenderer::LightUpByBulp ( EMBulp* /*light*/ )
{
    // TODO
}

GXVoid* EMRenderer::SampleObject ()
{
    GXRenderer& renderer = GXRenderer::GetInstance ();

    if ( _mouseX < 0 || _mouseX >= renderer.GetWidth () )
        return nullptr;

    if ( _mouseY < 0 || _mouseY >= renderer.GetHeight () )
        return nullptr;

    glBindFramebuffer ( GL_READ_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _objectTextures[ 0 ].GetTextureObject (), 0 );
    glReadBuffer ( GL_COLOR_ATTACHMENT0 );

    GXUByte objectHi[ 4u ];
    glReadPixels ( _mouseX, _mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, objectHi );

    glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _objectTextures[ 1 ].GetTextureObject (), 0 );

    GXUByte objectLow[ 4u ];
    glReadPixels ( _mouseX, _mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, objectLow );

    glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0u, 0 );
    glBindFramebuffer ( GL_READ_FRAMEBUFFER, 0u );

    GXUPointer object = 0;
    object |= ( static_cast<GXUPointer> ( objectHi[ 0u ] ) ) << 56;
    object |= ( static_cast<GXUPointer> ( objectHi[ 1u ] ) ) << 48;
    object |= ( static_cast<GXUPointer> ( objectHi[ 2u ] ) ) << 40;
    object |= ( static_cast<GXUPointer> ( objectHi[ 3u ] ) ) << 32;
    object |= ( static_cast<GXUPointer> ( objectLow[ 0u ] ) ) << 24;
    object |= ( static_cast<GXUPointer> ( objectLow[ 1u ] ) ) << 16;
    object |= ( static_cast<GXUPointer> ( objectLow[ 2u ] ) ) << 8;
    object |= static_cast<GXUPointer> ( objectLow[ 3u ] );

    return reinterpret_cast<GXVoid*> ( object );
}
