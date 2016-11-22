//version 1.6

#include <GXEngine/GXMotionBlurEffect.h>
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXTextureUtils.h>


GXMotionBlurEffect::GXMotionBlurEffect ()
{
	Load3DModel ();
	InitUniforms ();
	GXSetMat4Identity ( lastFrameViewProjMatrix );

	GXGLTextureStruct texture_info;
	texture_info.data = 0;
	texture_info.format = GL_RGBA;
	texture_info.internalFormat = GL_RGBA8;
	texture_info.width = gx_EngineSettings.potWidth;
	texture_info.height = gx_EngineSettings.potHeight;
	texture_info.wrap = GL_CLAMP_TO_EDGE;
	texture_info.type = GL_UNSIGNED_BYTE;
	texture_info.resampling = GX_TEXTURE_RESAMPLING_NONE;
	texture_info.anisotropy = 1;
	motionBlurTexture = GXCreateTexture ( texture_info );

	glGenFramebuffers ( 1, &motionBlurFBO );
	glBindFramebuffer ( GL_FRAMEBUFFER, motionBlurFBO );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, motionBlurTexture, 0 );

	GLenum fboStatus = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
		GXDebugBox ( L"Что-то с FBO для Motion Blur эффекта" );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
}

GXMotionBlurEffect::~GXMotionBlurEffect ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDeleteTextures ( 1, &motionBlurTexture );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &motionBlurFBO );
}

GXVoid GXMotionBlurEffect::Draw ()
{
	const GXMat4* current_view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 invViewProjMatrix;
	GXSetMat4Inverse ( invViewProjMatrix, *current_view_proj_mat );

	glBindFramebuffer ( GL_FRAMEBUFFER, motionBlurFBO );
	GLenum  buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	glViewport ( 0, 0, gx_EngineSettings.potWidth, gx_EngineSettings.potHeight );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, preoutTexture );
	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, depthTexture );

	glUniformMatrix4fv ( invViewProjMatrixLocation, 1, GL_FALSE, invViewProjMatrix.A );
	glUniformMatrix4fv ( lastFrameViewProjMatrixLocation, 1, GL_FALSE, lastFrameViewProjMatrix.A );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	lastFrameViewProjMatrix = *current_view_proj_mat;

	glUseProgram ( 0 );
	glBindVertexArray ( 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
}

GLuint GXMotionBlurEffect::GetOutTexture ()
{
	return motionBlurTexture;
}

GXVoid GXMotionBlurEffect::SetTextures ( GLuint preoutTexture, GLuint depthTexture )
{
	this->preoutTexture = preoutTexture;
	this->depthTexture = depthTexture;
}

GXVoid GXMotionBlurEffect::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "preoutTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "depthTexture" ), 1 );
	invViewProjMatrixLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "invViewProjMatrix" );
	lastFrameViewProjMatrixLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "lastFrameViewProjMatrix" );
	
	glUseProgram ( 0 );
}

GXVoid GXMotionBlurEffect::Load3DModel ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
	GXLoadMTR ( L"../Materials/System/Motion_Blur_Effect.mtr", mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
}