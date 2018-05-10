// version 1.1

#include <GXEngine/GXUnlitTexture2DMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_COLOR_RED		255u
#define DEFAULT_COLOR_GREEN		255u
#define DEFAULT_COLOR_BLUE		255u
#define DEFAULT_COLOR_ALPHA		255u

#define DEFAULT_UV_SCALE_X		1.0f
#define DEFAULT_UV_SCALE_Y		1.0f
#define DEFAULT_UV_OFFSET_X		0.0f
#define DEFAULT_UV_OFFSET_Y		0.0f

#define VERTEX_SHADER			L"Shaders/System/VertexUVScaleOffset_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/System/UnlitTexture2D_fs.txt"

#define TEXTURE_SLOT			0u

//---------------------------------------------------------------------------------------------------------------------

GXUnlitTexture2DMaterial::GXUnlitTexture2DMaterial ():
	texture ( nullptr ),
	color ( static_cast<GXUByte> ( DEFAULT_COLOR_RED ), static_cast<GXUByte> ( DEFAULT_COLOR_GREEN ), static_cast<GXUByte> ( DEFAULT_COLOR_BLUE ), static_cast<GXUByte> ( DEFAULT_COLOR_ALPHA ) ),
	uvScaleOffset ( DEFAULT_UV_SCALE_X, DEFAULT_UV_SCALE_Y, DEFAULT_UV_OFFSET_X, DEFAULT_UV_OFFSET_Y )
{
	static const GLchar* samplerNames[ 1 ] = { "textureSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 1u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	mod_view_proj_matLocation = shaderProgram.GetUniform ( "mod_view_proj_mat" );
	uvScaleOffsetLocation = shaderProgram.GetUniform ( "uvScaleOffset" );
	colorLocation = shaderProgram.GetUniform  ( "color" );
}

GXUnlitTexture2DMaterial::~GXUnlitTexture2DMaterial ()
{
	// NOTHING
}

GXVoid GXUnlitTexture2DMaterial::Bind ( const GXTransform &transfrom )
{
	if ( !texture ) return;

	GXMat4 mod_view_proj_mat;
	mod_view_proj_mat.Multiply ( transfrom.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );
	
	glUseProgram ( shaderProgram.GetProgram () );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.data );
	glUniform4fv ( uvScaleOffsetLocation, 1, uvScaleOffset.data );
	glUniform4fv ( colorLocation, 1, color.data );

	texture->Bind ( TEXTURE_SLOT );
}

GXVoid GXUnlitTexture2DMaterial::Unbind ()
{
	if ( !texture ) return;

	texture->Unbind ();
	glUseProgram ( 0u );
}

GXVoid GXUnlitTexture2DMaterial::SetTexture ( GXTexture2D &textureObject )
{
	texture = &textureObject;
}

GXVoid GXUnlitTexture2DMaterial::SetTextureScale ( GXFloat x, GXFloat y )
{
	uvScaleOffset.data[ 0 ] = x;
	uvScaleOffset.data[ 1 ] = y;
}

GXVoid GXUnlitTexture2DMaterial::SetTextureOffset ( GXFloat x, GXFloat y )
{
	uvScaleOffset.data[ 2 ] = x;
	uvScaleOffset.data[ 3 ] = y;
}

GXVoid GXUnlitTexture2DMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	color.From ( red, green, blue, alpha );
}

GXVoid GXUnlitTexture2DMaterial::SetColor ( const GXColorRGB &newColor )
{
	color = newColor;
}
