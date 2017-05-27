//version 1.0

#include <GXEngine/GXUnlitColorMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_RED			115
#define DEFAULT_GREEN		185
#define DEFAULT_BLUE		0
#define DEFAULT_ALPHA		255

#define VERTEX_SHADER		L"Shaders/System/Line_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/System/Line_fs.txt"


GXUnlitColorMaterial::GXUnlitColorMaterial ()
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

	mod_view_proj_matLocation = shaderProgram.GetUniform ( "mod_view_proj_mat" );
	colorLocation = shaderProgram.GetUniform ( "color" );

	SetColor ( DEFAULT_RED, DEFAULT_GREEN, DEFAULT_BLUE, DEFAULT_ALPHA );
}

GXUnlitColorMaterial::~GXUnlitColorMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid GXUnlitColorMaterial::Bind ( const GXTransform &transform ) const
{
	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* activeCamera = GXCamera::GetActiveCamera ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, transform.GetCurrentModelMatrix (), activeCamera->GetCurrentFrameViewProjectionMatrix () );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );
	glUniform4fv ( colorLocation, 1, color.arr );
}

GXVoid GXUnlitColorMaterial::Unbind () const
{
	glUseProgram ( 0 );
}

GXVoid GXUnlitColorMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXColorToVec4 ( color, red, green, blue, alpha );
}

GXVoid GXUnlitColorMaterial::SetColor ( const GXVec4 &color )
{
	this->color = color;
}
