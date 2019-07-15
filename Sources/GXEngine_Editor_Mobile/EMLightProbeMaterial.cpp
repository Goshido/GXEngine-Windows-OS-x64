#include <GXEngine_Editor_Mobile/EMLightProbeMaterial.h>
#include <GXEngine/GXCamera.h>


#define DIFFUSE_IRRADIANCE_SLOT             0u
#define PREFILTERED_ENVIRONMENT_MAP_SLOT    1u
#define BRDF_INTEGRATION_MAP_SLOT           2u
#define ALBEDO_SLOT                         3u
#define NORMAL_SLOT                         4u
#define EMISSION_SLOT                       5u
#define PARAMETER_SLOT                      6u
#define DEPTH_SLOT                          7u

#define VERTEX_SHADER                       L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER                     nullptr
#define FRAGMENT_SHADER                     L"Shaders/Editor Mobile/LightProbe_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMLightProbeMaterial::EMLightProbeMaterial ():
    _diffuseIrradianceTexture ( nullptr ),
    _prefilteredEnvironmentMapTexture ( nullptr ),
    _brdfIntegrationMapTexture ( nullptr ),
    _albedoTexture ( nullptr ),
    _normalTexture ( nullptr ),
    _emissionTexture ( nullptr ),
    _parameterTexture ( nullptr ),
    _depthTexture ( nullptr ),
    _diffuseIrradianceSampler ( GL_CLAMP_TO_EDGE, eGXResampling::Linear, 1.0f ),
    _prefilteredEnvironmentSampler ( GL_CLAMP_TO_EDGE, eGXResampling::Trilinear, 16.0f ),
    _brdfIntegrationSampler ( GL_CLAMP_TO_EDGE, eGXResampling::Linear, 1.0f ),
    _gBufferSampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f )
{
    constexpr GLchar* samplerNames[ 8u ] = { "diffuseIrradianceSampler", "prefilteredEnvironmentMapSampler", "brdfIntegrationMapSampler", "albedoSampler", "normalSampler", "emissionSampler", "parameterSampler", "depthSampler" };
    constexpr GLuint sampleLocations[ 8u ] = { DIFFUSE_IRRADIANCE_SLOT, PREFILTERED_ENVIRONMENT_MAP_SLOT, BRDF_INTEGRATION_MAP_SLOT, ALBEDO_SLOT, NORMAL_SLOT, EMISSION_SLOT, PARAMETER_SLOT, DEPTH_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 8u;
    si._samplerNames = samplerNames;
    si._samplerLocations = sampleLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _viewerLocationWorldLocation = _shaderProgram.GetUniform ( "viewerLocationWorld" );
    _prefilteredEnvironmentMapLODsLocation = _shaderProgram.GetUniform ( "prefilteredEnvironmentMapLODs" );
    _inverseViewMatrixLocation = _shaderProgram.GetUniform ( "inverseViewMatrix" );
    _inverseViewProjectionMatrixLocation = _shaderProgram.GetUniform ( "inverseViewProjectionMatrix" );
}

EMLightProbeMaterial::~EMLightProbeMaterial ()
{
    // NOTHING
}

GXVoid EMLightProbeMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_diffuseIrradianceTexture || !_prefilteredEnvironmentMapTexture || !_brdfIntegrationMapTexture || !_albedoTexture || !_normalTexture || !_emissionTexture || !_parameterTexture || !_depthTexture )
        return;

    glUseProgram ( _shaderProgram.GetProgram () );

    GXCamera* camera = GXCamera::GetActiveCamera ();

    const GXMat4& viewerModelMatrix = camera->GetCurrentFrameModelMatrix ();
    GXVec3 viewerLocationWorld;
    viewerModelMatrix.GetW ( viewerLocationWorld );
    glUniform3fv ( _viewerLocationWorldLocation, 1, viewerLocationWorld._data );
    glUniform1f ( _prefilteredEnvironmentMapLODsLocation, static_cast<GXFloat> ( _prefilteredEnvironmentMapTexture->GetLevelOfDetailNumber () ) );

    GXMat3 inverseViewMatrix ( viewerModelMatrix );
    glUniformMatrix3fv ( _inverseViewMatrixLocation, 1, GL_FALSE, inverseViewMatrix._data );
    glUniformMatrix4fv ( _inverseViewProjectionMatrixLocation, 1, GL_FALSE, camera->GetCurrentFrameInverseViewProjectionMatrix ()._data );

    _diffuseIrradianceTexture->Bind ( DIFFUSE_IRRADIANCE_SLOT );
    _diffuseIrradianceSampler.Bind ( DIFFUSE_IRRADIANCE_SLOT );

    _prefilteredEnvironmentMapTexture->Bind ( PREFILTERED_ENVIRONMENT_MAP_SLOT );
    _prefilteredEnvironmentSampler.Bind ( PREFILTERED_ENVIRONMENT_MAP_SLOT );

    _brdfIntegrationMapTexture->Bind ( BRDF_INTEGRATION_MAP_SLOT );
    _brdfIntegrationSampler.Bind ( BRDF_INTEGRATION_MAP_SLOT );

    _albedoTexture->Bind ( ALBEDO_SLOT );
    _gBufferSampler.Bind ( ALBEDO_SLOT );

    _normalTexture->Bind ( NORMAL_SLOT );
    _gBufferSampler.Bind ( NORMAL_SLOT );

    _emissionTexture->Bind ( EMISSION_SLOT );
    _gBufferSampler.Bind ( EMISSION_SLOT );

    _parameterTexture->Bind ( PARAMETER_SLOT );
    _gBufferSampler.Bind ( PARAMETER_SLOT );

    _depthTexture->Bind ( DEPTH_SLOT );
    _gBufferSampler.Bind ( DEPTH_SLOT );
}

