// version 1.4

#include <GXEngine/GXUnlitTexture2DMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_COLOR_RED       255u
#define DEFAULT_COLOR_GREEN     255u
#define DEFAULT_COLOR_BLUE      255u
#define DEFAULT_COLOR_ALPHA     255u

#define DEFAULT_UV_SCALE_X      1.0f
#define DEFAULT_UV_SCALE_Y      1.0f
#define DEFAULT_UV_OFFSET_X     0.0f
#define DEFAULT_UV_OFFSET_Y     0.0f

#define VERTEX_SHADER           L"Shaders/System/VertexUVScaleOffset_vs.txt"
#define GEOMETRY_SHADER         nullptr
#define FRAGMENT_SHADER         L"Shaders/System/UnlitTexture2D_fs.txt"

#define TEXTURE_SLOT            0u

//---------------------------------------------------------------------------------------------------------------------

GXUnlitTexture2DMaterial::GXUnlitTexture2DMaterial ():
    _texture ( nullptr ),
    _sampler ( GL_REPEAT, eGXResampling::None, 1.0f ),
    _color ( static_cast<GXUByte> ( DEFAULT_COLOR_RED ), static_cast<GXUByte> ( DEFAULT_COLOR_GREEN ), static_cast<GXUByte> ( DEFAULT_COLOR_BLUE ), static_cast<GXUByte> ( DEFAULT_COLOR_ALPHA ) ),
    _uvScaleOffset ( DEFAULT_UV_SCALE_X, DEFAULT_UV_SCALE_Y, DEFAULT_UV_OFFSET_X, DEFAULT_UV_OFFSET_Y )
{
    constexpr GLchar* samplerNames[ 1u ] = { "textureSampler" };
    constexpr GLuint samplerLocations[ 1u ] = { TEXTURE_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 1u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _mod_view_proj_matLocation = _shaderProgram.GetUniform ( "mod_view_proj_mat" );
    _uvScaleOffsetLocation = _shaderProgram.GetUniform ( "uvScaleOffset" );
    _colorLocation = _shaderProgram.GetUniform  ( "color" );
}

GXUnlitTexture2DMaterial::~GXUnlitTexture2DMaterial ()
{
    // NOTHING
}

GXVoid GXUnlitTexture2DMaterial::Bind ( const GXTransform &transfrom )
{
    if ( !_texture ) return;

    GXMat4 mod_view_proj_mat;
    mod_view_proj_mat.Multiply ( transfrom.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );
    
    glUseProgram ( _shaderProgram.GetProgram () );
    glUniformMatrix4fv ( _mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat._data );
    glUniform4fv ( _uvScaleOffsetLocation, 1, _uvScaleOffset._data );
    glUniform4fv ( _colorLocation, 1, _color._data );

    _texture->Bind ( TEXTURE_SLOT );
    _sampler.Bind ( TEXTURE_SLOT );
}

GXVoid GXUnlitTexture2DMaterial::Unbind ()
{
    if ( !_texture ) return;

    _sampler.Unbind ( TEXTURE_SLOT );
    _texture->Unbind ();
    glUseProgram ( 0u );
}

GXVoid GXUnlitTexture2DMaterial::SetTexture ( GXTexture2D &textureObject )
{
    _texture = &textureObject;
}

GXVoid GXUnlitTexture2DMaterial::SetTextureScale ( GXFloat x, GXFloat y )
{
    _uvScaleOffset._data[ 0u ] = x;
    _uvScaleOffset._data[ 1u ] = y;
}

GXVoid GXUnlitTexture2DMaterial::SetTextureOffset ( GXFloat x, GXFloat y )
{
    _uvScaleOffset._data[ 2u ] = x;
    _uvScaleOffset._data[ 3u ] = y;
}

GXVoid GXUnlitTexture2DMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    _color.From ( red, green, blue, alpha );
}

GXVoid GXUnlitTexture2DMaterial::SetColor ( const GXColorRGB &newColor )
{
    _color = newColor;
}
