#include <GXEngine_Editor_Mobile/EMPrefilteredEnvironmentMapGeneratorMaterial.h>
#include <GXEngine/GXCameraPerspective.h>


#define TEXTURE_SLOT                0u

#define DEFAULT_ROUGHNESS           0.5f
#define DEFAULT_TOTAL_SAMPLES       512u

#define SQUARE_ASPECT_RATIO         1.0f
#define Z_NEAR                      0.1f
#define Z_FAR                       777.777f
#define PROJECTION_FOV_Y            GX_MATH_HALF_PI

#define VERTEX_SHADER               L"Shaders/System/VertexPass_vs.txt"
#define GEOMETRY_SHADER             L"Shaders/System/CubeMapSplitter_gs.txt"
#define FRAGMENT_SHADER             L"Shaders/Editor Mobile/PrefilteredEnvironmentMap_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMPrefilteredEnvironmentMapGeneratorMaterial::EMPrefilteredEnvironmentMapGeneratorMaterial ():
    _environmentMap ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
    _roughness ( DEFAULT_ROUGHNESS * DEFAULT_ROUGHNESS ),
    _totalSamples ( static_cast<GXInt> ( DEFAULT_TOTAL_SAMPLES ) ),
    _inverseTotalSamples ( 1.0f / static_cast<GXFloat> ( DEFAULT_TOTAL_SAMPLES ) )
{
    constexpr GLchar* samplerNames[ 1u ] = { "environmentSampler" };
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

    _viewProjectionMatricesLocation = _shaderProgram.GetUniform ( "viewProjectionMatrices" );
    _roughnessLocation = _shaderProgram.GetUniform ( "roughness" );
    _totalSamplesLocation = _shaderProgram.GetUniform ( "totalSamples" );
    _inverseTotalSamplesLocation = _shaderProgram.GetUniform ( "inverseTotalSamples" );

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
}

EMPrefilteredEnvironmentMapGeneratorMaterial::~EMPrefilteredEnvironmentMapGeneratorMaterial ()
{
    // NOTHING
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_environmentMap ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    glUniformMatrix4fv ( _viewProjectionMatricesLocation, 6, GL_FALSE, reinterpret_cast<const GLfloat*> ( _viewProjectionMatrices ) );
    glUniform1f ( _roughnessLocation, _roughness );
    glUniform1i ( _totalSamplesLocation, _totalSamples );
    glUniform1f ( _inverseTotalSamplesLocation, _inverseTotalSamples );

    _environmentMap->Bind ( TEXTURE_SLOT );
    _sampler.Bind ( TEXTURE_SLOT );
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::Unbind ()
{
    if ( !_environmentMap ) return;

    _sampler.Unbind ( TEXTURE_SLOT );
    _environmentMap->Unbind ();
    
    glUseProgram ( 0u );
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::SetEnvironmentMap ( GXTextureCubeMap &cubeMap )
{
    _environmentMap = &cubeMap;
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::SetRoughness ( GXFloat newRoughness )
{
    _roughness = newRoughness * newRoughness;
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::SetTotalSamples ( GXUShort samples )
{
    _totalSamples = static_cast<GXInt> ( samples );
    _inverseTotalSamples = 1.0f / static_cast<GXFloat> ( samples );
}
