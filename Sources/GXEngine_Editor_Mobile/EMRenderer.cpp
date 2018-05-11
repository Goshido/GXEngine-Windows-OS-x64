#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine_Editor_Mobile/EMLightProbe.h>
#include <GXEngine_Editor_Mobile/EMUIMotionBlurSettings.h>
#include <GXEngine_Editor_Mobile/EMUISSAOSettings.h>
#include <GXEngine_Editor_Mobile/EMUIToneMapperSettings.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXLogger.h>


#define EM_OBJECT_HI_INDEX								14
#define EM_OBJECT_LOW_INDEX								15

#define OUT_TEXTURE_SLOT								0u

#define DIFFUSE_SLOT									0u
#define NORMAL_SLOT										1u
#define SPECULAR_SLOT									2u
#define EMISSION_SLOT									3u
#define DEPTH_SLOT										4u

#define Z_NEAR											0.0f
#define Z_FAR											77.0f
#define Z_RENDER										1.0f

#define CVV_WIDTH										2.0f
#define CVV_HEIGHT										2.0f

#define DEFAULT_MAX_MOTION_BLUR_SAMPLES					15u
#define DEFAULT_MOTION_BLUR_DEPTH_LIMIT					0.1f
#define DEFAULT_MOTION_BLUR_EXPLOSURE					0.03f

#define DEFAULT_SSAO_MAX_CHECK_RADIUS					0.3f
#define DEFAULT_SSAO_SAMPLES							32u
#define DEFAULT_SSAO_NOISE_TEXTURE_RESOLUTION			9u
#define DEFAULT_SSAO_MAX_DISTANCE						1000.0f

#define DEFAULT_TONE_MAPPER_GAMMA						2.2f
#define DEFAULT_TONE_MAPPER_EYE_ADAPTATION_SPEED		1.25f
#define DEFAULT_TONE_MAPPER_EYE_SENSITIVITY				7.2e-3f
#define DEFAULT_TONE_MAPPER_ABSOLUTE_WHITE_INTENSITY	1.0e+4f

#define CLEAR_DIFFUSE_R									0.0f
#define CLEAR_DIFFUSE_G									0.0f
#define CLEAR_DIFFUSE_B									0.0f
#define CLEAR_DIFFUSE_A									1.0f

#define CLEAR_NORMAL_R									0.5f
#define CLEAR_NORMAL_G									0.5f
#define CLEAR_NORMAL_B									0.5f
#define CLEAR_NORMAL_A									0.0f

#define CLEAR_EMISSION_R								0.0f
#define CLEAR_EMISSION_G								0.0f
#define CLEAR_EMISSION_B								0.0f
#define CLEAR_EMISSION_A								0.0f

#define CLEAR_PARAMETER_ROUGNESS						0.5f
#define CLEAR_PARAMETER_IOR								0.5f
#define CLEAR_PARAMETER_SPECULAR_INTENCITY				0.0f
#define CLEAR_PARAMETER_METALLIC						0.0f

#define CLEAR_VELOCITY_BLUR_R							0.5f
#define CLEAR_VELOCITY_BLUR_G							0.5f
#define CLEAR_VELOCITY_BLUR_B							0.5f
#define CLEAR_VELOCITY_BLUR_A							0.5f

#define CLEAR_OBJECT_0_R								0.0f
#define CLEAR_OBJECT_0_G								0.0f
#define CLEAR_OBJECT_0_B								0.0f
#define CLEAR_OBJECT_0_A								0.0f

#define CLEAR_OBJECT_1_R								0.0f
#define CLEAR_OBJECT_1_G								0.0f
#define CLEAR_OBJECT_1_B								0.0f
#define CLEAR_OBJECT_1_A								0.0f

#define OVERLAY_TRANSPARENCY							180u

#define INVALID_LUMINANCE								-1.0f

//---------------------------------------------------------------------------------------------------------------------

EMRenderer* EMRenderer::instance = nullptr;

EMRenderer::~EMRenderer ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0u );
	glDeleteFramebuffers ( 1, &fbo );

	delete &( EMUIToneMapperSettings::GetInstance () );
	delete &( EMUISSAOSettings::GetInstance () );
	delete &( EMUIMotionBlurSettings::GetInstance () );

	albedoTexture.FreeResources ();
	normalTexture.FreeResources ();
	emissionTexture.FreeResources ();
	parameterTexture.FreeResources ();
	velocityBlurTexture.FreeResources ();
	velocityTileMaxTexture.FreeResources ();
	velocityNeighborMaxTexture.FreeResources ();
	ssaoOmegaTexture.FreeResources ();
	ssaoYottaTexture.FreeResources ();
	objectTextures[ 0 ].FreeResources ();
	objectTextures[ 1 ].FreeResources ();
	importantAreaTexture.FreeResources ();
	depthStencilTexture.FreeResources ();
	omegaTexture.FreeResources ();
	yottaTexture.FreeResources ();

	instance = nullptr;
}

