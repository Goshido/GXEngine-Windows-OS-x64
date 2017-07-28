//version 1.0

#include <GXEngine/GXUnlitColorMaskMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_COLOR_RED		115
#define DEFAULT_COLOR_GREEN		185
#define DEFAULT_COLOR_BLUE		0
#define DEFAULT_COLOR_ALPHA		255

#define DEFAULT_MASK_SCALE_X	1.0f
#define DEFAULT_MASK_SCALE_Y	1.0f
#define DEFAULT_MASK_OFFSET_X	0.0f
#define DEFAULT_MASK_OFFSET_Y	0.0f

#define VERTEX_SHADER			L"Shaders/System/VertexUVScaleOffset_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/System/ColorMask_fs.txt"

#define MASK_SLOT				0


GXUnlitColorMaskMaterial::GXUnlitColorMaskMaterial ()
{
	static const GLchar* samplerNames[ 1 ] = { "maskSampler" };
	static const GLuint samplerLocations[ 1 ] = { MASK_SLOT };

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
	colorLocation = shaderProgram.GetUniform ( "color" );

	mask = nullptr;
	SetMaskScale ( DEFAULT_MASK_SCALE_X, DEFAULT_MASK_SCALE_Y );
	SetMaskOffset ( DEFAULT_MASK_OFFSET_X, DEFAULT_MASK_OFFSET_Y );

	SetColor ( DEFAULT_COLOR_RED, DEFAULT_COLOR_GREEN, DEFAULT_COLOR_BLUE, DEFAULT_COLOR_ALPHA );
}

GXUnlitColorMaskMaterial::~GXUnlitColorMaskMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid GXUnlitColorMaskMaterial::Bind ( const GXTransform &transform )
{
	if ( !mask ) return;

	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );

	glUseProgram ( shaderProgram.GetProgram () );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );
	glUniform4fv ( uvScaleOffsetLocation, 1, uvScaleOffset.arr );
	glUniform4fv ( colorLocation, 1, color.arr );

	mask->Bind ( MASK_SLOT );
}

GXVoid GXUnlitColorMaskMaterial::Unbind ()
{
	if ( !mask ) return;

	mask->Unbind ();
	glUseProgram ( 0 );
}

GXVoid GXUnlitColorMaskMaterial::SetMaskTexture ( GXTexture2D &texture )
{
	mask = &texture;
}

GXVoid GXUnlitColorMaskMaterial::SetMaskScale ( GXFloat x, GXFloat y )
{
	uvScaleOffset.x = x;
	uvScaleOffset.y = y;
}

GXVoid GXUnlitColorMaskMaterial::SetMaskOffset ( GXFloat x, GXFloat y )
{
	uvScaleOffset.z = x;
	uvScaleOffset.w = y;
}

GXVoid GXUnlitColorMaskMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXColorToVec4 ( color, red, green, blue, alpha );
}

GXVoid GXUnlitColorMaskMaterial::SetColor ( const GXVec4 &color )
{
	this->color = color;
}
