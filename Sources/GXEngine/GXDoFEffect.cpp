//version 1.7

#include <GXEngine/GXDoFEffect.h>
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXTextureUtils.h>


GXDoFEffect::GXDoFEffect ()
{
	Load3DModel ();
	InitUniforms ();

	downsampleWidth = gx_EngineSettings.potWidth;
	downsampleHeight = gx_EngineSettings.potHeight;

	GXGLTextureStruct texture_info;
	texture_info.data = 0;
	texture_info.format = GL_RGBA;
	texture_info.internalFormat = GL_RGBA8;
	texture_info.width = gx_EngineSettings.potWidth;
	texture_info.height = gx_EngineSettings.potHeight;
	texture_info.wrap = GL_CLAMP_TO_EDGE;
	texture_info.type = GL_UNSIGNED_BYTE;
	dofTexture = GXCreateTexture ( texture_info );

	glGenFramebuffers ( 1, &dofFBO );
	glBindFramebuffer ( GL_FRAMEBUFFER, dofFBO );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dofTexture, 0 );
	GLenum fboStatus = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
		GXDebugBox ( L"Что-то с FBO уменьшенного размытого изображения для DoF эффекта" );
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	downsampleWidth >>= 2;
	downsampleHeight >>= 2;
	texture_info.width = downsampleWidth;
	texture_info.height = downsampleHeight;	
	gaussDownsampleTextureFirstPass = GXCreateTexture ( texture_info );
	gaussDownsampleTextureSecondPass = GXCreateTexture ( texture_info );

	glGenFramebuffers ( 1, &downsampleFBO );
	glBindFramebuffer ( GL_FRAMEBUFFER, downsampleFBO );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gaussDownsampleTextureFirstPass, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gaussDownsampleTextureSecondPass, 0 );
	fboStatus = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
		GXDebugBox ( L"Что-то с FBO уменьшенного размытого изображения для DoF эффекта" );
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	pixelSizeHigh [ 0 ] = 1.0f / (GXFloat)gx_EngineSettings.potWidth;
	pixelSizeHigh [ 1 ] = 1.0f / (GXFloat)gx_EngineSettings.potHeight;
	pixelSizeLow [ 0 ] = 1.0f / (GXFloat)downsampleWidth;
	pixelSizeLow [ 1 ] = 1.0f / (GXFloat)downsampleHeight;
}

GXDoFEffect::~GXDoFEffect ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	GXRemoveShaderProgram ( mat_info.shaders[ 1 ] );
	GXRemoveShaderProgram ( mat_info.shaders[ 2 ] );
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteTextures ( 1, &dofTexture );
	glDeleteFramebuffers ( 1, &dofFBO );
	glDeleteTextures ( 1, &gaussDownsampleTextureFirstPass );
	glDeleteTextures ( 1, &gaussDownsampleTextureSecondPass );
	glDeleteFramebuffers ( 1, &downsampleFBO );
}

GXVoid GXDoFEffect::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "Texture" ), 0 );
	fBlurVLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "fBlurV" );

	glUseProgram ( mat_info.shaders[ 1 ].uiId );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "Texture" ), 0 );
	fBlurHLocation = GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "fBlurH" );

	glUseProgram ( mat_info.shaders[ 2 ].uiId );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 2 ].uiId, "preoutTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 2 ].uiId, "downsampleTexture" ), 1 );
	pixelSizeHighLocation = GXGetUniformLocation ( mat_info.shaders[ 2 ].uiId, "pixelSizeHigh" );
	pixelSizeLowLocation = GXGetUniformLocation ( mat_info.shaders[ 2 ].uiId, "pixelSizeLow" );

	glUseProgram ( 0 );
}

GXVoid GXDoFEffect::Draw ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, downsampleFBO );
	GLenum  buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	glViewport ( 0, 0, downsampleWidth, downsampleHeight );
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniform1f ( fBlurVLocation, pixelSizeLow[ 1 ] );
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, preoutTexture );
	glBindVertexArray ( vaoInfo.vao );

	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	buffer = GL_COLOR_ATTACHMENT1;
	glDrawBuffers ( 1, &buffer );

	glUseProgram ( mat_info.shaders[ 1 ].uiId );

	glUniform1f ( fBlurHLocation, pixelSizeLow[ 0 ] );
	glBindTexture ( GL_TEXTURE_2D, gaussDownsampleTextureFirstPass );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glBindFramebuffer ( GL_FRAMEBUFFER, dofFBO );
	buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	glViewport ( 0, 0, gx_EngineSettings.potWidth, gx_EngineSettings.potHeight );
	glUseProgram ( mat_info.shaders [ 2 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, preoutTexture );
	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, gaussDownsampleTextureSecondPass );

	glUniform2fv ( pixelSizeHighLocation, 1, pixelSizeHigh );
	glUniform2fv ( pixelSizeLowLocation, 1, pixelSizeLow );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glUseProgram ( 0 );
	glBindVertexArray ( 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
}

GXVoid GXDoFEffect::Load3DModel ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
	
	GXLoadMTR ( L"../Materials/System/Depth_Of_Field_Effect.mtr", mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
	GXGetShaderProgram ( mat_info.shaders[ 1 ] );
	GXGetShaderProgram ( mat_info.shaders[ 2 ] );
}

GLuint GXDoFEffect::GetOutTexture ()
{
	return dofTexture;
}

GXVoid GXDoFEffect::SetPreoutTexture ( GLuint preoutTexture )
{
	this->preoutTexture = preoutTexture;
}

GXVoid GXDoFEffect::SetDoFParams ( GXFloat world_space_focus_offset, GXFloat world_space_focus_range, GXFloat maxBlurness )
{
	if ( ( world_space_focus_offset - world_space_focus_range ) < 0.0f )
		world_space_focus_offset = world_space_focus_range;
	this->world_space_focus_offset = world_space_focus_offset;
	this->world_space_focus_range = world_space_focus_range;
	
	dofParams.x = this->world_space_focus_offset;
	dofParams.y = ( this->world_space_focus_range );
	dofParams.z = maxBlurness;
}

GXVoid GXDoFEffect::GetDoFDepthParams (  GXFloat& focus, GXFloat& focusRange, GXFloat& maxBlurness )
{
	focus = dofParams.x;
	focusRange = dofParams.y;
	maxBlurness = dofParams.z;
}

GXVoid GXDoFEffect::GetDoFDepthParams ( GXVec3& dofParams )
{
	dofParams = this->dofParams;
}