//version 1.0

#include <GXEngine/GXUnlitTexture2DMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_COLOR_RED		255
#define DEFAULT_COLOR_GREEN		255
#define DEFAULT_COLOR_BLUE		255
#define DEFAULT_COLOR_ALPHA		255

#define DEFAULT_UV_SCALE_X		1.0f
#define DEFAULT_UV_SCALE_Y		1.0f
#define DEFAULT_UV_OFFSET_X		0.0f
#define DEFAULT_UV_OFFSET_Y		0.0f

#define VERTEX_SHADER			L"Shaders/System/VertexUVScaleOffset_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/System/UnlitTexture2D_fs.txt"

#define TEXTURE_SLOT			0


GXUnlitTexture2DMaterial::GXUnlitTexture2DMaterial ()
{
	static const GLchar* samplerNames[ 1 ] = { "textureSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 1;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	mod_view_proj_matLocation = shaderProgram.GetUniform ( "mod_view_proj_mat" );
	uvScaleOffsetLocation = shaderProgram.GetUniform ( "uvScaleOffset" );
	colorLocation = shaderProgram.GetUniform  ( "color" );

	texture = nullptr;

	SetTextureScale ( DEFAULT_UV_SCALE_X, DEFAULT_UV_SCALE_Y );
	SetTextureOffset ( DEFAULT_UV_OFFSET_X, DEFAULT_UV_OFFSET_Y );

	SetColor ( DEFAULT_COLOR_RED, DEFAULT_COLOR_GREEN, DEFAULT_COLOR_BLUE, DEFAULT_COLOR_ALPHA );
}

GXUnlitTexture2DMaterial::~GXUnlitTexture2DMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid GXUnlitTexture2DMaterial::Bind ( const GXTransform &transfrom ) const
{
	if ( !texture ) return;

	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, transfrom.GetCurrentModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentViewProjectionMatrix () );
	
	glUseProgram ( shaderProgram.GetProgram () );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );
	glUniform4fv ( uvScaleOffsetLocation, 1, uvScaleOffset.arr );
	glUniform4fv ( colorLocation, 1, color.arr );

	texture->Bind ( TEXTURE_SLOT );
}

GXVoid GXUnlitTexture2DMaterial::Unbind () const
{
	if ( !texture ) return;

	texture->Unbind ();
	glUseProgram ( 0 );
}

GXVoid GXUnlitTexture2DMaterial::SetTexture ( GXTexture &texture )
{
	this->texture = &texture;
}

GXVoid GXUnlitTexture2DMaterial::SetTextureScale ( GXFloat x, GXFloat y )
{
	uvScaleOffset.x = x;
	uvScaleOffset.y = y;
}

GXVoid GXUnlitTexture2DMaterial::SetTextureOffset ( GXFloat x, GXFloat y )
{
	uvScaleOffset.z = x;
	uvScaleOffset.w = y;
}

GXVoid GXUnlitTexture2DMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXColorToVec4 ( color, red, green, blue, alpha );
}

GXVoid GXUnlitTexture2DMaterial::SetColor ( const GXVec4 &color )
{
	this->color = color;
}
