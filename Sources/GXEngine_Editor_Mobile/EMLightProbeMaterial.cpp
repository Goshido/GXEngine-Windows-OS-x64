#include <GXEngine_Editor_Mobile/EMLightProbeMaterial.h>
#include <GXEngine/GXCamera.h>


#define DIFFUSE_IRRADIANCE_SLOT						0u
#define PREFILTERED_ENVIRONMENT_MAP_SLOT			1u
#define BRDF_INTEGRATION_MAP_SLOT					2u
#define ALBEDO_SLOT									3u
#define NORMAL_SLOT									4u
#define EMISSION_SLOT								5u
#define PARAMETER_SLOT								6u
#define DEPTH_SLOT									7u

#define VERTEX_SHADER								L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER								nullptr
#define FRAGMENT_SHADER								L"Shaders/Editor Mobile/LightProbe_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMLightProbeMaterial::EMLightProbeMaterial ():
	diffuseIrradianceTexture ( nullptr ),
	prefilteredEnvironmentMapTexture ( nullptr ),
	brdfIntegrationMapTexture ( nullptr ),
	albedoTexture ( nullptr ),
	normalTexture ( nullptr ),
	emissionTexture ( nullptr ),
	parameterTexture ( nullptr ),
	depthTexture ( nullptr )
{
	static const GLchar* samplerNames[ 8 ] = { "diffuseIrradianceSampler", "prefilteredEnvironmentMapSampler", "brdfIntegrationMapSampler", "albedoSampler", "normalSampler", "emissionSampler", "parameterSampler", "depthSampler" };
	static const GLuint sampleLocations[ 8 ] = { DIFFUSE_IRRADIANCE_SLOT, PREFILTERED_ENVIRONMENT_MAP_SLOT, BRDF_INTEGRATION_MAP_SLOT, ALBEDO_SLOT, NORMAL_SLOT, EMISSION_SLOT, PARAMETER_SLOT, DEPTH_SLOT };

	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 8u;
	si.samplerNames = samplerNames;
	si.samplerLocations = sampleLocations;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	viewerLocationWorldLocation = shaderProgram.GetUniform ( "viewerLocationWorld" );
	prefilteredEnvironmentMapLODsLocation = shaderProgram.GetUniform ( "prefilteredEnvironmentMapLODs" );
	inverseViewMatrixLocation = shaderProgram.GetUniform ( "inverseViewMatrix" );
	inverseViewProjectionMatrixLocation = shaderProgram.GetUniform ( "inverseViewProjectionMatrix" );
}

EMLightProbeMaterial::~EMLightProbeMaterial ()
{
	// NOTHING
}

GXVoid EMLightProbeMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !diffuseIrradianceTexture || !prefilteredEnvironmentMapTexture || !brdfIntegrationMapTexture || !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture || !depthTexture )
		return;

	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* camera = GXCamera::GetActiveCamera ();

	const GXMat4& viewerModelMatrix = camera->GetCurrentFrameModelMatrix ();
	GXVec3 viewerLocationWorld;
	viewerModelMatrix.GetW ( viewerLocationWorld );
	glUniform3fv ( viewerLocationWorldLocation, 1, viewerLocationWorld.data );
	glUniform1f ( prefilteredEnvironmentMapLODsLocation, static_cast<GXFloat> ( prefilteredEnvironmentMapTexture->GetLevelOfDetailNumber () ) );

	GXMat3 inverseViewMatrix ( viewerModelMatrix );
	glUniformMatrix3fv ( inverseViewMatrixLocation, 1, GL_FALSE, inverseViewMatrix.data );
	glUniformMatrix4fv ( inverseViewProjectionMatrixLocation, 1, GL_FALSE, camera->GetCurrentFrameInverseViewProjectionMatrix ().data );

	diffuseIrradianceTexture->Bind ( DIFFUSE_IRRADIANCE_SLOT );
	prefilteredEnvironmentMapTexture->Bind ( PREFILTERED_ENVIRONMENT_MAP_SLOT );
	brdfIntegrationMapTexture->Bind ( BRDF_INTEGRATION_MAP_SLOT );
	albedoTexture->Bind ( ALBEDO_SLOT );
	normalTexture->Bind ( NORMAL_SLOT );
	emissionTexture->Bind ( EMISSION_SLOT );
	parameterTexture->Bind ( PARAMETER_SLOT );
	depthTexture->Bind ( DEPTH_SLOT );
}

GXVoid EMLightProbeMaterial::Unbind ()
{
	if ( !diffuseIrradianceTexture || !prefilteredEnvironmentMapTexture || !brdfIntegrationMapTexture || !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture || !depthTexture )
		return;

	glUseProgram ( 0u );

	diffuseIrradianceTexture->Unbind ();
	prefilteredEnvironmentMapTexture->Unbind ();
	brdfIntegrationMapTexture->Unbind ();
	albedoTexture->Unbind ();
	normalTexture->Unbind ();
	emissionTexture->Unbind ();
	parameterTexture->Unbind ();
	depthTexture->Unbind ();
}

GXVoid EMLightProbeMaterial::SetDiffuseIrradianceTexture ( GXTextureCubeMap &cubeMap )
{
	diffuseIrradianceTexture = &cubeMap;
}

GXVoid EMLightProbeMaterial::SetPrefilteredEnvironmentMapTexture ( GXTextureCubeMap &cubeMap )
{
	prefilteredEnvironmentMapTexture = &cubeMap;
}

GXVoid EMLightProbeMaterial::SetBRDFIntegrationMapTexture ( GXTexture2D &texture )
{
	brdfIntegrationMapTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetAlbedoTexture ( GXTexture2D &texture )
{
	albedoTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetNormalTexture ( GXTexture2D &texture )
{
	normalTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetEmissionTexture ( GXTexture2D &texture )
{
	emissionTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetParameterTexture ( GXTexture2D &texture )
{
	parameterTexture = &texture;
}

GXVoid EMLightProbeMaterial::SetDepthTexture ( GXTexture2D &texture )
{
	depthTexture = &texture;
}
