#include <GXEngine_Editor_Mobile/EMObjectMaskMaterial.h>
#include <GXEngine/GXCamera.h>


#define VERTEX_SHADER		L"Shaders/Editor Mobile/MaskVertexOnly_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/Mask_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMObjectMaskMaterial::EMObjectMaskMaterial ()
{
	GXShaderProgramInfo si;
	si._vertexShader = VERTEX_SHADER;
	si._geometryShader = GEOMETRY_SHADER;
	si._fragmentShader = FRAGMENT_SHADER;
	si._samplers = 0u;
	si._samplerNames = nullptr;
	si._samplerLocations = nullptr;
	si._transformFeedbackOutputs = 0;
	si._transformFeedbackOutputNames = nullptr;

	_shaderProgram.Init ( si );

	mod_view_proj_matLocation = _shaderProgram.GetUniform ( "mod_view_proj_mat" );
}

EMObjectMaskMaterial::~EMObjectMaskMaterial ()
{
	// NOTHING
}

GXVoid EMObjectMaskMaterial::Bind ( const GXTransform &transform )
{
	GXMat4 mod_view_proj_mat;
	mod_view_proj_mat.Multiply ( transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );

	glUseProgram ( _shaderProgram.GetProgram () );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat._data );
}

GXVoid EMObjectMaskMaterial::Unbind ()
{
	glUseProgram ( 0u );
}
