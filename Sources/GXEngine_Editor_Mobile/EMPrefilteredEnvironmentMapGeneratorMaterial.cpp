#include <GXEngine_Editor_Mobile/EMPrefilteredEnvironmentMapGeneratorMaterial.h>
#include <GXEngine/GXCameraPerspective.h>


#define VERTEX_SHADER			L"Shaders/System/VertexPass_vs.txt"
#define GEOMETRY_SHADER			L"Shaders/System/CubeMapSplitter_gs.txt"
#define FRAGMENT_SHADER			L"Shaders/Editor Mobile/PrefilteredEnvironmentMap_fs.txt"

#define TEXTURE_SLOT			0

#define DEFAULT_ROUGHNESS		0.5f
#define DEFAULT_TOTAL_SAMPLES	512

#define SQUARE_ASPECT_RATIO		1.0f
#define Z_NEAR					0.1f
#define Z_FAR					777.777f
#define PROJECTION_FOV_Y		GX_MATH_HALF_PI


EMPrefilteredEnvironmentMapGeneratorMaterial::EMPrefilteredEnvironmentMapGeneratorMaterial ()
{
	static const GLchar* samplerNames[ 1 ] = { "environmentSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 1;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	viewProjectionMatricesLocation = shaderProgram.GetUniform ( "viewProjectionMatrices" );
	roughnessLocation = shaderProgram.GetUniform ( "roughness" );
	totalSamplesLocation = shaderProgram.GetUniform ( "totalSamples" );
	inverseTotalSamplesLocation = shaderProgram.GetUniform ( "inverseTotalSamples" );

	environmentMap = nullptr;

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

	SetRoughness ( DEFAULT_ROUGHNESS );
	SetTotalSamples ( DEFAULT_TOTAL_SAMPLES );
}

EMPrefilteredEnvironmentMapGeneratorMaterial::~EMPrefilteredEnvironmentMapGeneratorMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !environmentMap ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	glUniformMatrix4fv ( viewProjectionMatricesLocation, 6, GL_FALSE, (const GLfloat*)viewProjectionMatrices );
	glUniform1f ( roughnessLocation, roughness );
	glUniform1i ( totalSamplesLocation, totalSamples );
	glUniform1f ( inverseTotalSamplesLocation, inverseTotalSamples );

	environmentMap->Bind ( TEXTURE_SLOT );
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::Unbind ()
{
	if ( !environmentMap ) return;

	glUseProgram ( 0 );

	environmentMap->Unbind ();
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::SetEnvironmentMap ( GXTextureCubeMap &cubeMap )
{
	environmentMap = &cubeMap;
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::SetRoughness ( GXFloat roughness )
{
	this->roughness = roughness * roughness;
}

GXVoid EMPrefilteredEnvironmentMapGeneratorMaterial::SetTotalSamples ( GXUShort samples )
{
	totalSamples = (GXInt)samples;
	inverseTotalSamples = 1.0f / (GXFloat)samples;
}