// version 1.4

#include <GXEngine/GXUnlitColorMaskMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_COLOR_RED           115u
#define DEFAULT_COLOR_GREEN         185u
#define DEFAULT_COLOR_BLUE          0u
#define DEFAULT_COLOR_ALPHA         255u

#define DEFAULT_MASK_SCALE_X        1.0f
#define DEFAULT_MASK_SCALE_Y        1.0f
#define DEFAULT_MASK_OFFSET_X       0.0f
#define DEFAULT_MASK_OFFSET_Y       0.0f

#define VERTEX_SHADER               L"Shaders/System/VertexUVScaleOffset_vs.txt"
#define GEOMETRY_SHADER             nullptr
#define FRAGMENT_SHADER             L"Shaders/System/ColorMask_fs.txt"

#define MASK_SLOT                   0u

//---------------------------------------------------------------------------------------------------------------------

GXUnlitColorMaskMaterial::GXUnlitColorMaskMaterial ():
    _mask ( nullptr ),
    _sampler ( GL_REPEAT, eGXResampling::None, 1.0f ),
    _uvScaleOffset ( DEFAULT_MASK_SCALE_X, DEFAULT_MASK_SCALE_Y, DEFAULT_MASK_OFFSET_X, DEFAULT_MASK_OFFSET_Y ),
    _color ( static_cast<GXUByte> ( DEFAULT_COLOR_RED ), static_cast<GXUByte> ( DEFAULT_COLOR_GREEN ), static_cast<GXUByte> ( DEFAULT_COLOR_BLUE ), static_cast<GXUByte> ( DEFAULT_COLOR_ALPHA ) )
{
    constexpr GLchar* samplerNames[ 1 ] = { "maskSampler" };
    constexpr GLuint samplerLocations[ 1 ] = { MASK_SLOT };

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
    _colorLocation = _shaderProgram.GetUniform ( "color" );
}

GXUnlitColorMaskMaterial::~GXUnlitColorMaskMaterial ()
{
    // NOTHING
}

GXVoid GXUnlitColorMaskMaterial::Bind ( const GXTransform &transform )
{
    if ( !_mask ) return;

    GXMat4 mod_view_proj_mat;
    mod_view_proj_mat.Multiply ( transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );

    glUseProgram ( _shaderProgram.GetProgram () );

    glUniformMatrix4fv ( _mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat._data );
    glUniform4fv ( _uvScaleOffsetLocation, 1, _uvScaleOffset._data );
    glUniform4fv ( _colorLocation, 1, _color._data );

    _mask->Bind ( MASK_SLOT );
    _sampler.Bind ( MASK_SLOT );
}

GXVoid GXUnlitColorMaskMaterial::Unbind ()
{
    if ( !_mask ) return;

    _sampler.Unbind ( MASK_SLOT );
    _mask->Unbind ();

    glUseProgram ( 0u );
}

GXVoid GXUnlitColorMaskMaterial::SetMaskTexture ( GXTexture2D &texture )
{
    _mask = &texture;
}

GXVoid GXUnlitColorMaskMaterial::SetMaskScale ( GXFloat x, GXFloat y )
{
    _uvScaleOffset._data[ 0u ] = x;
    _uvScaleOffset._data[ 1u ] = y;
}

GXVoid GXUnlitColorMaskMaterial::SetMaskOffset ( GXFloat x, GXFloat y )
{
    _uvScaleOffset._data[ 2u ] = x;
    _uvScaleOffset._data[ 3u ] = y;
}

GXVoid GXUnlitColorMaskMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    _color.From ( red, green, blue, alpha );
}

GXVoid GXUnlitColorMaskMaterial::SetColor ( const GXColorRGB &newColor )
{
    _color = newColor;
}
