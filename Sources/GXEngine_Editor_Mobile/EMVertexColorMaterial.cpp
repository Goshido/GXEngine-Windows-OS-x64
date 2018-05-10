#include <GXEngine_Editor_Mobile/EMVertexColorMaterial.h>
#include <GXEngine/GXCamera.h>


#define VERTEX_SHADER		L"Shaders/Editor Mobile/VertexColor_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/VertexColor_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMVertexColorMaterial::EMVertexColorMaterial ()
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

	currentFrameModelViewProjectionMatrixLocation = shaderProgram.GetUniform ( "currentFrameModelViewProjectionMatrix" );
}

EMVertexColorMaterial::~EMVertexColorMaterial ()
{
	// NOTHING
}

GXVoid EMVertexColorMaterial::Bind ( const GXTransform &transform )
{
	glUseProgram ( shaderProgram.GetProgram () );

	GXMat4 currentFrameModelViewProjectionMatrix;
	currentFrameModelViewProjectionMatrix.Multiply ( transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );

	glUniformMatrix4fv ( currentFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, currentFrameModelViewProjectionMatrix.data );
}

GXVoid EMVertexColorMaterial::Unbind ()
{
	glUseProgram ( 0u );
}
