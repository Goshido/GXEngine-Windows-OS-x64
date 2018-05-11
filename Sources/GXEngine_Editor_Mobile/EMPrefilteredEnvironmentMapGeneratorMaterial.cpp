#include <GXEngine_Editor_Mobile/EMPrefilteredEnvironmentMapGeneratorMaterial.h>
#include <GXEngine/GXCameraPerspective.h>


#define TEXTURE_SLOT			0u

#define DEFAULT_ROUGHNESS		0.5f
#define DEFAULT_TOTAL_SAMPLES	512u

#define SQUARE_ASPECT_RATIO		1.0f
#define Z_NEAR					0.1f
#define Z_FAR					777.777f
#define PROJECTION_FOV_Y		GX_MATH_HALF_PI

#define VERTEX_SHADER			L"Shaders/System/VertexPass_vs.txt"
#define GEOMETRY_SHADER			L"Shaders/System/CubeMapSplitter_gs.txt"
#define FRAGMENT_SHADER			L"Shaders/Editor Mobile/PrefilteredEnvironmentMap_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMPrefilteredEnvironmentMapGeneratorMaterial::EMPrefilteredEnvironmentMapGeneratorMaterial () :
	environmentMap ( nullptr ),
	sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
	roughness ( DEFAULT_ROUGHNESS * DEFAULT_ROUGHNESS ),
	totalSamples ( static_cast<GXInt> ( DEFAULT_TOTAL_SAMPLES ) ),
	inverseTotalSamples ( 1.0f / static_cast<GXFloat> ( DEFAULT_TOTAL_SAMPLES ) )
{
	static const GLchar* samplerNames[ 1 ] = { "environmentSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

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

	viewProjectionMatricesLocation = shaderProgram.GetUniform ( "viewProjectionMatrices" );
	roughnessLocation = shaderProgram.GetUniform ( "roughness" );
	totalSamplesLocation = shaderProgram.GetUniform ( "totalSamples" );
	inverseTotalSamplesLocation = shaderProgram.GetUniform ( "inverseTotalSamples" );

	GXCameraPerspective camera ( PROJECTION_FOV_Y, SQUARE_ASPECT_RATIO, Z_NEAR, Z_FAR );
	camera.SetRotation ( 0.0f, GX_MATH_HALF_PI, 0.0f );
	viewProjectionMatrices[ 0 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( 0.0f, -GX_MATH_HALF_PI, 0.0f );
	viewProjectionMatrices[ 1 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( GX_MATH_HALF_PI, 0.0f, 0.0f );
	viewProjectionMatrices[ 2 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( -GX_MATH_HALF_PI, 0.0f, 0.0f );
	viewProjectionMatrices[ 3 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( 0.0f, 0.0f, 0.0f );
	viewProjectionMatrices[ 4 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( 0.0f, GX_MATH_PI, 0.0f );
	viewProjectionMatrices[ 5 ] = camera.GetCurrentFrameViewProjectionMatrix ();
}

EMPrefilteredEnvironmentMapGeneratorMaterial::~EMPrefilteredEnvironmentMapGeneratorMaterial ()
{
	// NOTHING
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !environmentMap ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	glUniformMatrix4fv ( viewProjectionMatricesLocation, 6, GL_FALSE, reinterpret_cast<const GLfloat*> ( viewProjectionMatrices ) );
	glUniform1f ( roughnessLocation, roughness );
	glUniform1i ( totalSamplesLocation, totalSamples );
	glUniform1f ( inverseTotalSamplesLocation, inverseTotalSamples );

	environmentMap->Bind ( TEXTURE_SLOT );
	sampler.Bind ( TEXTURE_SLOT );
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::Unbind ()
{
	if ( !environmentMap ) return;

	sampler.Unbind ( TEXTURE_SLOT );
	environmentMap->Unbind ();
	
	glUseProgram ( 0u );
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::SetEnvironmentMap ( GXTextureCubeMap &cubeMap )
{
	environmentMap = &cubeMap;
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::SetRoughness ( GXFloat newRoughness )
{
	roughness = newRoughness * newRoughness;
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::SetTotalSamples ( GXUShort samples )
{
	totalSamples = static_cast<GXInt> ( samples );
	inverseTotalSamples = 1.0f / static_cast<GXFloat> ( samples );
}
