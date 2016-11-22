#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXTextureUtils.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXShaderUtils.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXCommon/GXLogger.h>


#define EM_OBJECT_HI_INDEX		14
#define EM_OBJECT_LOW_INDEX		15

EMRenderer* em_Renderer = 0;


EMRenderer::EMRenderer ()
{
	memset ( objectMask, 0, 8 * sizeof ( GXUByte ) );
	mouseX = mouseY = -1;
	OnObject = 0;

	GXSetMat4Identity ( inv_proj_mat );

	CreateScreenQuad ();
	CreateFBO ();
	InitDirectedLightShader ();

	SetObjectMask ( 0 );
}

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

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDeleteTextures ( 1, &diffuseTexture );
	glDeleteTextures ( 1, &normalTexture );
	glDeleteTextures ( 1, &specularTexture );
	glDeleteTextures ( 1, &emissionTexture );
	glDeleteTextures ( 1, &objectTextures[ 0 ] );
	glDeleteTextures ( 1, &objectTextures[ 1 ] );
	glDeleteTextures ( 1, &depthStencilTexture );
	glDeleteTextures ( 1, &outTexture );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	GXRemoveVAO ( screenQuadVAO );

	GXRemoveShaderProgram ( screenQuadProgram );
	GXRemoveShaderProgram ( directedLightProgram );

	glDeleteSamplers ( 1, &screenSampler );
}

GXVoid EMRenderer::StartCommonPass ()
{
	if ( ( mouseX != -1 || mouseY != -1 ) && OnObject )
	{
		OnObject ( SampleObject () );
		mouseX = mouseY = -1;
	}

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diffuseTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, specularTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, emissionTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, objectTextures[ 0 ], 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 1 ], 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture, 0 );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
	glDrawBuffers ( 6, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartCommonPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n" );

	glViewport ( 0, 0, gx_Core->GetRenderer ()->GetWidth (), gx_Core->GetRenderer ()->GetHeight () );

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
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, outTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GX_FALSE );
	glStencilMask ( 0xFF );

	glViewport ( 0, 0, gx_Core->GetRenderer ()->GetWidth (), gx_Core->GetRenderer ()->GetHeight () );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartLightPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n" );

	glClear ( GL_COLOR_BUFFER_BIT );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );
	glEnable ( GL_BLEND );

	GXSetMat4Inverse ( inv_proj_mat, gx_ActiveCamera->GetProjectionMatrix () );

	LightUp ();
}

GXVoid EMRenderer::StartHudColorPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, outTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture, 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GX_TRUE );
	glStencilMask ( 0xFF );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glViewport ( 0, 0, gx_Core->GetRenderer ()->GetWidth (), gx_Core->GetRenderer ()->GetHeight () );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartHudDepthDependentPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n" );

	glEnable ( GL_BLEND );
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMRenderer::StartHudMaskPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectTextures[ 0 ], 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, objectTextures[ 1 ], 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture, 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GX_TRUE );
	glStencilMask ( 0xFF );

	glClear ( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glViewport ( 0, 0, gx_Core->GetRenderer ()->GetWidth (), gx_Core->GetRenderer ()->GetHeight () );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers ( 2, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::StartHudDepthDependentPass::Error - Что-то не так с FBO (ошибка 0x%08x)\n" );

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

GXVoid EMRenderer::PresentFrame ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );
	glDisable ( GL_BLEND );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	glDrawBuffer ( GL_BACK );

	glViewport ( 0, 0, gx_Core->GetRenderer ()->GetWidth (), gx_Core->GetRenderer ()->GetHeight () );

	glUseProgram ( screenQuadProgram.program );
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, outTexture );
	glBindSampler ( 0, screenSampler );

	glBindVertexArray ( screenQuadVAO.vao );

	glDrawArrays ( GL_TRIANGLES, 0, screenQuadVAO.numVertices );

	glBindVertexArray ( 0 );
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

GXVoid EMRenderer::CreateFBO ()
{
	GXInt width = gx_Core->GetRenderer ()->GetWidth ();
	GXInt height = gx_Core->GetRenderer ()->GetHeight ();

	GXGLTextureStruct ts;
	ts.width = width;
	ts.height = height;
	ts.anisotropy = 1;
	ts.resampling = GX_TEXTURE_RESAMPLING_NONE;
	ts.wrap = GL_CLAMP_TO_EDGE;
	ts.format = GL_RGBA;
	ts.internalFormat = GL_RGBA8;
	ts.type = GL_UNSIGNED_BYTE;
	ts.data = 0;
	diffuseTexture = GXCreateTexture ( ts );

	ts.format = GL_RGB;
	ts.internalFormat = GL_RGB16;
	ts.type = GL_UNSIGNED_SHORT;
	normalTexture = GXCreateTexture ( ts );

	ts.format = GL_RGBA;
	ts.internalFormat = GL_RGBA8;
	ts.type = GL_UNSIGNED_BYTE;
	specularTexture = GXCreateTexture ( ts );

	ts.format = GL_RGB;
	ts.internalFormat = GL_RGB8;
	ts.type = GL_UNSIGNED_BYTE;
	emissionTexture = GXCreateTexture ( ts );

	ts.format = GL_RGBA;
	ts.internalFormat = GL_RGBA8;
	ts.type = GL_UNSIGNED_BYTE;
	objectTextures[ 0 ] = GXCreateTexture ( ts );
	objectTextures[ 1 ] = GXCreateTexture ( ts );

	ts.format = GL_DEPTH_STENCIL;
	ts.internalFormat = GL_DEPTH24_STENCIL8;
	ts.type = GL_UNSIGNED_INT_24_8;
	depthStencilTexture = GXCreateTexture ( ts );

	ts.format = GL_RGB;
	ts.internalFormat = GL_RGB8;
	ts.type = GL_UNSIGNED_BYTE;
	outTexture = GXCreateTexture ( ts );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diffuseTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, specularTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, emissionTexture, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, objectTextures[ 0 ], 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, objectTextures[ 1 ], 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture, 0 );

	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
	glDrawBuffers ( 6, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMRenderer::CreateFBO::Error - Что-то не так с FBO (ошибка 0x%08X)\n", (GXUInt)status );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	glDrawBuffer ( GL_BACK );
}

