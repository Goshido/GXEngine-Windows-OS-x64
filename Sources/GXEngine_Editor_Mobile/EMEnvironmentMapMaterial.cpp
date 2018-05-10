#include <GXEngine_Editor_Mobile/EMEnvironmentMapMaterial.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>


#define ENVIRONMENT_SLOT					0u
#define DEPTH_SLOT							1u

#define DEFAULT_SCREEN_WIDTH				1280.0f
#define DEFAULT_SCREEN_HEIGHT				720.0f
#define DEFAULT_DELTA_TIME					1.0f
#define DEFAULT_ENVIRONMENT_QUASI_DISTANCE	77.7f

#define ZERO_VELOCITY_BLUR_X				0.0f
#define ZERO_VELOCITY_BLUR_Y				0.0f

#define VERTEX_SHADER						L"Shaders/Editor Mobile/EnvironmentMap_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/EnvironmentMap_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMEnvironmentMapMaterial::EMEnvironmentMapMaterial ():
	environmentTexture ( nullptr ),
	depthTexture ( nullptr ),
	screenResolution ( DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT ),
	inverseScreenResolution ( 1.0f / DEFAULT_SCREEN_WIDTH, 1.0f / DEFAULT_SCREEN_HEIGHT ),
	inverseDeltaTime ( 1.0f / DEFAULT_DELTA_TIME ),
	environmentQuasiDistance ( DEFAULT_ENVIRONMENT_QUASI_DISTANCE )
{
	static const GLchar* samplerNames[ 2 ] = { "environmentSampler", "depthSampler" };
	static const GLuint samplerLocations[ 2 ] = { ENVIRONMENT_SLOT, DEPTH_SLOT };

	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 2u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	modelViewProjectionMatrixLocation = shaderProgram.GetUniform ( "modelViewProjectionMatrix" );
	inverseScreenResolutionLocation = shaderProgram.GetUniform ( "inverseScreenResolution" );
	velocityBlurLocation = shaderProgram.GetUniform ( "velocityBlur" );
}

EMEnvironmentMapMaterial::~EMEnvironmentMapMaterial ()
{
	// NOTHING
}

GXVoid EMEnvironmentMapMaterial::Bind ( const GXTransform &transform )
{
	if ( !environmentTexture || !depthTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* camera = GXCamera::GetActiveCamera ();
	const GXMat4& currentFrameViewProjectionMatrix = camera->GetCurrentFrameViewProjectionMatrix ();

	GXMat4 modelViewProjectionMatrix;
	modelViewProjectionMatrix.Multiply ( transform.GetCurrentFrameModelMatrix (), currentFrameViewProjectionMatrix );

	GXVec2 currentFrameEnvironmentQuasiLocationScreen;
	currentFrameEnvironmentQuasiLocationScreen.Multiply ( screenResolution, 0.5f );

	GXVec3 currentFrameCameraLocationWorld;
	camera->GetLocation ( currentFrameCameraLocationWorld );

	const GXMat4& lastFrameCameraModelMatrix = camera->GetLastFrameModelMatrix ();
	GXVec3 lastFrameEnvironmentQuasiLocationWorld;
	lastFrameCameraModelMatrix.GetZ ( lastFrameEnvironmentQuasiLocationWorld );

	lastFrameEnvironmentQuasiLocationWorld.Multiply ( lastFrameEnvironmentQuasiLocationWorld, environmentQuasiDistance );
	lastFrameEnvironmentQuasiLocationWorld.Sum ( lastFrameEnvironmentQuasiLocationWorld, currentFrameCameraLocationWorld );
	
	GXVec4 v ( lastFrameEnvironmentQuasiLocationWorld, 1.0f );
	GXVec4 temp;
	currentFrameViewProjectionMatrix.MultiplyVectorMatrix ( temp, v );

	GXVec2 lastFrameEnvironmentQuasiLocationScreen ( temp.GetX (), temp.GetY () );
	lastFrameEnvironmentQuasiLocationScreen.Multiply ( lastFrameEnvironmentQuasiLocationScreen, 1.0f / temp.GetW () );

	static const GXVec2 biasScale ( 0.5f, 0.5f );
	static const GXVec2 biasOffset ( 0.5f, 0.5f );

	lastFrameEnvironmentQuasiLocationScreen.Multiply ( lastFrameEnvironmentQuasiLocationScreen, biasScale );
	lastFrameEnvironmentQuasiLocationScreen.Sum ( lastFrameEnvironmentQuasiLocationScreen, biasOffset );

	lastFrameEnvironmentQuasiLocationScreen.Multiply ( lastFrameEnvironmentQuasiLocationScreen, screenResolution );

	GXVec2 velocityImage;
	velocityImage.Substract ( currentFrameEnvironmentQuasiLocationScreen, lastFrameEnvironmentQuasiLocationScreen );
	velocityImage.Multiply ( velocityImage, inverseDeltaTime );

	GXVec2 halfSpreadVelocityImage;
	EMRenderer& renderer = EMRenderer::GetInstance ();
	halfSpreadVelocityImage.Multiply ( velocityImage, 0.5f * renderer.GetMotionBlurExposure () );

	GXVec2 velocityBlur;

	if ( halfSpreadVelocityImage.GetX () == 0.0f && halfSpreadVelocityImage.GetY () == 0.0f )
	{
		velocityBlur.Init ( ZERO_VELOCITY_BLUR_X, ZERO_VELOCITY_BLUR_Y );
	}
	else
	{
		GXFloat maximumMotionBlurSamples = static_cast<GXFloat> ( renderer.GetMaximumMotionBlurSamples () );
		GXFloat halfSpreadVelocityMagnitudeImage = GXMinf ( halfSpreadVelocityImage.Length (), maximumMotionBlurSamples );

		velocityBlur = velocityImage;
		velocityBlur.Normalize ();
		velocityBlur.Multiply ( velocityBlur, halfSpreadVelocityMagnitudeImage / maximumMotionBlurSamples );
	}

	glUniformMatrix4fv ( modelViewProjectionMatrixLocation, 1, GL_FALSE, modelViewProjectionMatrix.data );
	glUniform2fv ( inverseScreenResolutionLocation, 1, inverseScreenResolution.data );
	glUniform2fv ( velocityBlurLocation, 1, velocityBlur.data );

	environmentTexture->Bind ( ENVIRONMENT_SLOT );
	depthTexture->Bind ( DEPTH_SLOT );
}

GXVoid EMEnvironmentMapMaterial::Unbind ()
{
	if ( !environmentTexture || !depthTexture ) return;

	glUseProgram ( 0u );
	environmentTexture->Unbind ();
	depthTexture->Unbind ();
}

GXVoid EMEnvironmentMapMaterial::SetEnvironmentMap ( GXTextureCubeMap &cubeMap )
{
	environmentTexture = &cubeMap;
}

GXVoid EMEnvironmentMapMaterial::SetDepthTexture ( GXTexture2D &texture )
{
	depthTexture = &texture;
}

GXVoid EMEnvironmentMapMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
	screenResolution.Init ( static_cast<GXFloat> ( width ), static_cast<GXFloat> ( height ) );
	inverseScreenResolution.Init ( 1.0f / screenResolution.data[ 0 ], 1.0f / screenResolution.data[ 1 ] );
}

GXVoid EMEnvironmentMapMaterial::SetDeltaTime ( GXFloat seconds )
{
	inverseDeltaTime = 1.0f / seconds;
}

GXVoid EMEnvironmentMapMaterial::SetEnvironmentQuasiDistance ( GXFloat meters )
{
	environmentQuasiDistance = meters;
}
