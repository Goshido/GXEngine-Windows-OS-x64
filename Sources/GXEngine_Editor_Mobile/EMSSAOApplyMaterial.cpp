#include <GXEngine_Editor_Mobile/EMSSAOApplyMaterial.h>


#define SSAO_SLOT           0u
#define IMAGE_SLOT          1u

#define VERTEX_SHADER       L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER     nullptr
#define FRAGMENT_SHADER     L"Shaders/Editor Mobile/SSAOApply_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMSSAOApplyMaterial::EMSSAOApplyMaterial ():
    _ssaoTexture ( nullptr ),
    _imageTexture ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f )
{
    constexpr GLchar* samplerNames[ 2u ] = { "ssaoSampler", "imageSampler" };
    constexpr GLuint samplerLocations[ 2u ] = { SSAO_SLOT, IMAGE_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 2u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );
}

EMSSAOApplyMaterial::~EMSSAOApplyMaterial ()
{
    // NOTHING
}

GXVoid EMSSAOApplyMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_ssaoTexture || !_imageTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    _ssaoTexture->Bind ( SSAO_SLOT );
    _sampler.Bind ( SSAO_SLOT );

    _imageTexture->Bind ( IMAGE_SLOT );
    _sampler.Bind ( IMAGE_SLOT );
}

GXVoid EMSSAOApplyMaterial::Unbind ()
{
    if ( !_ssaoTexture || !_imageTexture ) return;

    _sampler.Unbind ( SSAO_SLOT );
    _ssaoTexture->Unbind ();

    _sampler.Unbind ( IMAGE_SLOT );
    _imageTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMSSAOApplyMaterial::SetSSAOTexture ( GXTexture2D &texture )
{
    _ssaoTexture = &texture;
}

GXVoid EMSSAOApplyMaterial::SetImageTexture ( GXTexture2D &texture )
{
    _imageTexture = &texture;
}
