// version 1.4

#include <GXEngine/GXUnlitColorMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_RED         115u
#define DEFAULT_GREEN       185u
#define DEFAULT_BLUE        0u
#define DEFAULT_ALPHA       255u

#define VERTEX_SHADER       L"Shaders/System/Line_vs.txt"
#define GEOMETRY_SHADER     nullptr
#define FRAGMENT_SHADER     L"Shaders/System/Line_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

GXUnlitColorMaterial::GXUnlitColorMaterial ():
    _color ( static_cast<GXUByte> ( DEFAULT_RED ), static_cast<GXUByte> ( DEFAULT_GREEN ), static_cast<GXUByte> ( DEFAULT_BLUE ), static_cast<GXUByte> ( DEFAULT_ALPHA ) )
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

    _mod_view_proj_matLocation = _shaderProgram.GetUniform ( "mod_view_proj_mat" );
    _colorLocation = _shaderProgram.GetUniform ( "color" );
}

GXUnlitColorMaterial::~GXUnlitColorMaterial ()
{
    // NOTHING
}

GXVoid GXUnlitColorMaterial::Bind ( const GXTransform &transform )
{
    glUseProgram ( _shaderProgram.GetProgram () );

    GXCamera* activeCamera = GXCamera::GetActiveCamera ();
    GXMat4 mod_view_proj_mat;
    mod_view_proj_mat.Multiply ( transform.GetCurrentFrameModelMatrix (), activeCamera->GetCurrentFrameViewProjectionMatrix () );
    glUniformMatrix4fv ( _mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat._data );
    glUniform4fv ( _colorLocation, 1, _color._data );
}

GXVoid GXUnlitColorMaterial::Unbind ()
{
    glUseProgram ( 0u );
}

GXVoid GXUnlitColorMaterial::SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    _color.From ( red, green, blue, alpha );
}

GXVoid GXUnlitColorMaterial::SetColor ( const GXColorRGB &newColor )
{
    _color = newColor;
}
