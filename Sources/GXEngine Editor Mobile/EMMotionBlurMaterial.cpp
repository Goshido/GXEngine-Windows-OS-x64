#include <GXEngine_Editor_Mobile/EMMotionBlurMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_BLUR_STRENGHT			0.003f
#define DEFAULT_BLUR_MINIMUM_VELOCITY	1.0e-5f
#define DEFAULT_BLUR_DEPTH_FACTOR_VIEW	1.2f
#define DEFAULT_SCREEN_WIDTH			1280
#define DEFAULT_SCREEN_HEIGHT			720

#define VELOCITY_SLOT					0
#define DEPTH_SLOT						1
#define IMAGE_SLOT						2

#define VERTEX_SHADER					L"Shaders/Editor Mobile/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER					nullptr
#define FRAGMENT_SHADER					L"Shaders/Editor Mobile/MotionBlur_fs.txt"


EMMotionBlurMaterial::EMMotionBlurMaterial ()
{
	velocityTexture = nullptr;
	depthTexture = nullptr;
	imageTexture = nullptr;
	objectHighTexture = nullptr;
	objectLowTexture = nullptr;

	static const GLchar* samplerNames[ 3 ] = { "velocitySampler", "depthSampler", "imageSampler" };
	static const GLuint samplerLocations[ 3 ] = { VELOCITY_SLOT, DEPTH_SLOT, IMAGE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 3;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	blurStrengthLocation = shaderProgram.GetUniform ( "blurStrength" );
	blurMinimumvelocityLocation = shaderProgram.GetUniform ( "blurMinimumVelocity" );
	inverseScreenResolutionLocation = shaderProgram.GetUniform ( "inverseScreenResolution" );
	inverseProjectionMatrixLocation = shaderProgram.GetUniform ( "inverseProjectionMatrix" );
	blurDepthFactorViewLocation = shaderProgram.GetUniform ( "blurDepthFactorView" );

	SetBlurStrength ( DEFAULT_BLUR_STRENGHT );
	SetBlurMinimumVelocity ( DEFAULT_BLUR_MINIMUM_VELOCITY );
	SetBlurDepthFactorView ( DEFAULT_BLUR_DEPTH_FACTOR_VIEW );
	SetScreenResolution ( DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT );
}

EMMotionBlurMaterial::~EMMotionBlurMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMMotionBlurMaterial::Bind ( const GXTransform& /*transform*/ ) const
{
	if ( !velocityTexture || !depthTexture || !imageTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	const GXMat4& inverseProjectionMatrix = GXCamera::GetActiveCamera ()->GetCurrentInverseProjectionMatrix ();

	glUniform1f ( blurStrengthLocation, blurStrength );
	glUniform1f ( blurMinimumvelocityLocation, blurMinimumVelocity );
	glUniform2f ( inverseScreenResolutionLocation, inverseScreenResolution.x, inverseScreenResolution.y );
	glUniform1f ( blurDepthFactorViewLocation, blurDepthFactorView );
	glUniformMatrix4fv ( inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix.arr );

	velocityTexture->Bind ( VELOCITY_SLOT );
	depthTexture->Bind ( DEPTH_SLOT );
	imageTexture->Bind ( IMAGE_SLOT );
}

GXVoid EMMotionBlurMaterial::Unbind () const
{
	if ( !velocityTexture || !depthTexture || !imageTexture ) return;

	glUseProgram ( 0 );

	velocityTexture->Unbind ();
	depthTexture->Unbind ();
	imageTexture->Unbind ();
}

GXVoid EMMotionBlurMaterial::SetVelocityTexture ( GXTexture &texture )
{
	velocityTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetDepthTexture ( GXTexture &texture )
{
	depthTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetImageTexture ( GXTexture &texture )
{
	imageTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetBlurStrength ( GXFloat strength )
{
	blurStrength = strength;
}

GXVoid EMMotionBlurMaterial::SetBlurMinimumVelocity ( GXFloat velocity )
{
	blurMinimumVelocity = velocity;
}

GXVoid EMMotionBlurMaterial::SetBlurDepthFactorView ( GXFloat depthFactorView )
{
	blurDepthFactorView = depthFactorView;
}

GXVoid EMMotionBlurMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
	inverseScreenResolution.x = 1.0f / (GXFloat)width;
	inverseScreenResolution.y = 1.0f / (GXFloat)height;
}
