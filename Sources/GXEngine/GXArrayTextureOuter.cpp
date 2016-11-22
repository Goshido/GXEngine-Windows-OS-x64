//version 1.1

#include <GXEngine/GXArrayTextureOuter.h>
#include <GXCommon/GXMTRLoader.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXEngine/GXShaderstorage.h>
#include <GXEngine/GXShaderUtils.h>
#include <GXEngine/GXGlobals.h>


GXArrayTextureOuter::GXArrayTextureOuter ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
	GXLoadMTR ( L"../Materials/System/ArrayTextureOuter.mtr", matInfo );
	GXGetShaderProgram ( matInfo.shaders[ 0 ] );

	glUseProgram ( matInfo.shaders[ 0 ].uiId );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 0 ].uiId, "arrayTexture" ), 0 );
}

GXArrayTextureOuter::~GXArrayTextureOuter ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveShaderProgram ( matInfo.shaders[ 0 ] );

	matInfo.Cleanup ();
}

GXVoid	GXArrayTextureOuter::Draw ( GLuint arrayTexture )
{
	glUseProgram ( matInfo.shaders[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D_ARRAY, arrayTexture );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
}