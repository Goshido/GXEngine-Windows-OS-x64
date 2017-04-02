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
#define DEPTH_STENCIL_SLOT		4


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
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	if ( sourceFbo )
		glDeleteFramebuffers ( 1, &sourceFbo );

	GXMeshGeometry::RemoveMeshGeometry ( screenQuadMesh );
	GXShaderProgram::RemoveShaderProgram ( screenQuadProgram );
	GXShaderProgram::RemoveShaderProgram ( directedLightProgram );

	glDeleteSamplers ( 1, &screenSampler );

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
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
	glDrawBuffers ( 6, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartCommonPass::Error - ���-�� �� ��� � FBO (������ 0x%08x)\n", status );

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

	glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth ( 1.0f );
	glClearStencil ( 0x00 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GL_TRUE );
	glStencilMask ( 0xFF );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );
	glDisable ( GL_BLEND );

	glClear ( GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
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
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GX_FALSE );
	glStencilMask ( 0xFF );

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartLightPass::Error - ���-�� �� ��� � FBO (������ 0x%08x)\n", status );

	glClear ( GL_COLOR_BUFFER_BIT );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );
	glEnable ( GL_BLEND );

	GXSetMat4Inverse ( inv_proj_mat, GXCamera::GetActiveCamera ()->GetProjectionMatrix () );

	LightUp ();
}

GXVoid EMRenderer::StartHudColorPass ()
{
	if ( combineHudTarget.GetTextureObject () != outTexture.GetTextureObject () )
		CopyTexureToCombineTexture ( combineHudTarget.GetTextureObject () );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, outTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GX_TRUE );
	glStencilMask ( 0xFF );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartHudDepthDependentPass::Error - ���-�� �� ��� � FBO (������ 0x%08x)\n", status );

	glEnable ( GL_BLEND );
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
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GX_TRUE );
	glStencilMask ( 0xFF );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers ( 2, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartHudDepthDependentPass::Error - ���-�� �� ��� � FBO (������ 0x%08x)\n", status );

	glDisable ( GL_BLEND );
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

GXVoid EMRenderer::CombineHudWithTarget ( eEMRenderTarget target )
{
	if ( target == eEMRenderTarget::Depth )
		GXLogW ( L"EMRenderer::CombineHudWithTarget::Warning - Can't combine HUD with depth target" );

	switch ( target )
	{
		case eEMRenderTarget::Albedo:
			combineHudTarget = diffuseTexture;
		break;

		case eEMRenderTarget::Normal:
			combineHudTarget = normalTexture;
		break;

		case eEMRenderTarget::Specular:
			combineHudTarget = specularTexture;
		break;

		case eEMRenderTarget::Emission:
			combineHudTarget = emissionTexture;
		break;

		case eEMRenderTarget::Depth:
			GXLogW ( L"EMRenderer::CombineHudWithTarget::Warnin - Can't combine HUD with depth target" );
		break;

		case eEMRenderTarget::Combine:
			combineHudTarget = outTexture;
		break;
	}
}

GXVoid EMRenderer::PresentFrame ( eEMRenderTarget target )
{
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );
	glDisable ( GL_BLEND );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	glDrawBuffer ( GL_BACK );

	GXRenderer* renderer = GXRenderer::GetInstance ();
	glViewport ( 0, 0, renderer->GetWidth (), renderer->GetHeight () );

	glUseProgram ( screenQuadProgram.GetProgram () );

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
			texture = &normalTexture;
		break;

		case eEMRenderTarget::Emission:
			texture = &emissionTexture;
		break;

		case eEMRenderTarget::Depth:
			texture = &depthStencilTexture;
		break;

		case eEMRenderTarget::Combine:
			texture = &outTexture;
		break;
	}

	glBindSampler ( OUT_TEXTURE_SLOT, screenSampler );
	texture->Bind ( OUT_TEXTURE_SLOT );
	screenQuadMesh.Render ();

	texture->Unbind ();
	glBindSampler ( 0, 0 );
	glUseProgram ( 0 );
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

EMRenderer::EMRenderer ()
{
	memset ( objectMask, 0, 8 * sizeof ( GXUByte ) );
	mouseX = mouseY = -1;
	OnObject = nullptr;

	GXSetMat4Identity ( inv_proj_mat );

	CreateScreenQuad ();
	CreateFBO ();
	InitDirectedLightShader ();

	SetObjectMask ( 0 );
	CombineHudWithTarget ( eEMRenderTarget::Combine );
}

GXVoid EMRenderer::CreateFBO ()
{
	GXRenderer* renderer = GXRenderer::GetInstance ();
	GXUShort width = (GXUShort)renderer->GetWidth ();
	GXUShort height = (GXUShort)renderer->GetHeight ();

	diffuseTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE );
	normalTexture.InitResources ( width, height, GL_RGB16, GX_FALSE );
	specularTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE );
	emissionTexture.InitResources ( width, height, GL_RGB8, GX_FALSE );
	objectTextures[ 0 ].InitResources ( width, height, GL_RGBA8, GX_FALSE );
	objectTextures[ 1 ].InitResources ( width, height, GL_RGBA8, GX_FALSE );
	depthStencilTexture.InitResources ( width, height, GL_DEPTH24_STENCIL8, GX_FALSE );
	outTexture.InitResources ( width, height, GL_RGB8, GX_FALSE );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diffuseTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, specularTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, emissionTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, objectTextures[ 0 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 1 ].GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture.GetTextureObject (), 0 );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
	glDrawBuffers ( 6, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::CreateFBO::Error - ���-�� �� ��� � FBO (������ 0x%08X)\n", (GXUInt)status );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	sourceFbo = 0;

	glDrawBuffer ( GL_BACK );
}

