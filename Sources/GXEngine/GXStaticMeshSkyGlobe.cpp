//version 1.5

#include <GXEngine/GXStaticMeshSkyGlobe.h>
#include <GXEngine/GXGlobals.h>


GXStaticMeshSkyGlobe::GXStaticMeshSkyGlobe ()
{
	Load3DModel ();
	InitUniforms ();
}

GXStaticMeshSkyGlobe::~GXStaticMeshSkyGlobe ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveShaderProgram ( mat_info.shaders [ 0 ] );
	GXRemoveTexture ( mat_info.textures [ 0 ] );
}

GXVoid GXStaticMeshSkyGlobe::InitUniforms ()
{
	glUseProgram ( mat_info.shaders [ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders [ 0 ].uiId, "skySampler" ), 0 );
	mod_view_proj_mat_Location = GXGetUniformLocation ( mat_info.shaders [ 0 ].uiId, "mod_view_proj_mat" );

	glUseProgram ( 0 );
}

GXVoid GXStaticMeshSkyGlobe::Draw ()
{
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders [ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_proj_mat_Location, 1, GL_FALSE, mod_view_proj_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures [ 0 ].uiId );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glUseProgram ( 0 );
}

GXVoid GXStaticMeshSkyGlobe::Load3DModel ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/Test/SkyGlobe.obj", L"../3D Models/Test/Cache/SkyGlobe.cache" );
	GXLoadMTR ( L"../Materials/Test/Sky_Globe.mtr", mat_info );
	GXGetShaderProgram ( mat_info.shaders [ 0 ] );
	GXGetTexture ( mat_info.textures [ 0 ] );
}