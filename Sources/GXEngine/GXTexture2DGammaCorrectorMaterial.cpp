// version 1.4

#include <GXEngine/GXTexture2DGammaCorrectorMaterial.h>


#define VERTEX_SHADER       L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER     nullptr
#define FRAGMENT_SHADER     L"Shaders/System/Texture2DGammaCorrector_fs.txt"

#define TEXTURE_SLOT        0u

#define GAMMA               2.2f

//---------------------------------------------------------------------------------------------------------------------

GXTexture2DGammaCorrectorMaterial::GXTexture2DGammaCorrectorMaterial ():
    _sRGBTexture ( nullptr ),
    _sampler( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f )
{
    constexpr GLchar* samplerNames[ 1u ] = { "sRGBSampler" };
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
    _gammaLocation = _shaderProgram.GetUniform ( "gamma" );
}

GXTexture2DGammaCorrectorMaterial::~GXTexture2DGammaCorrectorMaterial ()
{
    // NOTHING
}

GXVoid GXTexture2DGammaCorrectorMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_sRGBTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    glUniform1f ( _gammaLocation, GAMMA );

    _sRGBTexture->Bind ( TEXTURE_SLOT );
    _sampler.Bind ( TEXTURE_SLOT );
}

GXVoid GXTexture2DGammaCorrectorMaterial::Unbind ()
{
    if ( !_sRGBTexture ) return;

    _sampler.Unbind ( TEXTURE_SLOT );
    _sRGBTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid GXTexture2DGammaCorrectorMaterial::SetSRGBTexture ( GXTexture2D &texture )
{
    _sRGBTexture = &texture;
}