GXVoid EMRenderer::StartCommonPass ()
{
	if ( ( mouseX != -1 || mouseY != -1 ) && OnObject )
	{
		OnObject ( handler, SampleObject () );
		mouseX = mouseY = -1;
	}

	if ( isMotionBlurSettingsChanged )
		UpdateMotionBlurSettings ();

	if ( isSSAOSettingsChanged )
		UpdateSSAOSettings ();

	if ( isToneMapperSettingsChanged )
		UpdateToneMapperSettings ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, albedoTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, emissionTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, parameterTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, velocityBlurTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	const GLenum buffers[ 7 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
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
		GXLogW ( L"EMRenderer::StartCommonPass::Error - ���-�� �� ��� � FBO (������ 0x%08x)\n", status );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	static const GXFloat clearDiffuseValue[ 4 ] = { CLEAR_DIFFUSE_R, CLEAR_DIFFUSE_G, CLEAR_DIFFUSE_B, CLEAR_DIFFUSE_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClearBufferfv ( GL_COLOR, 0, clearDiffuseValue );

	static const GXFloat clearNormalValue[ 4 ] = { CLEAR_NORMAL_R, CLEAR_NORMAL_G, CLEAR_NORMAL_B, CLEAR_NORMAL_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glClearBufferfv ( GL_COLOR, 1, clearNormalValue );

	static const GXFloat clearEmissionValue[ 4 ] = { CLEAR_EMISSION_R, CLEAR_EMISSION_G, CLEAR_EMISSION_B, CLEAR_EMISSION_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glClearBufferfv ( GL_COLOR, 2, clearEmissionValue );

	static const GXFloat clearSpecularValue[ 4 ] = { CLEAR_PARAMETER_ROUGNESS, CLEAR_PARAMETER_IOR, CLEAR_PARAMETER_SPECULAR_INTENCITY, CLEAR_PARAMETER_METALLIC };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClearBufferfv ( GL_COLOR, 3, clearSpecularValue );

	static const GXFloat clearVelocityBlurValue[ 4 ] = { CLEAR_VELOCITY_BLUR_R, CLEAR_VELOCITY_BLUR_G, CLEAR_VELOCITY_BLUR_B, CLEAR_VELOCITY_BLUR_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glClearBufferfv ( GL_COLOR, 4, clearVelocityBlurValue );

	static const GXFloat clearObject0Value[ 4 ] = { CLEAR_OBJECT_0_R, CLEAR_OBJECT_0_G, CLEAR_OBJECT_0_B, CLEAR_OBJECT_0_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClearBufferfv ( GL_COLOR, 5, clearObject0Value );

	static const GXFloat clearObject1Value[ 4 ] = { CLEAR_OBJECT_1_R, CLEAR_OBJECT_1_G, CLEAR_OBJECT_1_B, CLEAR_OBJECT_1_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClearBufferfv ( GL_COLOR, 6, clearObject1Value );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
}

GXVoid EMRenderer::StartEnvironmentPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, emissionTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, velocityBlurTexture.GetTextureObject (), 0 );
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

	const GLenum buffers[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers ( 2, buffers );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartEnvironmentPass::Error - ���-�� �� ��� � FBO (������ 0x%08x)\n", status );
}

GXVoid EMRenderer::StartLightPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, omegaTexture.GetTextureObject (), 0 );
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

	const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glDisable ( GL_CULL_FACE );
	glDisable ( GL_DEPTH_TEST );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_ONE, GL_ONE );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartLightPass::Error - ���-�� �� ��� � FBO (������ 0x%08x)\n", status );

	glClear ( GL_COLOR_BUFFER_BIT );

	LightUp ();
}

GXVoid EMRenderer::StartHudColorPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, yottaTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glDepthMask ( GX_TRUE );
	glStencilMask ( 0xFFu );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

	const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable ( GL_DEPTH_TEST );
	
	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartHudDepthDependentPass::Error - ���-�� �� ��� � FBO (������ 0x%08x)\n", status );
}

GXVoid EMRenderer::StartHudMaskPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GX_TRUE );
	glStencilMask ( 0xFFu );

	glDisable ( GL_BLEND );
	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

	const GLenum buffers[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers ( 2, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartHudDepthDependentPass::Error - ���-�� �� ��� � FBO (������ 0x%08x)\n", status );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

GXVoid EMRenderer::SetObjectMask ( GXVoid* object )
{
	GXUPointer obj = reinterpret_cast<GXUPointer> ( object );
	objectMask[ 0 ] = static_cast<GXUByte> ( ( obj & 0xFF00000000000000u ) >> 56 );
	objectMask[ 1 ] = static_cast<GXUByte> ( ( obj & 0x00FF000000000000u ) >> 48 );
	objectMask[ 2 ] = static_cast<GXUByte> ( ( obj & 0x0000FF0000000000u ) >> 40 );
	objectMask[ 3 ] = static_cast<GXUByte> ( ( obj & 0x000000FF00000000u ) >> 32 );
	objectMask[ 4 ] = static_cast<GXUByte> ( ( obj & 0x00000000FF000000u ) >> 24 );
	objectMask[ 5 ] = static_cast<GXUByte> ( ( obj & 0x0000000000FF0000u ) >> 16 );
	objectMask[ 6 ] = static_cast<GXUByte> ( ( obj & 0x000000000000FF00u ) >> 8 );
	objectMask[ 7 ] = static_cast<GXUByte> ( obj & 0x00000000000000FFu );

	glVertexAttrib4Nub ( EM_OBJECT_HI_INDEX, objectMask[ 0 ], objectMask[ 1 ], objectMask[ 2 ], objectMask[ 3 ] );
	glVertexAttrib4Nub ( EM_OBJECT_LOW_INDEX, objectMask[ 4 ], objectMask[ 5 ], objectMask[ 6 ], objectMask[ 7 ] );
}

GXVoid EMRenderer::ApplySSAO ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ssaoOmegaTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glColorMask ( GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	glViewport ( 0, 0, static_cast<GLsizei> ( ssaoOmegaTexture.GetWidth () ), static_cast<GLsizei> ( ssaoOmegaTexture.GetHeight () ) );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplySSAO::Error - ���-�� �� ��� � FBO �� ������ ������� (������ 0x%08x)\n", status );

	const GXTransform& nullTransform = GXTransform::GetNullTransform ();

	ssaoSharpMaterial.Bind ( nullTransform );
	screenQuadMesh.Render ();
	ssaoSharpMaterial.Unbind ();

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ssaoYottaTexture.GetTextureObject (), 0 );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplySSAO::Error - ���-�� �� ��� � FBO �� ������ ������� (������ 0x%08x)\n", status );

	gaussHorizontalBlurMaterial.Bind ( nullTransform );
	screenQuadMesh.Render ();
	gaussHorizontalBlurMaterial.Unbind ();

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ssaoOmegaTexture.GetTextureObject (), 0 );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplySSAO::Error - ���-�� �� ��� � FBO �� ������� ������� (������ 0x%08x)\n", status );

	gaussVerticalBlurMaterial.Bind ( nullTransform );
	screenQuadMesh.Render ();
	gaussVerticalBlurMaterial.Unbind ();

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, yottaTexture.GetTextureObject (), 0 );
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplySSAO::Error - ���-�� �� ��� � FBO �� �������� ������� (������ 0x%08x)\n", status );

	ssaoApplyMaterial.Bind ( nullTransform );
	screenQuadMesh.Render ();
	ssaoApplyMaterial.Unbind ();
}

