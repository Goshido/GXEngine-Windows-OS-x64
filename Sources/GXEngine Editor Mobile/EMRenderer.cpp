#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXCommon/GXLogger.h>


#define EM_OBJECT_HI_INDEX		14
#define EM_OBJECT_LOW_INDEX		15

#define OUT_TEXTURE_SLOT		0

#define DIFFUSE_SLOT			0
#define NORMAL_SLOT				1
#define SPECULAR_SLOT			2
#define EMISSION_SLOT			3
#define DEPTH_SLOT				4

#define Z_NEAR					0.0f
#define Z_FAR					77.0f
#define Z_RENDER				1.0f

#define CVV_WIDTH				2.0f
#define CVV_HEIGHT				2.0f


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

	if ( sourceFbo )
		glDeleteFramebuffers ( 1, &sourceFbo );

	instance = nullptr;
}

GXVoid EMRenderer::StartCommonPass ()
{
	if ( ( mouseX != -1 || mouseY != -1 ) && OnObject )
	{
		OnObject ( SampleObject () );
		mouseX = mouseY = -1;
	}

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diffuseTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, specularTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, emissionTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, velocityTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	const GLenum buffers[ 7 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
	glDrawBuffers ( 7, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartCommonPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GL_TRUE );
	glStencilMask ( 0xFF );

	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glClearDepth ( 1.0f );
	glClearStencil ( 0x00 );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );
	glDisable ( GL_BLEND );

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
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

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

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

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

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

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

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

GXVoid EMRenderer::ApplyMotionBlur ()
{
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

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

	const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::ApplyMotionBlur::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

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

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

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

		case eEMRenderTarget::Velocity:
			texture = &velocityTexture;
		break;

		case eEMRenderTarget::Depth:
			texture = &depthStencilTexture;
		break;

		case eEMRenderTarget::Combine:
			texture = &motionBlurredTexture;
		break;
	}

	unlitMaterial.SetTexture ( *texture );
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

EMRenderer* EMRenderer::GetInstance ()
{
	if ( !instance )
		instance = new EMRenderer ();

	return instance;
}

EMRenderer::EMRenderer ():
screenQuadMesh( L"3D Models/System/ScreenQuad.stm" )
{
	memset ( objectMask, 0, 8 * sizeof ( GXUByte ) );
	mouseX = mouseY = -1;
	OnObject = nullptr;

	CreateFBO ();

	GXRenderer* coreRenderer = GXRenderer::GetInstance ();
	outCamera.SetProjection ( CVV_WIDTH, CVV_HEIGHT, Z_NEAR, Z_FAR );
	screenQuadMesh.SetLocation ( 0.0f, 0.0f, Z_RENDER );

	directedLightMaterial.SetDiffuseTexture ( diffuseTexture );
	directedLightMaterial.SetNormalTexture ( normalTexture );
	directedLightMaterial.SetSpecularTexture ( specularTexture );
	directedLightMaterial.SetEmissionTexture ( emissionTexture );
	directedLightMaterial.SetDepthTexture ( depthStencilTexture );

	motionBlurMaterial.SetVelocityTexture ( velocityTexture );
	motionBlurMaterial.SetDepthTexture ( depthStencilTexture );
	motionBlurMaterial.SetImageTexture ( outTexture );
	motionBlurMaterial.SetScreenResolution ( outTexture.GetWidth (), outTexture.GetHeight () );

	SetObjectMask ( (GXUPointer)nullptr );
}

GXVoid EMRenderer::CreateFBO ()
{
	GXRenderer* renderer = GXRenderer::GetInstance ();
	GXUShort width = (GXUShort)renderer->GetWidth ();
	GXUShort height = (GXUShort)renderer->GetHeight ();

	diffuseTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
	normalTexture.InitResources ( width, height, GL_RGB16, GX_FALSE, GL_CLAMP_TO_EDGE );
	specularTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
	emissionTexture.InitResources ( width, height, GL_RGB8, GX_FALSE, GL_CLAMP_TO_EDGE );
	velocityTexture.InitResources ( width, height, GL_RGBA16F, GX_FALSE, GL_CLAMP_TO_EDGE );
	objectTextures[ 0 ].InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
	objectTextures[ 1 ].InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
	depthStencilTexture.InitResources ( width, height, GL_DEPTH24_STENCIL8, GX_FALSE, GL_CLAMP_TO_EDGE );
	outTexture.InitResources ( width, height, GL_RGB8, GX_FALSE, GL_CLAMP_TO_EDGE );
	motionBlurredTexture.InitResources ( width, height, GL_RGB8, GX_FALSE, GL_CLAMP_TO_EDGE );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diffuseTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, specularTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, emissionTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, velocityTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	const GLenum buffers[ 7 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
	glDrawBuffers ( 7, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::CreateFBO::Error - Что-то не так с FBO (ошибка 0x%08X)\n", (GXUInt)status );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	sourceFbo = 0;

	glDrawBuffer ( GL_BACK );
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
	GXMulVec3Mat4AsNormal ( toLightDirectionView, rotation.zv, GXCamera::GetActiveCamera ()->GetCurrentViewMatrix () );
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
	GXRenderer* renderer = GXRenderer::GetInstance ();
	if ( mouseX < 0 || mouseX >= renderer->GetWidth () ) return 0;
	if ( mouseY < 0 || mouseY >= renderer->GetHeight () ) return 0;

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
