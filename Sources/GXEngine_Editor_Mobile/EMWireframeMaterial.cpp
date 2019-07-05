#include <GXEngine_Editor_Mobile/EMWireframeMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_COLOR_RED		115u
#define DEFAULT_COLOR_GREEN		185u
#define DEFAULT_COLOR_BLUE		0u
#define DEFAULT_COLOR_ALPHA		255u

#define VERTEX_SHADER			L"Shaders/Editor Mobile/VertexOnly_vs.txt"
#define GEOMETRY_SHADER			L"Shaders/System/TriangleToLines_gs.txt"
#define FRAGMENT_SHADER			L"Shaders/Editor Mobile/Color_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMWireframeMaterial::EMWireframeMaterial ():
	color ( static_cast<GXUByte> ( DEFAULT_COLOR_RED ), static_cast<GXUByte> ( DEFAULT_COLOR_GREEN ), static_cast<GXUByte> ( DEFAULT_COLOR_BLUE ), static_cast<GXUByte> ( DEFAULT_COLOR_ALPHA ) )
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

	currentFrameModelViewProjectionMatrixLocation = _shaderProgram.GetUniform ( "currentFrameModelViewProjectionMatrix" );
	colorLocation = _shaderProgram.GetUniform ( "color" );
}

EMWireframeMaterial::~EMWireframeMaterial ()
{
	// NOTHING
}

GXVoid EMWireframeMaterial::Bind ( const GXTransform &transform )
{
	glUseProgram ( _shaderProgram.GetProgram () );

	GXMat4 currentFrameModelViewProjectionMatrix;
	currentFrameModelViewProjectionMatrix.Multiply ( transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );
	glUniformMatrix4fv ( currentFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, currentFrameModelViewProjectionMatrix._data );

	glUniform4fv ( colorLocation, 1, color._data );
}

GXVoid EMWireframeMaterial::Unbind ()
{
	glUseProgram ( 0u );
}

GXVoid EMWireframeMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	color.From ( red, green, blue, alpha );
}