GXVoid EMRenderer::ApplyMotionBlur ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, velocityTileMaxTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glColorMask ( GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00u );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	glViewport ( 0, 0, static_cast<GLsizei> ( velocityTileMaxTexture.GetWidth () ), static_cast<GLsizei> ( velocityTileMaxTexture.GetHeight () ) );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplyMotionBlur::Error - ���-�� �� ��� � FBO �� ������ ������� (������ 0x%08x)\n", status );

	velocityTileMaxMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuadMesh.Render ();
	velocityTileMaxMaterial.Unbind ();

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, velocityNeighborMaxTexture.GetTextureObject (), 0 );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplyMotionBlur::Error - ���-�� �� ��� � FBO �� ������ ������� (������ 0x%08x)\n", status );

	velocityNeighborMaxMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuadMesh.Render ();
	velocityNeighborMaxMaterial.Unbind ();

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, omegaTexture.GetTextureObject (), 0 );
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glViewport ( 0, 0, static_cast<GLsizei> ( omegaTexture.GetWidth () ), static_cast<GLsizei> ( omegaTexture.GetHeight () ) );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplyMotionBlur::Error - ���-�� �� ��� � FBO �� ������� ������� (������ 0x%08x)\n", status );

	motionBlurMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuadMesh.Render ();
	motionBlurMaterial.Unbind ();
}