GXVoid EMLightProbeMaterial::Unbind ()
{
    if ( !_diffuseIrradianceTexture || !_prefilteredEnvironmentMapTexture || !_brdfIntegrationMapTexture || !_albedoTexture || !_normalTexture || !_emissionTexture || !_parameterTexture || !_depthTexture )
        return;

    _diffuseIrradianceSampler.Unbind ( DIFFUSE_IRRADIANCE_SLOT );
    _diffuseIrradianceTexture->Unbind ();

    _prefilteredEnvironmentSampler.Unbind ( PREFILTERED_ENVIRONMENT_MAP_SLOT );
    _prefilteredEnvironmentMapTexture->Unbind ();

    _brdfIntegrationSampler.Unbind ( BRDF_INTEGRATION_MAP_SLOT );
    _brdfIntegrationMapTexture->Unbind ();

    _gBufferSampler.Unbind ( ALBEDO_SLOT );
    _albedoTexture->Unbind ();

    _gBufferSampler.Unbind ( NORMAL_SLOT );
    _normalTexture->Unbind ();

    _gBufferSampler.Unbind ( EMISSION_SLOT );
    _emissionTexture->Unbind ();

    _gBufferSampler.Unbind ( PARAMETER_SLOT );
    _parameterTexture->Unbind ();

    _gBufferSampler.Unbind ( DEPTH_SLOT );
    _depthTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMLightProbeMaterial::SetDiffuseIrradianceTexture ( GXTextureCubeMap &cubeMap )
{
    _diffuseIrradianceTexture = &cubeMap;
}

GXVoid EMLightProbeMaterial::SetPrefilteredEnvironmentMapTexture ( GXTextureCubeMap &cubeMap )
{
    _prefilteredEnvironmentMapTexture = &cubeMap;
}

GXVoid EMLightProbeMaterial::SetBRDFIntegrationMapTexture ( GXTexture2D &texture )
{
    _brdfIntegrationMapTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetAlbedoTexture ( GXTexture2D &texture )
{
    _albedoTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetNormalTexture ( GXTexture2D &texture )
{
    _normalTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetEmissionTexture ( GXTexture2D &texture )
{
    _emissionTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetParameterTexture ( GXTexture2D &texture )
{
    _parameterTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetDepthTexture ( GXTexture2D &texture )
{
    _depthTexture = &texture;
}
