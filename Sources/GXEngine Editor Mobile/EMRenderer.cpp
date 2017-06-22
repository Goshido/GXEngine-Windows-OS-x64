#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine_Editor_Mobile/EMUIMotionBlurSettings.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXCommon/GXLogger.h>


#define EM_OBJECT_HI_INDEX						14
#define EM_OBJECT_LOW_INDEX						15

#define OUT_TEXTURE_SLOT						0

#define DIFFUSE_SLOT							0
#define NORMAL_SLOT								1
#define SPECULAR_SLOT							2
#define EMISSION_SLOT							3
#define DEPTH_SLOT								4

#define Z_NEAR									0.0f
#define Z_FAR									77.0f
#define Z_RENDER								1.0f

#define CVV_WIDTH								2.0f
#define CVV_HEIGHT								2.0f

#define DEFAULT_MAX_MOTION_BLUR_SAMPLES			15
#define DEFAULT_MOTION_BLUR_DEPTH_LIMIT			0.1f
#define DEFAULT_MOTION_BLUR_EXPLOSURE			0.03f

#define CLEAR_DIFFUSE_R							0.0f
#define CLEAR_DIFFUSE_G							0.0f
#define CLEAR_DIFFUSE_B							0.0f
#define CLEAR_DIFFUSE_A							1.0f

#define CLEAR_NORMAL_R							0.5f
#define CLEAR_NORMAL_G							0.5f
#define CLEAR_NORMAL_B							0.5f
#define CLEAR_NORMAL_A							0.0f

#define CLEAR_SPECULAR_R						0.0f
#define CLEAR_SPECULAR_G						0.0f
#define CLEAR_SPECULAR_B						0.0f
#define CLEAR_SPECULAR_A						0.5f

#define CLEAR_EMISSION_R						0.0f
#define CLEAR_EMISSION_G						0.0f
#define CLEAR_EMISSION_B						0.0f
#define CLEAR_EMISSION_A						0.0f

#define CLEAR_VELOCITY_BLUR_R					0.5f
#define CLEAR_VELOCITY_BLUR_G					0.5f
#define CLEAR_VELOCITY_BLUR_B					0.5f
#define CLEAR_VELOCITY_BLUR_A					0.5f

#define CLEAR_OBJECT_0_R						0.0f
#define CLEAR_OBJECT_0_G						0.0f
#define CLEAR_OBJECT_0_B						0.0f
#define CLEAR_OBJECT_0_A						0.0f

#define CLEAR_OBJECT_1_R						0.0f
#define CLEAR_OBJECT_1_G						0.0f
#define CLEAR_OBJECT_1_B						0.0f
#define CLEAR_OBJECT_1_A						0.0f

#define OVERLAY_TRANSPARENCY					80

EMRenderer* EMRenderer::instance = nullptr;

EMRenderer::~EMRenderer ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	delete &( EMUIMotionBlurSettings::GetInstance () );

	instance = nullptr;
}