GXVoid EMRenderer::ApplyToneMapping ( GXFloat deltaTime )
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, importantAreaTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	GLsizei width = static_cast<GLsizei> ( importantAreaTexture.GetWidth () );
	GLsizei height = static_cast<GLsizei> ( importantAreaTexture.GetHeight () );
	glViewport ( 0, 0, width, height );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplyToneMapping::Error - ���-�� �� ��� � FBO �� ������� ���������� ������ �������� (������ 0x%08x)\n", status );

	const GXTransform& nullTransform = GXTransform::GetNullTransform ();

	importantAreaFilterMaterial.Bind ( nullTransform );
	screenQuadMesh.Render ();
	importantAreaFilterMaterial.Unbind ();

	GXUByte reducingSteps = importantAreaTexture.GetLevelOfDetailNumber ();

	for ( GXUByte i = 1u; i < reducingSteps; i++ )
	{
		width /= 2u;

		if ( width == 0 )
			width = 1u;

		height /= 2u;

		if ( height == 0 )
			height = 1u;

		glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, importantAreaTexture.GetTextureObject (), static_cast<GLint> ( i ) );

		glViewport ( 0, 0, width, height );

		status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

		if ( status != GL_FRAMEBUFFER_COMPLETE )
		{
			GXLogW ( L"EMRenderer::ApplyToneMapping::Error - ���-�� �� ��� � FBO �� ������� ������������� %hhu (������ 0x%08x)\n", i, status );
			continue;
		}

		toneMapperLuminanceTripletReducerMaterial.SetLevelOfDetailToReduce ( static_cast<GXUByte> ( i - 1u ) );
		toneMapperLuminanceTripletReducerMaterial.Bind ( nullTransform );
		screenQuadMesh.Render ();
		toneMapperLuminanceTripletReducerMaterial.Unbind ();
	}

	glBindFramebuffer ( GL_READ_FRAMEBUFFER, fbo );
	glPixelStorei ( GL_PACK_ALIGNMENT, 2 );
	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, importantAreaTexture.GetTextureObject (), (GLint)( importantAreaTexture.GetLevelOfDetailNumber () - 1 ) );
	glReadBuffer ( GL_COLOR_ATTACHMENT0 );

	GXVec3 luminanceTriplet;
	glReadPixels ( 0, 0, 1, 1, GL_RGB, GL_FLOAT, luminanceTriplet.data );

	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0u, 0 );
	glReadBuffer ( GL_NONE );
	glBindFramebuffer ( GL_READ_FRAMEBUFFER, 0 );

	if ( toneMapperEffectiveLuminanceTriplet.GetX () == INVALID_LUMINANCE )
	{
		toneMapperEffectiveLuminanceTriplet = luminanceTriplet;
	}
	else
	{
		GXVec3 delta;
		delta.Substract ( luminanceTriplet, toneMapperEffectiveLuminanceTriplet );
		delta.Multiply ( delta, toneMapperEyeAdaptationSpeed * deltaTime );
		toneMapperEffectiveLuminanceTriplet.Sum ( toneMapperEffectiveLuminanceTriplet, delta );
	}

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, yottaTexture.GetTextureObject (), 0 );
	glViewport ( 0, 0, static_cast<GLsizei> ( yottaTexture.GetWidth () ), static_cast<GLsizei> ( yottaTexture.GetHeight () ) );
	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplyToneMapping::Error - ���-�� �� ��� � FBO �� ������� ����������� (������ 0x%08x)\n", status );

	toneMapperMaterial.SetLuminanceTriplet ( toneMapperEffectiveLuminanceTriplet.GetX (), toneMapperEffectiveLuminanceTriplet.GetY (), toneMapperEffectiveLuminanceTriplet.GetZ () );
	toneMapperMaterial.Bind ( nullTransform );
	screenQuadMesh.Render ();
	toneMapperMaterial.Unbind ();
}

GXVoid EMRenderer::PresentFrame ( eEMRenderTarget target )
{
	GXCamera* oldCamera = GXCamera::GetActiveCamera ();
	GXCamera::SetActiveCamera ( &outCamera );

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

	unlitMaterial.SetTexture ( yottaTexture );
	unlitMaterial.SetColor ( 255u, 255u, 255u, 255u );
	unlitMaterial.Bind ( screenQuadMesh );

	screenQuadMesh.Render ();

	unlitMaterial.Unbind ();

	if ( target == eEMRenderTarget::Combine )
	{
		GXCamera::SetActiveCamera ( oldCamera );
		return;
	}

	GXTexture2D* texture = nullptr;

	switch ( target )
	{
		case eEMRenderTarget::Albedo:
			texture = &albedoTexture;
		break;

		case eEMRenderTarget::Normal:
			texture = &normalTexture;
		break;

		case eEMRenderTarget::Emission:
			texture = &emissionTexture;
		break;

		case eEMRenderTarget::Parameter:
		texture = &parameterTexture;
		break;

		case eEMRenderTarget::VelocityBlur:
			texture = &velocityBlurTexture;
		break;

		case eEMRenderTarget::VelocityTileMax:
			texture = &velocityTileMaxTexture;
		break;

		case eEMRenderTarget::VelocityNeighborMax:
			texture = &velocityNeighborMaxTexture;
		break;

		case eEMRenderTarget::Depth:
			texture = &depthStencilTexture;
		break;

		case eEMRenderTarget::SSAO:
			texture = &ssaoOmegaTexture;
		break;

		case eEMRenderTarget::Combine:
			// NOTHING
		break;
	}

	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	unlitMaterial.SetTexture ( *texture );
	unlitMaterial.SetColor ( 255u, 255u, 255u, OVERLAY_TRANSPARENCY );
	unlitMaterial.Bind ( screenQuadMesh );

	screenQuadMesh.Render ();

	unlitMaterial.Unbind ();

	GXCamera::SetActiveCamera ( oldCamera );
}

