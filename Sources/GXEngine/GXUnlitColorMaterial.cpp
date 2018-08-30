// version 1.2

#include <GXEngine/GXUnlitColorMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_RED			115u
#define DEFAULT_GREEN		185u
#define DEFAULT_BLUE		0u
#define DEFAULT_ALPHA		255u

#define VERTEX_SHADER		L"Shaders/System/Line_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/System/Line_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

GXUnlitColorMaterial::GXUnlitColorMaterial ():
	color ( static_cast<GXUByte> ( DEFAULT_RED ), static_cast<GXUByte> ( DEFAULT_GREEN ), static_cast<GXUByte> ( DEFAULT_BLUE ), static_cast<GXUByte> ( DEFAULT_ALPHA ) )
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
	colorLocation = shaderProgram.GetUniform ( "color" );
}

GXUnlitColorMaterial::~GXUnlitColorMaterial ()
{
	// NOTHING
}

GXVoid GXUnlitColorMaterial::Bind ( const GXTransform &transform )
{
	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* activeCamera = GXCamera::GetActiveCamera ();
	GXMat4 mod_view_proj_mat;
	mod_view_proj_mat.Multiply ( transform.GetCurrentFrameModelMatrix (), activeCamera->GetCurrentFrameViewProjectionMatrix () );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.data );
	glUniform4fv ( colorLocation, 1, color.data );
}

GXVoid GXUnlitColorMaterial::Unbind ()
{
	glUseProgram ( 0u );
}

GXVoid GXUnlitColorMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	color.From ( red, green, blue, alpha );
}

GXVoid GXUnlitColorMaterial::SetColor ( const GXColorRGB &newColor )
{
	color = newColor;
}
