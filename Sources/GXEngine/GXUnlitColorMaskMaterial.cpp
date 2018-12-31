// version 1.3

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
    mask ( nullptr ),
    sampler ( GL_REPEAT, eGXResampling::None, 1.0f ),
    uvScaleOffset ( DEFAULT_MASK_SCALE_X, DEFAULT_MASK_SCALE_Y, DEFAULT_MASK_OFFSET_X, DEFAULT_MASK_OFFSET_Y ),
    color ( static_cast<GXUByte> ( DEFAULT_COLOR_RED ), static_cast<GXUByte> ( DEFAULT_COLOR_GREEN ), static_cast<GXUByte> ( DEFAULT_COLOR_BLUE ), static_cast<GXUByte> ( DEFAULT_COLOR_ALPHA ) )
{
    static const GLchar* samplerNames[ 1 ] = { "maskSampler" };
    static const GLuint samplerLocations[ 1 ] = { MASK_SLOT };

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
    colorLocation = shaderProgram.GetUniform ( "color" );
}

GXUnlitColorMaskMaterial::~GXUnlitColorMaskMaterial ()
{
    // NOTHING
}

GXVoid GXUnlitColorMaskMaterial::Bind ( const GXTransform &transform )
{
    if ( !mask ) return;

    GXMat4 mod_view_proj_mat;
    mod_view_proj_mat.Multiply ( transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );

    glUseProgram ( shaderProgram.GetProgram () );

    glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.data );
    glUniform4fv ( uvScaleOffsetLocation, 1, uvScaleOffset.data );
    glUniform4fv ( colorLocation, 1, color.data );

    mask->Bind ( MASK_SLOT );
    sampler.Bind ( MASK_SLOT );
}

GXVoid GXUnlitColorMaskMaterial::Unbind ()
{
    if ( !mask ) return;

    sampler.Unbind ( MASK_SLOT );
    mask->Unbind ();

    glUseProgram ( 0u );
}

GXVoid GXUnlitColorMaskMaterial::SetMaskTexture ( GXTexture2D &texture )
{
    mask = &texture;
}

GXVoid GXUnlitColorMaskMaterial::SetMaskScale ( GXFloat x, GXFloat y )
{
    uvScaleOffset.data[ 0u ] = x;
    uvScaleOffset.data[ 1u ] = y;
}

GXVoid GXUnlitColorMaskMaterial::SetMaskOffset ( GXFloat x, GXFloat y )
{
    uvScaleOffset.data[ 2u ] = x;
    uvScaleOffset.data[ 3u ] = y;
}

GXVoid GXUnlitColorMaskMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    color.From ( red, green, blue, alpha );
}

GXVoid GXUnlitColorMaskMaterial::SetColor ( const GXColorRGB &newColor )
{
    color = newColor;
}