GXVoid EMRenderer::CreateScreenQuad ()
{
	GXGetVAOFromNativeStaticMesh ( screenQuadVAO, L"3D Models/System/ScreenQuad.stm" );

	GXGetShaderProgram ( screenQuadProgram, L"Shaders/System/TextureOuter_vs.txt", 0, L"Shaders/System/TextureOuter_fs.txt" );

	if ( !screenQuadProgram.isSamplersTuned )
	{
		const GLuint samplerIndexes[] = { 0 };
		const GLchar* samplerNames[] = { "Texture" };
		GXTuneShaderSamplers ( screenQuadProgram, samplerIndexes, samplerNames, 1 );
	}

	GXGLSamplerStruct ss;
	ss.anisotropy = 1.0f;
	ss.resampling = GX_SAMPLER_RESAMPLING_NONE;
	ss.wrap = GL_CLAMP_TO_EDGE;
	screenSampler = GXCreateSampler ( ss );
}

GXVoid EMRenderer::InitDirectedLightShader ()
{
	GXGetShaderProgram ( directedLightProgram, L"Shaders/Editor Mobile/ScreenQuad_vs.txt", 0, L"Shaders/Editor Mobile/DirectedLight_fs.txt" );

	if ( !directedLightProgram.isSamplersTuned )
	{
		const GLuint samplerIndexes[ 5 ] = { 0, 1, 2, 3, 4 };
		const GLchar* samplerNames[ 5 ] = { "diffuseSampler", "normalSampler", "specularSampler", "emissionSampler", "depthSampler" };
		GXTuneShaderSamplers ( directedLightProgram, samplerIndexes, samplerNames, 5 );
	}

	dl_lightDirectionViewLocation = GXGetUniformLocation ( directedLightProgram.program, "toLightDirectionView" );
	dl_inv_proj_matLocation = GXGetUniformLocation ( directedLightProgram.program, "inv_proj_mat" );
	dl_colorLocation = GXGetUniformLocation ( directedLightProgram.program, "color" );
	dl_ambientColorLocation = GXGetUniformLocation ( directedLightProgram.program, "ambientColor" );
}

GXVoid EMRenderer::LightUp ()
{
	EMLightEmitter* light = em_LightEmitters;

	for ( EMLightEmitter* light = em_LightEmitters; light; light = light->next )
	{
		switch ( light->GetType () )
		{
			case EM_DIRECTED:
				LightUpByDirected ( (EMDirectedLight*)light );
			break;

			case EM_SPOT:
				LightUpBySpot ( (EMSpotlight*)light );
			break;

			case EM_BULP:
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

	glUseProgram ( directedLightProgram.program );

	GXVec3 toLightDirectionView;
	const GXMat4& rotation = light->GetRotation ();
	GXMulVec3Mat4AsNormal ( toLightDirectionView, rotation.zv, gx_ActiveCamera->GetViewMatrix () );
	toLightDirectionView.x = -toLightDirectionView.x;
	toLightDirectionView.y = -toLightDirectionView.y;
	toLightDirectionView.z = -toLightDirectionView.z;

	glUniform3fv ( dl_lightDirectionViewLocation, 1, toLightDirectionView.v );
	glUniformMatrix4fv ( dl_inv_proj_matLocation, 1, GL_FALSE, inv_proj_mat.A );

	const GXVec3& color = light->GetColor ();
	glUniform3fv ( dl_colorLocation, 1, color.v );

	const GXVec3& ambientColor = light->GetAmbientColor ();
	glUniform3fv ( dl_ambientColorLocation, 1, ambientColor.v );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, diffuseTexture );
	glBindSampler ( 0, screenSampler );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, normalTexture );
	glBindSampler ( 1, screenSampler );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, specularTexture );
	glBindSampler ( 2, screenSampler );

	glActiveTexture ( GL_TEXTURE3 );
	glBindTexture ( GL_TEXTURE_2D, emissionTexture );
	glBindSampler ( 3, screenSampler );

	glActiveTexture ( GL_TEXTURE4 );
	glBindTexture ( GL_TEXTURE_2D, depthStencilTexture );
	glBindSampler ( 4, screenSampler );

	glBindVertexArray ( screenQuadVAO.vao );
	glDrawArrays ( GL_TRIANGLES, 0, screenQuadVAO.numVertices );
	glBindVertexArray ( 0 );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( 0, 0 );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( 1, 0 );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( 2, 0 );

	glActiveTexture ( GL_TEXTURE3 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( 3, 0 );

	glActiveTexture ( GL_TEXTURE4 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( 4, 0 );

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

GXUPointer EMRenderer::SampleObject ()
{
	if ( mouseX < 0 || mouseX >= gx_Core->GetRenderer ()->GetWidth () ) return 0;
	if ( mouseY < 0 || mouseY >= gx_Core->GetRenderer ()->GetHeight () ) return 0;

	glBindFramebuffer ( GL_READ_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectTextures[ 0 ], 0 );
	glReadBuffer ( GL_COLOR_ATTACHMENT0 );

	GXUByte objectHi[ 4 ];
	glReadPixels ( mouseX, mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, objectHi );

	glFramebufferTexture ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectTextures[ 1 ], 0 );

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
