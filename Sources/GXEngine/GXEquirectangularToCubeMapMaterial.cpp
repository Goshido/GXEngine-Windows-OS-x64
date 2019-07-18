// version 1.4

#include <GXEngine/GXEquirectangularToCubeMapMaterial.h>
#include <GXEngine/GXCameraPerspective.h>


#define VERTEX_SHADER                       L"Shaders/System/VertexPass_vs.txt"
#define GEOMETRY_SHADER                     L"Shaders/System/CubeMapSplitter_gs.txt"
#define FRAGMENT_SHADER                     L"Shaders/System/EquirectangularToCubeMap_fs.txt"

#define TEXTURE_SLOT                        0u

#define SQUARE_ASPECT_RATIO                 1.0f
#define Z_NEAR                              0.1f
#define Z_FAR                               777.777f
#define PROJECTION_FOV_Y                    GX_MATH_HALF_PI

#define ENABLE_GAMMA_CORRECTION_VALUE       2.2f
#define DISABLE_GAMMA_CORRECTION_VALUE      1.0f

//---------------------------------------------------------------------------------------------------------------------

GXEquirectangularToCubeMapMaterial::GXEquirectangularToCubeMapMaterial ():
    _equirectangularTexture ( nullptr ),
    _sampler ( GL_REPEAT, eGXResampling::Linear, 1.0f )
{
    constexpr GLchar* samplerNames[ 1u ] = { "equirectangularSampler" };
    constexpr const GLuint samplerLocations[ 1u ] = { TEXTURE_SLOT };

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

    _viewProjectionMatricesLocation = _shaderProgram.GetUniform ( "viewProjectionMatrices" );
    _gammaLocation = _shaderProgram.GetUniform ( "gamma" );

    GXCameraPerspective camera ( PROJECTION_FOV_Y, SQUARE_ASPECT_RATIO, Z_NEAR, Z_FAR );
    camera.SetRotation ( 0.0f, GX_MATH_HALF_PI, 0.0f );
    _viewProjectionMatrices[ 0u ] = camera.GetCurrentFrameViewProjectionMatrix ();

    camera.SetRotation ( 0.0f, -GX_MATH_HALF_PI, 0.0f );
    _viewProjectionMatrices[ 1u ] = camera.GetCurrentFrameViewProjectionMatrix ();

    camera.SetRotation ( GX_MATH_HALF_PI, 0.0f, 0.0f );
    _viewProjectionMatrices[ 2u ] = camera.GetCurrentFrameViewProjectionMatrix ();

    camera.SetRotation ( -GX_MATH_HALF_PI, 0.0f, 0.0f );
    _viewProjectionMatrices[ 3u ] = camera.GetCurrentFrameViewProjectionMatrix ();

    camera.SetRotation ( 0.0f, 0.0f, 0.0f );
    _viewProjectionMatrices[ 4u ] = camera.GetCurrentFrameViewProjectionMatrix ();

    camera.SetRotation ( 0.0f, GX_MATH_PI, 0.0f );
    _viewProjectionMatrices[ 5u ] = camera.GetCurrentFrameViewProjectionMatrix ();

    EnableGammaCorrection ();
}

GXEquirectangularToCubeMapMaterial::~GXEquirectangularToCubeMapMaterial ()
{
    // NOTHING
}

GXVoid GXEquirectangularToCubeMapMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_equirectangularTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );
    glUniformMatrix4fv ( _viewProjectionMatricesLocation, 6, GL_FALSE, reinterpret_cast<const GLfloat*> ( _viewProjectionMatrices ) );
    glUniform1f ( _gammaLocation, _gamma );

    _equirectangularTexture->Bind ( static_cast<GXUByte> ( TEXTURE_SLOT ) );
    _sampler.Bind ( TEXTURE_SLOT );
}

GXVoid GXEquirectangularToCubeMapMaterial::Unbind ()
{
    if ( !_equirectangularTexture ) return;

    _sampler.Unbind ( TEXTURE_SLOT );
    _equirectangularTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid GXEquirectangularToCubeMapMaterial::SetEquirectangularTexture ( GXTexture2D &texture )
{
    _equirectangularTexture = &texture;
}

GXVoid GXEquirectangularToCubeMapMaterial::EnableGammaCorrection ()
{
    _gamma = ENABLE_GAMMA_CORRECTION_VALUE;
}

GXVoid GXEquirectangularToCubeMapMaterial::DisableGammaCorrection ()
{
    _gamma = DISABLE_GAMMA_CORRECTION_VALUE;
}
