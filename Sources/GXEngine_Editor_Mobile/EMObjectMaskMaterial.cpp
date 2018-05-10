#include <GXEngine_Editor_Mobile/EMObjectMaskMaterial.h>
#include <GXEngine/GXCamera.h>


#define VERTEX_SHADER		L"Shaders/Editor Mobile/MaskVertexOnly_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/Mask_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMObjectMaskMaterial::EMObjectMaskMaterial ()
{
	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 0u;
	si.samplerNames = nullptr;
	si.samplerLocations = nullptr;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	mod_view_proj_matLocation = shaderProgram.GetUniform ( "mod_view_proj_mat" );
}

EMObjectMaskMaterial::~EMObjectMaskMaterial ()
{
	// NOTHING
}

GXVoid EMObjectMaskMaterial::Bind ( const GXTransform &transform )
{
	GXMat4 mod_view_proj_mat;
	mod_view_proj_mat.Multiply ( transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );

	glUseProgram ( shaderProgram.GetProgram () );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.data );
}

GXVoid EMObjectMaskMaterial::Unbind ()
{
	glUseProgram ( 0u );
}
