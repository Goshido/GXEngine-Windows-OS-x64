#include <GXEngine_Editor_Mobile/EMEnvironmentMapMaterial.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>


#define VERTEX_SHADER						L"Shaders/Editor Mobile/EnvironmentMap_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/EnvironmentMap_fs.txt"

#define ENVIRONMENT_SLOT					0
#define DEPTH_SLOT							1

#define DEFAULT_SCREEN_WIDTH				1280
#define DEFAULT_SCREEN_HEIGHT				720
#define DEFAULT_DELTA_TIME					1.0f
#define DEFAULT_ENVIRONMENT_QUASI_DISTANCE	77.7f

#define ZERO_VELOCITY_BLUR_X				0.5f
#define ZERO_VELOCITY_BLUR_Y				0.5f


EMEnvironmentMapMaterial::EMEnvironmentMapMaterial ()
{
	environmentTexture = nullptr;
	depthTexture = nullptr;

	static const GLchar* samplerNames[ 2 ] = { "environmentSampler", "depthSampler" };
	static const GLuint samplerLocations[ 2 ] = { ENVIRONMENT_SLOT, DEPTH_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 2;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	modelViewProjectionMatrixLocation = shaderProgram.GetUniform ( "modelViewProjectionMatrix" );
	inverseScreenResolutionLocation = shaderProgram.GetUniform ( "inverseScreenResolution" );
	velocityBlurLocation = shaderProgram.GetUniform ( "velocityBlur" );

	SetScreenResolution ( DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT );
	SetDeltaTime ( DEFAULT_DELTA_TIME );
	SetEnvironmentQuasiDistance ( DEFAULT_ENVIRONMENT_QUASI_DISTANCE );
}

EMEnvironmentMapMaterial::~EMEnvironmentMapMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMEnvironmentMapMaterial::Bind ( const GXTransform &transform )
{
	if ( !environmentTexture || !depthTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* camera = GXCamera::GetActiveCamera ();
	const GXMat4& currentFrameViewProjectionMatrix = camera->GetCurrentFrameViewProjectionMatrix ();

	GXMat4 modelViewProjectionMatrix;
	GXMulMat4Mat4 ( modelViewProjectionMatrix, transform.GetCurrentFrameModelMatrix (), currentFrameViewProjectionMatrix );

	GXVec2 currentFrameEnvironmentQuasiLocationScreen ( screenResolution.x * 0.5f, screenResolution.y * 0.5f );

	GXVec3 currentFrameCameraLocationWorld;
	camera->GetLocation ( currentFrameCameraLocationWorld );

	const GXMat4& lastFrameCameraModelMatrix = camera->GetLastFrameModelMatrix ();
	GXVec3 lastFrameEnvironmentQuasiLocationWorld;
	lastFrameCameraModelMatrix.GetZ ( lastFrameEnvironmentQuasiLocationWorld );

	GXMulVec3Scalar ( lastFrameEnvironmentQuasiLocationWorld, lastFrameEnvironmentQuasiLocationWorld, environmentQuasiDistance );
	GXSumVec3Vec3 ( lastFrameEnvironmentQuasiLocationWorld, lastFrameEnvironmentQuasiLocationWorld, currentFrameCameraLocationWorld );
	
	GXVec4 v ( lastFrameEnvironmentQuasiLocationWorld, 1.0f );
	GXVec4 temp;
	GXMulVec4Mat4 ( temp, v, currentFrameViewProjectionMatrix );

	GXVec2 lastFrameEnvironmentQuasiLocationScreen ( temp.x, temp.y );
	GXFloat invW = 1.0f / temp.w;

	GXMulVec2Scalar ( lastFrameEnvironmentQuasiLocationScreen, lastFrameEnvironmentQuasiLocationScreen, invW );

	static const GXVec2 biasScale ( 0.5f, 0.5f );
	static const GXVec2 biasOffset ( 0.5f, 0.5f );

	GXMulVec2Vec2 ( lastFrameEnvironmentQuasiLocationScreen, lastFrameEnvironmentQuasiLocationScreen, biasScale );
	GXSumVec2Vec2 ( lastFrameEnvironmentQuasiLocationScreen, lastFrameEnvironmentQuasiLocationScreen, biasOffset );

	GXMulVec2Vec2 ( lastFrameEnvironmentQuasiLocationScreen, lastFrameEnvironmentQuasiLocationScreen, screenResolution );

	GXVec2 velocityImage;
	GXSubVec2Vec2 ( velocityImage, currentFrameEnvironmentQuasiLocationScreen, lastFrameEnvironmentQuasiLocationScreen );
	GXMulVec2Scalar ( velocityImage, velocityImage, inverseDeltaTime );

	GXVec2 halfSpreadVelocityImage;
	EMRenderer& renderer = EMRenderer::GetInstance ();
	GXMulVec2Scalar ( halfSpreadVelocityImage, velocityImage, 0.5f * renderer.GetMotionBlurExposure () );

	GXVec2 velocityBlur;

	if ( halfSpreadVelocityImage.x == 0.0f && halfSpreadVelocityImage.y == 0.0f )
		velocityBlur = GXCreateVec2 ( ZERO_VELOCITY_BLUR_X, ZERO_VELOCITY_BLUR_Y );
	else
	{
		GXFloat maximumMotionBlurSamples = (GXFloat)renderer.GetMaximumMotionBlurSamples ();
		GXFloat halfSpreadVelocityMagnitudeImage = GXMinf ( GXLengthVec2 ( halfSpreadVelocityImage ), maximumMotionBlurSamples );

		velocityBlur = velocityImage;
		GXNormalizeVec2 ( velocityBlur );
		GXMulVec2Scalar ( velocityBlur, velocityBlur, halfSpreadVelocityMagnitudeImage / maximumMotionBlurSamples );

		GXMulVec2Vec2 ( velocityBlur, velocityBlur, biasScale );
		GXSumVec2Vec2 ( velocityBlur, velocityBlur, biasOffset );
	}

	glUniformMatrix4fv ( modelViewProjectionMatrixLocation, 1, GL_FALSE, modelViewProjectionMatrix.arr );
	glUniform2fv ( inverseScreenResolutionLocation, 1, inverseScreenResolution.arr );
	glUniform2fv ( velocityBlurLocation, 1, velocityBlur.arr );

	environmentTexture->Bind ( ENVIRONMENT_SLOT );
	depthTexture->Bind ( DEPTH_SLOT );
}

GXVoid EMEnvironmentMapMaterial::Unbind ()
{
	if ( !environmentTexture || !depthTexture ) return;

	glUseProgram ( 0 );
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
	screenResolution.x = (GXFloat)width;
	screenResolution.y = (GXFloat)height;

	inverseScreenResolution.x = 1.0f / screenResolution.x;
	inverseScreenResolution.y = 1.0f / screenResolution.y;
}

GXVoid EMEnvironmentMapMaterial::SetDeltaTime ( GXFloat seconds )
{
	inverseDeltaTime = 1.0f / seconds;
}

GXVoid EMEnvironmentMapMaterial::SetEnvironmentQuasiDistance ( GXFloat meters )
{
	environmentQuasiDistance = meters;
}
