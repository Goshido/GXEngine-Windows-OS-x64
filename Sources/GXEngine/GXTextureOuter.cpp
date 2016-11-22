//version 1.6

#include <GXEngine/GXTextureOuter.h>
#include <GXEngine/GXGlobals.h>


GXTextureOuter::GXTextureOuter ()
{
	Load3DModel ();
	InitUniforms ();
	texture = 0xFFFFFFFF;
}

GXTextureOuter::~GXTextureOuter ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
}

GXVoid GXTextureOuter::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "Texture" ), 0 );
	glUseProgram ( 0 );
}

GXVoid GXTextureOuter::Draw ()
{
	if ( texture == 0xFFFFFFFF )
	{
		GXLogW ( L"GXTextureOuter::Draw::Error - Текстура не установлена!\n" );
		return;
	}



	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	static const GLenum buffer[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffer );
	glDisable ( GL_DEPTH_TEST );
	glViewport ( 0, 0, gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, texture );
	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glEnable ( GL_DEPTH_TEST );
}

GXVoid GXTextureOuter::SetTexture ( GLuint texture )
{
	this->texture  = texture;
}

GXVoid GXTextureOuter::Load3DModel ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
	GXLoadMTR ( L"../Materials/System/Texture_Outer.mtr", mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
}