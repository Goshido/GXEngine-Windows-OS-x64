#include <GXEngine_Editor_Mobile/EMObjectMaskMaterial.h>
#include <GXEngine/GXCamera.h>


#define VERTEX_SHADER		L"Shaders/Editor Mobile/MaskVertexOnly_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/Mask_fs.txt"


EMObjectMaskMaterial::EMObjectMaskMaterial ()
{
	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 0;
	si.samplerNames = nullptr;
	si.samplerLocations = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	mod_view_proj_matLocation = shaderProgram.GetUniform ( "mod_view_proj_mat" );
}

EMObjectMaskMaterial::~EMObjectMaskMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMObjectMaskMaterial::Bind ( const GXTransform &transform ) const
{
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, transform.GetModelMatrix (), GXCamera::GetActiveCamera ()->GetViewProjectionMatrix () );

	glUseProgram ( shaderProgram.GetProgram () );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );
}

GXVoid EMObjectMaskMaterial::Unbind () const
{
	glUseProgram ( 0 );
}