GXVoid EMRenderer::StartCommonPass ()
{
	if ( ( mouseX != -1 || mouseY != -1 ) && OnObject )
	{
		OnObject ( SampleObject () );
		mouseX = mouseY = -1;
	}

	if ( isMotionBlurSettingsChanged )
		UpdateMotionBlurSettings ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diffuseTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, specularTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, emissionTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, velocityBlurTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	const GLenum buffers[ 7 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
	glDrawBuffers ( 7, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartCommonPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

	glDepthMask ( GL_TRUE );
	glStencilMask ( 0xFF );

	glClearDepth ( 1.0f );
	glClearStencil ( 0x00 );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );
	glDisable ( GL_BLEND );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	static const GXFloat clearDiffuseValue[ 4 ] = { CLEAR_DIFFUSE_R, CLEAR_DIFFUSE_G, CLEAR_DIFFUSE_B, CLEAR_DIFFUSE_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClearBufferfv ( GL_COLOR, 0, clearDiffuseValue );

	static const GXFloat clearNormalValue[ 4 ] = { CLEAR_NORMAL_R, CLEAR_NORMAL_G, CLEAR_NORMAL_B, CLEAR_NORMAL_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glClearBufferfv ( GL_COLOR, 1, clearNormalValue );

	static const GXFloat clearSpecularValue[ 4 ] = { CLEAR_SPECULAR_R, CLEAR_SPECULAR_G, CLEAR_SPECULAR_B, CLEAR_SPECULAR_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClearBufferfv ( GL_COLOR, 2, clearSpecularValue );

	static const GXFloat clearEmissionValue[ 4 ] = { CLEAR_EMISSION_R, CLEAR_EMISSION_G, CLEAR_EMISSION_B, CLEAR_EMISSION_A };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glClearBufferfv ( GL_COLOR, 3, clearEmissionValue );

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

GXVoid EMRenderer::StartLightPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, outTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glDepthMask ( GX_FALSE );
	glStencilMask ( 0x00 );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

	const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartLightPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	glClear ( GL_COLOR_BUFFER_BIT );

	glDisable ( GL_CULL_FACE );
	glDisable ( GL_DEPTH_TEST );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_ONE, GL_ONE );

	LightUp ();
}

GXVoid EMRenderer::StartHudColorPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, motionBlurredTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glDepthMask ( GX_TRUE );
	glStencilMask ( 0xFF );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

	const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartHudDepthDependentPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMRenderer::StartHudMaskPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GX_TRUE );
	glStencilMask ( 0xFF );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

	const GLenum buffers[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers ( 2, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartHudDepthDependentPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	glDisable ( GL_BLEND );
	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMRenderer::SetObjectMask ( GXUPointer object )
{
	objectMask[ 0 ] = (GXUByte)( ( object & 0xFF00000000000000 ) >> 56 );
	objectMask[ 1 ] = (GXUByte)( ( object & 0x00FF000000000000 ) >> 48 );
	objectMask[ 2 ] = (GXUByte)( ( object & 0x0000FF0000000000 ) >> 40 );
	objectMask[ 3 ] = (GXUByte)( ( object & 0x000000FF00000000 ) >> 32 );
	objectMask[ 4 ] = (GXUByte)( ( object & 0x00000000FF000000 ) >> 24 );
	objectMask[ 5 ] = (GXUByte)( ( object & 0x0000000000FF0000 ) >> 16 );
	objectMask[ 6 ] = (GXUByte)( ( object & 0x000000000000FF00 ) >> 8 );
	objectMask[ 7 ] = (GXUByte)( object & 0x00000000000000FF );

	glVertexAttrib4Nub ( EM_OBJECT_HI_INDEX, objectMask[ 0 ], objectMask[ 1 ], objectMask[ 2 ], objectMask[ 3 ] );
	glVertexAttrib4Nub ( EM_OBJECT_LOW_INDEX, objectMask[ 4 ], objectMask[ 5 ], objectMask[ 6 ], objectMask[ 7 ] );
}

GXVoid EMRenderer::ApplyMotionBlur ( GXFloat deltaTime )
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, velocityTileMaxTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glColorMask ( GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	glViewport ( 0, 0, (GLsizei)velocityTileMaxTexture.GetWidth (), (GLsizei)velocityTileMaxTexture.GetHeight () );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplyMotionBlur::Error - Что-то не так с FBO на первом проходе (ошибка 0x%08x)\n", status );

	velocityTileMaxMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuadMesh.Render ();
	velocityTileMaxMaterial.Unbind ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, velocityNeighborMaxTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );

	glDrawBuffers ( 1, buffers );

	glColorMask ( GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	glViewport ( 0, 0, (GLsizei)velocityNeighborMaxTexture.GetWidth (), (GLsizei)velocityNeighborMaxTexture.GetHeight () );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplyMotionBlur::Error - Что-то не так с FBO на втором проходе (ошибка 0x%08x)\n", status );

	velocityNeighborMaxMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuadMesh.Render ();
	velocityNeighborMaxMaterial.Unbind ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, motionBlurredTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	glViewport ( 0, 0, (GLsizei)motionBlurredTexture.GetWidth (), (GLsizei)motionBlurredTexture.GetHeight () );

	glDrawBuffers ( 1, buffers );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplyMotionBlur::Error - Что-то не так с FBO на третьем проходе (ошибка 0x%08x)\n", status );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	motionBlurMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuadMesh.Render ();
	motionBlurMaterial.Unbind ();
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
	glStencilMask ( 0xFF );
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );

	glDrawBuffer ( GL_BACK );

	glClear ( GL_COLOR_BUFFER_BIT );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer.GetWidth (), renderer.GetHeight () );

	unlitMaterial.SetTexture ( motionBlurredTexture );
	unlitMaterial.SetColor ( 255, 255, 255, 255 );
	unlitMaterial.Bind ( screenQuadMesh );

	screenQuadMesh.Render ();

	unlitMaterial.Unbind ();

	if ( target == eEMRenderTarget::Combine )
	{
		GXCamera::SetActiveCamera ( oldCamera );
		return;
	}

	GXTexture* texture = nullptr;

	switch ( target )
	{
		case eEMRenderTarget::Albedo:
			texture = &diffuseTexture;
		break;

		case eEMRenderTarget::Normal:
			texture = &normalTexture;
		break;

		case eEMRenderTarget::Specular:
			texture = &specularTexture;
		break;

		case eEMRenderTarget::Emission:
			texture = &emissionTexture;
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

		case eEMRenderTarget::Combine:
			texture = &motionBlurredTexture;
		break;
	}

	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	unlitMaterial.SetTexture ( *texture );
	unlitMaterial.SetColor ( 255, 255, 255, OVERLAY_TRANSPARENCY );
	unlitMaterial.Bind ( screenQuadMesh );

	screenQuadMesh.Render ();

	unlitMaterial.Unbind ();

	GXCamera::SetActiveCamera ( oldCamera );
}

GXVoid EMRenderer::SetOnObjectCallback ( PFNEMRENDERERONOBJECTPROC callback )
{
	OnObject = callback;
}

GXVoid EMRenderer::GetObject ( GXUShort x, GXUShort y )
{
	mouseX = x;
	mouseY = y;
}

GXVoid EMRenderer::SetMaximumMotionBlurSamples ( GXUByte samples )
{
	if ( maxMotionBlurSamples == samples ) return;

	newMaxMotionBlurSamples = samples;
	isMotionBlurSettingsChanged = GX_TRUE;
}

GXUByte EMRenderer::GetMaximumMotionBlurSamples () const
{
	return maxMotionBlurSamples;
}

GXVoid EMRenderer::SetMotionBlurDepthLimit ( GXFloat meters )
{
	if ( motionBlurDepthLimit == meters ) return;

	newMotionBlurDepthLimit = meters;
	isMotionBlurSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetMotionBlurDepthLimit () const
{
	return motionBlurDepthLimit;
}

GXVoid EMRenderer::SetMotionBlurExposure ( GXFloat seconds )
{
	if ( motionBlurExposure == seconds ) return;

	newMotionBlurExposure = seconds;
	isMotionBlurSettingsChanged = GX_TRUE;
}

GXFloat EMRenderer::GetMotionBlurExplosure () const
{
	return motionBlurExposure;
}

EMRenderer& EMRenderer::GetInstance ()
{
	if ( !instance )
		instance = new EMRenderer ();

	return *instance;
}

EMRenderer::EMRenderer ():
screenQuadMesh( L"3D Models/System/ScreenQuad.stm" )
{
	memset ( objectMask, 0, 8 * sizeof ( GXUByte ) );
	mouseX = mouseY = -1;
	OnObject = nullptr;

	maxMotionBlurSamples = newMaxMotionBlurSamples = DEFAULT_MAX_MOTION_BLUR_SAMPLES;
	motionBlurDepthLimit = newMotionBlurDepthLimit = DEFAULT_MOTION_BLUR_DEPTH_LIMIT;
	motionBlurExposure = newMotionBlurExposure = DEFAULT_MOTION_BLUR_EXPLOSURE;
	isMotionBlurSettingsChanged = GX_FALSE;

	CreateFBO ();

	GXRenderer& coreRenderer = GXRenderer::GetInstance ();
	outCamera.SetProjection ( CVV_WIDTH, CVV_HEIGHT, Z_NEAR, Z_FAR );
	screenQuadMesh.SetLocation ( 0.0f, 0.0f, Z_RENDER );

	directedLightMaterial.SetDiffuseTexture ( diffuseTexture );
	directedLightMaterial.SetNormalTexture ( normalTexture );
	directedLightMaterial.SetSpecularTexture ( specularTexture );
	directedLightMaterial.SetEmissionTexture ( emissionTexture );
	directedLightMaterial.SetDepthTexture ( depthStencilTexture );

	velocityTileMaxMaterial.SetVelocityBlurTexture ( velocityBlurTexture );
	velocityTileMaxMaterial.SetMaxBlurSamples ( maxMotionBlurSamples );
	velocityTileMaxMaterial.SetScreenResolution ( (GXUShort)coreRenderer.GetWidth (), (GXUShort)coreRenderer.GetHeight () );

	velocityNeighborMaxMaterial.SetVelocityTileMaxTexture ( velocityTileMaxTexture );
	velocityNeighborMaxMaterial.SetVelocityTileMaxTextureResolution ( velocityTileMaxTexture.GetWidth (), velocityTileMaxTexture.GetHeight () );

	motionBlurMaterial.SetVelocityNeighborMaxTexture ( velocityNeighborMaxTexture );
	motionBlurMaterial.SetVelocityTexture ( velocityBlurTexture );
	motionBlurMaterial.SetDepthTexture ( depthStencilTexture );
	motionBlurMaterial.SetImageTexture ( outTexture );
	motionBlurMaterial.SetMaxBlurSamples ( maxMotionBlurSamples );
	motionBlurMaterial.SetScreenResolution ( outTexture.GetWidth (), outTexture.GetHeight () );
	motionBlurMaterial.SetDepthLimit ( motionBlurExposure );

	SetObjectMask ( (GXUPointer)nullptr );

	EMUIMotionBlurSettings::GetInstance ();
}

GXVoid EMRenderer::CreateFBO ()
{
	GXRenderer& renderer = GXRenderer::GetInstance ();
	GXUShort width = (GXUShort)renderer.GetWidth ();
	GXUShort height = (GXUShort)renderer.GetHeight ();

	diffuseTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
	normalTexture.InitResources ( width, height, GL_RGB16, GX_FALSE, GL_CLAMP_TO_EDGE );
	specularTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
	emissionTexture.InitResources ( width, height, GL_RGB8, GX_FALSE, GL_CLAMP_TO_EDGE );
	velocityBlurTexture.InitResources ( width, height, GL_RG8, GX_FALSE, GL_CLAMP_TO_EDGE );
	objectTextures[ 0 ].InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
	objectTextures[ 1 ].InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
	depthStencilTexture.InitResources ( width, height, GL_DEPTH24_STENCIL8, GX_FALSE, GL_CLAMP_TO_EDGE );
	outTexture.InitResources ( width, height, GL_RGB8, GX_FALSE, GL_CLAMP_TO_EDGE );
	motionBlurredTexture.InitResources ( width, height, GL_RGB8, GX_FALSE, GL_CLAMP_TO_EDGE );

	GXUShort w = width / (GXUShort)maxMotionBlurSamples;
	GXUShort h = height / (GXUShort)maxMotionBlurSamples;
	velocityTileMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE, GL_CLAMP_TO_EDGE );
	velocityNeighborMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE, GL_CLAMP_TO_EDGE );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diffuseTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, specularTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, emissionTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, velocityBlurTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	const GLenum buffers[ 7 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
	glDrawBuffers ( 7, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::CreateFBO::Error - Что-то не так с FBO (ошибка 0x%08X)\n", (GXUInt)status );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	glDrawBuffer ( GL_BACK );
}

GXVoid EMRenderer::UpdateMotionBlurSettings ()
{
	if ( newMaxMotionBlurSamples != maxMotionBlurSamples )
	{
		velocityTileMaxTexture.FreeResources ();
		velocityNeighborMaxTexture.FreeResources ();

		GXRenderer& renderer = GXRenderer::GetInstance ();
		GXUShort width = (GXUShort)renderer.GetWidth ();
		GXUShort height = (GXUShort)renderer.GetHeight ();
		GXUShort w = width / (GXUShort)newMaxMotionBlurSamples;
		GXUShort h = height / (GXUShort)newMaxMotionBlurSamples;

		velocityTileMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE, GL_CLAMP_TO_EDGE );
		velocityNeighborMaxTexture.InitResources ( w, h, GL_RG8, GX_FALSE, GL_CLAMP_TO_EDGE );

		velocityTileMaxMaterial.SetMaxBlurSamples ( newMaxMotionBlurSamples );
		velocityNeighborMaxMaterial.SetVelocityTileMaxTextureResolution ( velocityTileMaxTexture.GetWidth (), velocityTileMaxTexture.GetHeight () );
		motionBlurMaterial.SetMaxBlurSamples ( newMaxMotionBlurSamples );

		maxMotionBlurSamples = newMaxMotionBlurSamples;
	}

	if ( newMotionBlurDepthLimit != motionBlurDepthLimit )
	{
		motionBlurMaterial.SetDepthLimit ( newMotionBlurDepthLimit );
		motionBlurDepthLimit = newMotionBlurDepthLimit;
	}

	if ( newMotionBlurExposure != motionBlurExposure )
		motionBlurExposure = newMotionBlurExposure;

	isMotionBlurSettingsChanged = GX_FALSE;
}

GXVoid EMRenderer::LightUp ()
{
	EMLightEmitter* light = em_LightEmitters;

	for ( EMLightEmitter* light = em_LightEmitters; light; light = light->next )
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
				//NOTHING
			break;
		}
	}
}