GXVoid EMRenderer::SetOnObjectCallback ( GXVoid* handlerObject, PFNEMRENDERERONOBJECTPROC callback )
{
	OnObject = callback;
	handler = handlerObject;
}

GXVoid EMRenderer::GetObject ( GXUShort x, GXUShort y )
{
	mouseX = x;
	mouseY = y;
}

GXVoid EMRenderer::SetMaximumMotionBlurSamples ( GXUByte samples )
{
	newMaxMotionBlurSamples = samples;

	if ( motionBlurMaterial.GetMaxBlurSamples () == samples ) return;

	isMotionBlurSettingsChanged = GX_TRUE;
}

GXUByte EMRenderer::GetMaximumMotionBlurSamples () const
{
	return motionBlurMaterial.GetMaxBlurSamples ();
}

GXVoid EMRenderer::SetMotionBlurDepthLimit ( GXFloat meters )
{
	newMotionBlurDepthLimit = meters;

	if ( motionBlurMaterial.GetDepthLimit () == meters ) return;

	isMotionBlurSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetMotionBlurDepthLimit () const
{
	return motionBlurMaterial.GetDepthLimit ();
}

GXVoid EMRenderer::SetMotionBlurExposure ( GXFloat seconds )
{
	if ( motionBlurExposure == seconds ) return;

	newMotionBlurExposure = seconds;
	isMotionBlurSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetMotionBlurExposure () const
{
	return motionBlurExposure;
}

GXVoid EMRenderer::SetSSAOCheckRadius ( GXFloat meters )
{
	newSSAOCheckRadius = meters;

	if ( ssaoSharpMaterial.GetCheckRadius () == meters ) return;

	isSSAOSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetSSAOCheckRadius () const
{
	return ssaoSharpMaterial.GetCheckRadius ();
}

GXVoid EMRenderer::SetSSAOSampleNumber ( GXUByte samples )
{
	newSSAOSamples = samples;

	if ( ssaoSharpMaterial.GetSampleNumber () == samples ) return;

	isSSAOSettingsChanged = GX_TRUE;
}

GXUByte EMRenderer::GetSSAOSampleNumber () const
{
	return ssaoSharpMaterial.GetSampleNumber ();
}

GXVoid EMRenderer::SetSSAONoiseTextureResolution ( GXUShort resolution )
{
	newSSAONoiseTextureResolution = resolution;

	if ( ssaoSharpMaterial.GetNoiseTextureResolution () == resolution ) return;

	isSSAOSettingsChanged = GX_TRUE;
}

GXUShort EMRenderer::GetSSAONoiseTextureResolution () const
{
	return ssaoSharpMaterial.GetNoiseTextureResolution ();
}

GXVoid EMRenderer::SetSSAOMaximumDistance ( GXFloat meters )
{
	newSSAOMaxDistance = meters;

	if ( ssaoSharpMaterial.GetMaximumDistance () == meters ) return;

	isSSAOSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetSSAOMaximumDistance () const
{
	return ssaoSharpMaterial.GetMaximumDistance ();
}

GXVoid EMRenderer::SetToneMapperGamma ( GXFloat gamma )
{
	newToneMapperGamma = gamma;

	if ( toneMapperMaterial.GetGamma () == gamma ) return;

	isToneMapperSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetToneMapperGamma () const
{
	return toneMapperMaterial.GetGamma ();
}

GXVoid EMRenderer::SetToneMapperEyeAdaptationSpeed ( GXFloat speed )
{
	toneMapperEyeAdaptationSpeed = speed;
}

GXFloat EMRenderer::GetToneMapperEyeAdaptationSpeed () const
{
	return toneMapperEyeAdaptationSpeed;
}

GXVoid EMRenderer::SetToneMapperEyeSensitivity ( GXFloat sensitivity )
{
	newToneMapperEyeSensitivity = sensitivity;

	if ( toneMapperMaterial.GetEyeSensitivity () == sensitivity ) return;

	isToneMapperSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetToneMapperEyeSensitivity () const
{
	return toneMapperMaterial.GetEyeSensitivity ();
}

GXVoid EMRenderer::SetToneMapperAbsoluteWhiteIntensity ( GXFloat intensity )
{
	newToneMapperAbsoluteWhiteIntensity = intensity;

	if ( toneMapperMaterial.GetAbsoluteWhiteIntensity () == intensity ) return;

	isToneMapperSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetToneMapperAbsoluteWhiteIntensity () const
{
	return toneMapperMaterial.GetAbsoluteWhiteIntensity ();
}

EMRenderer& EMRenderer::GetInstance ()
{
	if ( !instance )
		instance = new EMRenderer ();

	return *instance;
}

GXTexture2D& EMRenderer::GetDepthTexture ()
{
	return depthStencilTexture;
}

EMRenderer::EMRenderer ():
	mouseX ( -1 ),
	mouseY ( -1 ),
	OnObject ( nullptr ),
	handler ( nullptr ),
	newMaxMotionBlurSamples ( static_cast<GXUByte> ( DEFAULT_MAX_MOTION_BLUR_SAMPLES ) ),
	newMotionBlurDepthLimit ( DEFAULT_MOTION_BLUR_DEPTH_LIMIT ),
	motionBlurExposure ( DEFAULT_MOTION_BLUR_EXPLOSURE ),
	newMotionBlurExposure ( DEFAULT_MOTION_BLUR_EXPLOSURE ),
	isMotionBlurSettingsChanged ( GX_FALSE ),
	newSSAOCheckRadius ( DEFAULT_SSAO_MAX_CHECK_RADIUS ),
	newSSAOSamples ( static_cast<GXUByte> ( DEFAULT_SSAO_SAMPLES ) ),
	newSSAONoiseTextureResolution ( static_cast<GXUShort> ( DEFAULT_SSAO_NOISE_TEXTURE_RESOLUTION ) ),
	newSSAOMaxDistance ( DEFAULT_SSAO_MAX_DISTANCE ),
	isSSAOSettingsChanged ( GX_FALSE ),
	screenQuadMesh ( L"Meshes/System/ScreenQuad.stm" ),
	gaussHorizontalBlurMaterial ( eEMGaussHorizontalBlurKernelType::OneChannelFivePixel ),
	gaussVerticalBlurMaterial ( eEMGaussVerticalBlurKernelType::OneChannelFivePixel )
{
	SetObjectMask ( nullptr );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	GXUShort width = static_cast<GXUShort> ( renderer.GetWidth () );
	GXUShort height = static_cast<GXUShort> ( renderer.GetHeight () );

	velocityTileMaxMaterial.SetMaxBlurSamples ( newMaxMotionBlurSamples );
	velocityTileMaxMaterial.SetScreenResolution ( width, height );

	motionBlurMaterial.SetMaxBlurSamples ( newMaxMotionBlurSamples );
	motionBlurMaterial.SetScreenResolution ( width, height );
	motionBlurMaterial.SetDepthLimit ( newMotionBlurDepthLimit );

	ssaoSharpMaterial.SetCheckRadius ( newSSAOCheckRadius );
	ssaoSharpMaterial.SetSampleNumber ( newSSAOSamples );
	ssaoSharpMaterial.SetNoiseTextureResolution ( newSSAONoiseTextureResolution );
	ssaoSharpMaterial.SetMaximumDistance ( newSSAOMaxDistance );

	CreateFBO ();

	outCamera.SetProjection ( CVV_WIDTH, CVV_HEIGHT, Z_NEAR, Z_FAR );
	screenQuadMesh.SetLocation ( 0.0f, 0.0f, Z_RENDER );

	directedLightMaterial.SetAlbedoTexture ( albedoTexture );
	directedLightMaterial.SetNormalTexture ( normalTexture );
	directedLightMaterial.SetEmissionTexture ( emissionTexture );
	directedLightMaterial.SetParameterTexture ( parameterTexture );
	directedLightMaterial.SetDepthTexture ( depthStencilTexture );

	lightProbeMaterial.SetAlbedoTexture ( albedoTexture );
	lightProbeMaterial.SetNormalTexture ( normalTexture );
	lightProbeMaterial.SetEmissionTexture ( emissionTexture );
	lightProbeMaterial.SetParameterTexture ( parameterTexture );
	lightProbeMaterial.SetDepthTexture ( depthStencilTexture );

	velocityTileMaxMaterial.SetVelocityBlurTexture ( velocityBlurTexture );

	velocityNeighborMaxMaterial.SetVelocityTileMaxTexture ( velocityTileMaxTexture );
	velocityNeighborMaxMaterial.SetVelocityTileMaxTextureResolution ( velocityTileMaxTexture.GetWidth (), velocityTileMaxTexture.GetHeight () );

	motionBlurMaterial.SetVelocityNeighborMaxTexture ( velocityNeighborMaxTexture );
	motionBlurMaterial.SetVelocityTexture ( velocityBlurTexture );
	motionBlurMaterial.SetDepthTexture ( depthStencilTexture );
	motionBlurMaterial.SetImageTexture ( yottaTexture );

	ssaoSharpMaterial.SetDepthTexture ( depthStencilTexture );
	ssaoSharpMaterial.SetNormalTexture ( normalTexture );

	gaussHorizontalBlurMaterial.SetImageTexture ( ssaoOmegaTexture );
	gaussVerticalBlurMaterial.SetImageTexture ( ssaoYottaTexture );

	ssaoApplyMaterial.SetSSAOTexture ( ssaoOmegaTexture );
	ssaoApplyMaterial.SetImageTexture ( omegaTexture );

	SetToneMapperGamma ( DEFAULT_TONE_MAPPER_GAMMA );
	SetToneMapperEyeAdaptationSpeed ( DEFAULT_TONE_MAPPER_EYE_ADAPTATION_SPEED );
	SetToneMapperEyeSensitivity ( DEFAULT_TONE_MAPPER_EYE_SENSITIVITY );
	SetToneMapperAbsoluteWhiteIntensity ( DEFAULT_TONE_MAPPER_ABSOLUTE_WHITE_INTENSITY );

	toneMapperMaterial.SetLinearSpaceTexture ( omegaTexture );

	toneMapperLuminanceTripletReducerMaterial.SetLuminanceTripletTexture ( importantAreaTexture );

	toneMapperEffectiveLuminanceTriplet.Init ( INVALID_LUMINANCE, INVALID_LUMINANCE, INVALID_LUMINANCE );

	importantAreaFilterMaterial.SetImageTexture ( omegaTexture );

	EMUIMotionBlurSettings::GetInstance ();
	EMUISSAOSettings::GetInstance ();
	EMUIToneMapperSettings::GetInstance ();
}

GXVoid EMRenderer::CreateFBO ()
{
	GXRenderer& renderer = GXRenderer::GetInstance ();
	GXUShort width = static_cast<GXUShort> ( renderer.GetWidth () );
	GXUShort height = static_cast<GXUShort> ( renderer.GetHeight () );

	GXUShort importantAreaSide = height < width ? height : width;

	albedoTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE );
	normalTexture.InitResources ( width, height, GL_RGB16F, GX_FALSE );
	emissionTexture.InitResources ( width, height, GL_RGB16F, GX_FALSE );
	parameterTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE );
	velocityBlurTexture.InitResources ( width, height, GL_RG8, GX_FALSE );
	ssaoOmegaTexture.InitResources ( width, height, GL_R8, GX_FALSE );
	ssaoYottaTexture.InitResources ( width, height, GL_R8, GX_FALSE );
	objectTextures[ 0 ].InitResources ( width, height, GL_RGBA8, GX_FALSE );
	objectTextures[ 1 ].InitResources ( width, height, GL_RGBA8, GX_FALSE );
	importantAreaTexture.InitResources ( importantAreaSide, importantAreaSide, GL_RGB16F, GX_TRUE );
	depthStencilTexture.InitResources ( width, height, GL_DEPTH24_STENCIL8, GX_FALSE );
	omegaTexture.InitResources ( width, height, GL_RGB16F, GX_FALSE );
	yottaTexture.InitResources ( width, height, GL_RGB16F, GX_FALSE );

	GXUShort maxSamples = static_cast<GXUShort> ( motionBlurMaterial.GetMaxBlurSamples () );
	GXUShort w = static_cast<GXUShort> ( width / maxSamples );
	GXUShort h = static_cast<GXUShort> ( height / maxSamples );
	velocityTileMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE );
	velocityNeighborMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, albedoTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, emissionTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, parameterTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, velocityBlurTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	const GLenum buffers[ 7 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
	glDrawBuffers ( 7, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::CreateFBO::Error - ���-�� �� ��� � FBO (������ 0x%08X)\n", (GXUInt)status );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0u );

	glDrawBuffer ( GL_BACK );
}

GXVoid EMRenderer::UpdateMotionBlurSettings ()
{
	if ( newMaxMotionBlurSamples != motionBlurMaterial.GetMaxBlurSamples () )
	{
		velocityTileMaxTexture.FreeResources ();
		velocityNeighborMaxTexture.FreeResources ();

		GXRenderer& renderer = GXRenderer::GetInstance ();
		GXUShort width = static_cast<GXUShort> ( renderer.GetWidth () );
		GXUShort height = static_cast<GXUShort> ( renderer.GetHeight () );
		GXUShort w = static_cast<GXUShort> ( width / newMaxMotionBlurSamples );
		GXUShort h = static_cast<GXUShort> ( height / newMaxMotionBlurSamples );

		velocityTileMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE );
		velocityNeighborMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE );

		velocityTileMaxMaterial.SetMaxBlurSamples ( newMaxMotionBlurSamples );
		velocityNeighborMaxMaterial.SetVelocityTileMaxTextureResolution ( velocityTileMaxTexture.GetWidth (), velocityTileMaxTexture.GetHeight () );
		motionBlurMaterial.SetMaxBlurSamples ( newMaxMotionBlurSamples );
	}

	if ( newMotionBlurDepthLimit != motionBlurMaterial.GetDepthLimit () )
		motionBlurMaterial.SetDepthLimit ( newMotionBlurDepthLimit );

	if ( newMotionBlurExposure != motionBlurExposure )
		motionBlurExposure = newMotionBlurExposure;

	isMotionBlurSettingsChanged = GX_FALSE;
}

GXVoid EMRenderer::UpdateSSAOSettings ()
{
	if ( newSSAOCheckRadius != ssaoSharpMaterial.GetCheckRadius () )
		ssaoSharpMaterial.SetCheckRadius ( newSSAOCheckRadius );

	if ( newSSAOSamples != ssaoSharpMaterial.GetSampleNumber () )
		ssaoSharpMaterial.SetSampleNumber ( newSSAOSamples );

	if ( newSSAONoiseTextureResolution != ssaoSharpMaterial.GetNoiseTextureResolution () )
		ssaoSharpMaterial.SetNoiseTextureResolution ( newSSAONoiseTextureResolution );

	if ( newSSAOMaxDistance != ssaoSharpMaterial.GetMaximumDistance () )
		ssaoSharpMaterial.SetMaximumDistance ( newSSAOMaxDistance );

	isSSAOSettingsChanged = GX_FALSE;
}

GXVoid EMRenderer::UpdateToneMapperSettings ()
{
	if ( newToneMapperGamma != toneMapperMaterial.GetGamma () )
		toneMapperMaterial.SetGamma ( newToneMapperGamma );

	if ( newToneMapperEyeSensitivity != toneMapperMaterial.GetEyeSensitivity () )
		toneMapperMaterial.SetEyeSensitivity ( newToneMapperEyeSensitivity );

	if ( newToneMapperAbsoluteWhiteIntensity != toneMapperMaterial.GetAbsoluteWhiteIntensity () )
		toneMapperMaterial.SetAbsoluteWhiteIntensity ( newToneMapperAbsoluteWhiteIntensity );

	isToneMapperSettingsChanged = GX_FALSE;
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

	for ( EMLightProbe* probe = EMLightProbe::GetProbes (); probe; probe = probe->next )
	{
		lightProbeMaterial.SetDiffuseIrradianceTexture ( probe->GetDiffuseIrradiance () );
		lightProbeMaterial.SetPrefilteredEnvironmentMapTexture ( probe->GetPrefilteredEnvironmentMap () );
		lightProbeMaterial.SetBRDFIntegrationMapTexture ( probe->GetBRDFIntegrationMap () );

		lightProbeMaterial.Bind ( screenQuadMesh );
		screenQuadMesh.Render ();
		lightProbeMaterial.Unbind ();
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
	directedLightMaterial.SetLightDirectionView ( lightDirectionView );

	GXUByte colorRed;
	GXUByte colorGreen;
	GXUByte colorBlue;
	light->GetBaseColor ( colorRed, colorGreen, colorBlue );
	directedLightMaterial.SetHue ( colorRed, colorGreen, colorBlue );

	directedLightMaterial.SetIntencity ( light->GetIntensity () );
	directedLightMaterial.SetAmbientColor ( light->GetAmbientColor () );

	directedLightMaterial.Bind ( screenQuadMesh );

	screenQuadMesh.Render ();

	directedLightMaterial.Unbind ();
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

	if ( mouseX < 0 || mouseX >= renderer.GetWidth () ) return nullptr;
	if ( mouseY < 0 || mouseY >= renderer.GetHeight () ) return nullptr;

	glBindFramebuffer ( GL_READ_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectTextures[ 0 ].GetTextureObject (), 0 );
	glReadBuffer ( GL_COLOR_ATTACHMENT0 );

	GXUByte objectHi[ 4 ];
	glReadPixels ( mouseX, mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, objectHi );

	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectTextures[ 1 ].GetTextureObject (), 0 );

	GXUByte objectLow[ 4 ];
	glReadPixels ( mouseX, mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, objectLow );

	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0u, 0 );
	glBindFramebuffer ( GL_READ_FRAMEBUFFER, 0u );

	GXUPointer object = 0;
	object |= ( static_cast<GXUPointer> ( objectHi[ 0 ] ) ) << 56;
	object |= ( static_cast<GXUPointer> ( objectHi[ 1 ] ) ) << 48;
	object |= ( static_cast<GXUPointer> ( objectHi[ 2 ] ) ) << 40;
	object |= ( static_cast<GXUPointer> ( objectHi[ 3 ] ) ) << 32;
	object |= ( static_cast<GXUPointer> ( objectLow[ 0 ] ) ) << 24;
	object |= ( static_cast<GXUPointer> ( objectLow[ 1 ] ) ) << 16;
	object |= ( static_cast<GXUPointer> ( objectLow[ 2 ] ) ) << 8;
	object |= static_cast<GXUPointer> ( objectLow[ 3 ] );

	return reinterpret_cast<GXVoid*> ( object );
}