GXVoid EMRenderer::CreateScreenQuad ()
{
	screenQuadMesh = GXMeshGeometry::LoadFromStm ( L"3D Models/System/ScreenQuad.stm" );

	const GLchar* samplerNames[ 1 ] = { "Texture" };
	const GLuint samplerLocations[ 1 ] = { 0 };

	GXShaderProgramInfo si;
	si.vs = L"Shaders/System/TextureOuter_vs.txt";
	si.gs = nullptr;
	si.fs = L"Shaders/System/TextureOuter_fs.txt";
	si.numSamplers = 1;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;

	screenQuadProgram = GXShaderProgram::GetShaderProgram ( si );

	GXGLSamplerInfo samplerInfo;
	samplerInfo.anisotropy = 1.0f;
	samplerInfo.resampling = eGXSamplerResampling::None;
	samplerInfo.wrap = GL_CLAMP_TO_EDGE;
	screenSampler = GXCreateSampler ( samplerInfo );
}

GXVoid EMRenderer::InitDirectedLightShader ()
{
	const GLchar* samplerNames[ 5 ] = { "diffuseSampler", "normalSampler", "specularSampler", "emissionSampler", "depthSampler" };
	const GLuint samplerLocations[ 5 ] = { DIFFUSE_SLOT, NORMAL_SLOT, SPECULAR_SLOT, EMISSION_SLOT, DEPTH_STENCIL_SLOT };

	GXShaderProgramInfo si;
	si.vs = L"Shaders/Editor Mobile/ScreenQuad_vs.txt";
	si.gs = nullptr;
	si.fs = L"Shaders/Editor Mobile/DirectedLight_fs.txt";
	si.numSamplers = 5;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;

	directedLightProgram = GXShaderProgram::GetShaderProgram ( si );

	dl_lightDirectionViewLocation = directedLightProgram.GetUniform ( "toLightDirectionView" );
	dl_inv_proj_matLocation = directedLightProgram.GetUniform ( "inv_proj_mat" );
	dl_colorLocation = directedLightProgram.GetUniform ( "color" );
	dl_ambientColorLocation = directedLightProgram.GetUniform ( "ambientColor" );
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

	glUseProgram ( directedLightProgram.GetProgram () );

	GXVec3 toLightDirectionView;
	const GXMat4& rotation = light->GetRotation ();
	GXMulVec3Mat4AsNormal ( toLightDirectionView, rotation.zv, GXCamera::GetActiveCamera ()->GetViewMatrix () );
	toLightDirectionView.x = -toLightDirectionView.x;
	toLightDirectionView.y = -toLightDirectionView.y;
	toLightDirectionView.z = -toLightDirectionView.z;

	glUniform3fv ( dl_lightDirectionViewLocation, 1, toLightDirectionView.arr );
	glUniformMatrix4fv ( dl_inv_proj_matLocation, 1, GL_FALSE, inv_proj_mat.arr );

	const GXVec3& color = light->GetColor ();
	glUniform3fv ( dl_colorLocation, 1, color.arr );

	const GXVec3& ambientColor = light->GetAmbientColor ();
	glUniform3fv ( dl_ambientColorLocation, 1, ambientColor.arr );

	diffuseTexture.Bind ( DIFFUSE_SLOT );
	glBindSampler ( DIFFUSE_SLOT, screenSampler );

	normalTexture.Bind ( NORMAL_SLOT );
	glBindSampler ( NORMAL_SLOT, screenSampler );

	specularTexture.Bind ( SPECULAR_SLOT );
	glBindSampler ( SPECULAR_SLOT, screenSampler );

	emissionTexture.Bind ( EMISSION_SLOT );
	glBindSampler ( EMISSION_SLOT, screenSampler );

	depthStencilTexture.Bind ( DEPTH_STENCIL_SLOT );
	glBindSampler ( DEPTH_STENCIL_SLOT, screenSampler );

	screenQuadMesh.Render ();

	diffuseTexture.Unbind ();
	glBindSampler ( DIFFUSE_SLOT, 0 );

	normalTexture.Unbind ();
	glBindSampler ( NORMAL_SLOT, 0 );

	specularTexture.Unbind ();
	glBindSampler ( SPECULAR_SLOT, 0 );

	emissionTexture.Unbind ();
	glBindSampler ( EMISSION_SLOT, 0 );

	depthStencilTexture.Unbind ();
	glBindSampler ( DEPTH_STENCIL_SLOT, 0 );

	glUseProgram ( 0 );
}

GXVoid EMRenderer::LightUpBySpot ( EMSpotlight* light )
{
	//TODO
}

GXVoid EMRenderer::LightUpByBulp ( EMBulp* light )
{
	//TODO
}

GXVoid EMRenderer::CopyTexureToCombineTexture ( GLuint texture )
{
	if ( sourceFbo == 0 )
		glGenFramebuffers ( 1, &sourceFbo );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };

	

	glBindFramebuffer ( GL_READ_FRAMEBUFFER, sourceFbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0 );
	glDrawBuffers ( 1, buffers );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, outTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );
	glDrawBuffers ( 1, buffers );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	GXInt width = GXRenderer::GetInstance ()->GetWidth ();
	GXInt heingt = GXRenderer::GetInstance ()->GetHeight ();
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
