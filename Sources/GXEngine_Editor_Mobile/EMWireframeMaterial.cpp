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
	colorLocation = shaderProgram.GetUniform ( "color" );
}

EMWireframeMaterial::~EMWireframeMaterial ()
{
	// NOTHING
}

GXVoid EMWireframeMaterial::Bind ( const GXTransform &transform )
{
	glUseProgram ( shaderProgram.GetProgram () );

	GXMat4 currentFrameModelViewProjectionMatrix;
	currentFrameModelViewProjectionMatrix.Multiply ( transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );
	glUniformMatrix4fv ( currentFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, currentFrameModelViewProjectionMatrix.data );

	glUniform4fv ( colorLocation, 1, color.data );
}

GXVoid EMWireframeMaterial::Unbind ()
{
	glUseProgram ( 0u );
}

GXVoid EMWireframeMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	color.From ( red, green, blue, alpha );
}