GXVoid EMRenderer::LightUpByDirected ( EMDirectedLight* light )
{
	glEnable ( GL_BLEND );
	glBlendEquation ( GL_FUNC_ADD );
	glBlendFunc ( GL_ONE, GL_ONE );

	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	GXVec3 toLightDirectionView;
	const GXMat4& rotation = light->GetRotation ();
	GXMulVec3Mat4AsNormal ( toLightDirectionView, rotation.zv, GXCamera::GetActiveCamera ()->GetCurrentFrameViewMatrix () );
	GXReverseVec3 ( toLightDirectionView );

	directedLightMaterial.SetToLightDirectionView ( toLightDirectionView );
	directedLightMaterial.SetColor ( light->GetColor () );
	directedLightMaterial.SetAmbientColor ( light->GetAmbientColor () );

	directedLightMaterial.Bind ( screenQuadMesh );

	screenQuadMesh.Render ();

	directedLightMaterial.Unbind ();
}

GXVoid EMRenderer::LightUpBySpot ( EMSpotlight* light )
{
	//TODO
}

GXVoid EMRenderer::LightUpByBulp ( EMBulp* light )
{
	//TODO
}

GXUPointer EMRenderer::SampleObject ()
{
	GXRenderer& renderer = GXRenderer::GetInstance ();
	if ( mouseX < 0 || mouseX >= renderer.GetWidth () ) return 0;
	if ( mouseY < 0 || mouseY >= renderer.GetHeight () ) return 0;

	glBindFramebuffer ( GL_READ_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectTextures[ 0 ].GetTextureObject (), 0 );
	glReadBuffer ( GL_COLOR_ATTACHMENT0 );

	GXUByte objectHi[ 4 ];
	glReadPixels ( mouseX, mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, objectHi );

	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectTextures[ 1 ].GetTextureObject (), 0 );

	GXUByte objectLow[ 4 ];
	glReadPixels ( mouseX, mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, objectLow );

	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0 );
	glBindFramebuffer ( GL_READ_FRAMEBUFFER, 0 );

	GXUPointer object = 0;
	object |= ( (GXUPointer)objectHi[ 0 ] ) << 56;
	object |= ( (GXUPointer)objectHi[ 1 ] ) << 48;
	object |= ( (GXUPointer)objectHi[ 2 ] ) << 40;
	object |= ( (GXUPointer)objectHi[ 3 ] ) << 32;
	object |= ( (GXUPointer)objectLow[ 0 ] ) << 24;
	object |= ( (GXUPointer)objectLow[ 1 ] ) << 16;
	object |= ( (GXUPointer)objectLow[ 2 ] ) << 8;
	object |= (GXUPointer)objectLow[ 3 ];

	return object;
}
