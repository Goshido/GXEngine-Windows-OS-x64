#include <GXEngine_Editor_Mobile/EMDiffuseIrradianceGeneratorMaterial.h>
#include <GXEngine/GXCameraPerspective.h>


#define TEXTURE_SLOT            0u
#define DEFAULT_ANGLE_STEP      0.025f

#define SQUARE_ASPECT_RATIO     1.0f
#define Z_NEAR                  0.1f
#define Z_FAR                   777.777f
#define PROJECTION_FOV_Y        GX_MATH_HALF_PI

#define VERTEX_SHADER           L"Shaders/System/VertexPass_vs.txt"
#define GEOMETRY_SHADER         L"Shaders/System/CubeMapSplitter_gs.txt"
#define FRAGMENT_SHADER         L"Shaders/Editor Mobile/DiffuseIrradianceGenerator_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMDiffuseIrradianceGeneratorMaterial::EMDiffuseIrradianceGeneratorMaterial ():
    _environmentMap ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
    _angleStep ( DEFAULT_ANGLE_STEP )
{
    constexpr GLchar* samplerNames[ 1u ] = { "environmentSampler" };
    constexpr GLuint samplerLocations[ 1u ] = { TEXTURE_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 1;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _viewProjectionMatricesLocation = _shaderProgram.GetUniform ( "viewProjectionMatrices" );
    _angleStepLocation = _shaderProgram.GetUniform ( "angleStep" );

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

EMDiffuseIrradianceGeneratorMaterial::~EMDiffuseIrradianceGeneratorMaterial ()
{
    // NOTHING
}

GXVoid EMDiffuseIrradianceGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_environmentMap ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    glUniformMatrix4fv ( _viewProjectionMatricesLocation, 6, GL_FALSE, (const GLfloat*)_viewProjectionMatrices );
    glUniform1f ( _angleStepLocation, _angleStep );

    _environmentMap->Bind ( TEXTURE_SLOT );
    _sampler.Bind ( TEXTURE_SLOT );
}

GXVoid EMDiffuseIrradianceGeneratorMaterial::Unbind ()
{
    if ( !_environmentMap ) return;

    _sampler.Unbind ( TEXTURE_SLOT );
    _environmentMap->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMDiffuseIrradianceGeneratorMaterial::SetEnvironmentMap ( GXTextureCubeMap &cubeMap )
{
    _environmentMap = &cubeMap;
}

GXUInt EMDiffuseIrradianceGeneratorMaterial::SetAngleStep ( GXFloat radians )
{
    _angleStep = radians;
    GXFloat invAngleStep = 1.0f / _angleStep;
    return static_cast<GXUInt> ( floorf ( GX_MATH_HALF_PI * invAngleStep ) + floorf ( GX_MATH_DOUBLE_PI * invAngleStep ) );
}
