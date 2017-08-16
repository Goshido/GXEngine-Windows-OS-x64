#include <GXEngine_Editor_Mobile/EMWireframeMaterial.h>
#include <GXEngine/GXCamera.h>


#define VERTEX_SHADER		L"Shaders/Editor Mobile/VertexOnly_vs.txt"
#define GEOMETRY_SHADER		L"Shaders/System/TriangleToLines_gs.txt"
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/Color_fs.txt"

#define DEFAULT_COLOR_RED		115
#define DEFAULT_COLOR_GREEN		185
#define DEFAULT_COLOR_BLUE		0
#define DEFAULT_COLOR_ALPHA		255


EMWireframeMaterial::EMWireframeMaterial ()
{
	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 0;
	si.samplerNames = nullptr;
	si.samplerLocations = nullptr;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	currentFrameModelViewProjectionMatrixLocation = shaderProgram.GetUniform ( "currentFrameModelViewProjectionMatrix" );
	colorLocation = shaderProgram.GetUniform ( "color" );

	SetColor ( DEFAULT_COLOR_RED, DEFAULT_COLOR_GREEN, DEFAULT_COLOR_BLUE, DEFAULT_COLOR_ALPHA );
}

EMWireframeMaterial::~EMWireframeMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMWireframeMaterial::Bind ( const GXTransform &transform )
{
	glUseProgram ( shaderProgram.GetProgram () );

	GXMat4 currentFrameModelViewProjectionMatrix;
	GXMulMat4Mat4 ( currentFrameModelViewProjectionMatrix, transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );
	glUniformMatrix4fv ( currentFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, currentFrameModelViewProjectionMatrix.arr );

	glUniform4fv ( colorLocation, 1, color.arr );
}

GXVoid EMWireframeMaterial::Unbind ()
{
	glUseProgram ( 0 );
}

GXVoid EMWireframeMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXColorToVec4 ( color, red, green, blue, alpha );
}
