#include <GXEngine_Editor_Mobile/EMMotionBlurMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_BLUR_MINIMUM_VELOCITY	5.0f
#define DEFAULT_DEPTH_LIMIT				0.1f
#define DEFAULT_MAX_BLUR_SAMPLES		15
#define DEFAULT_SCREEN_WIDTH			1280
#define DEFAULT_SCREEN_HEIGHT			720

#define VELOCITY_NEIGHBOR_MAX_SLOT		0
#define VELOCITY_SLOT					1
#define DEPTH_SLOT						2
#define IMAGE_SLOT						3

#define VERTEX_SHADER					L"Shaders/Editor Mobile/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER					nullptr
#define FRAGMENT_SHADER					L"Shaders/Editor Mobile/MotionBlur_fs.txt"


EMMotionBlurMaterial::EMMotionBlurMaterial ()
{
	velocityNeighborMaxTexture = nullptr;
	velocityTexture = nullptr;
	depthTexture = nullptr;
	imageTexture = nullptr;

	static const GLchar* samplerNames[ 4 ] = { "velocityNeighborMaxSampler", "velocitySampler", "depthSampler", "imageSampler" };
	static const GLuint samplerLocations[ 4 ] = { VELOCITY_NEIGHBOR_MAX_SLOT, VELOCITY_SLOT, DEPTH_SLOT, IMAGE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 4;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	blurMinimumvelocityLocation = shaderProgram.GetUniform ( "blurMinimumVelocity" );
	inverseDepthLimitLocation = shaderProgram.GetUniform ( "inverseDepthLimit" );
	maxBlurSamplesLocation = shaderProgram.GetUniform ( "maxBlurSamples" );
	inverseScreenResolutionLocation = shaderProgram.GetUniform ( "inverseScreenResolution" );
	inverseProjectionMatrixLocation = shaderProgram.GetUniform ( "inverseProjectionMatrix" );

	SetBlurMinimumVelocity ( DEFAULT_BLUR_MINIMUM_VELOCITY );
	SetDepthLimit ( DEFAULT_DEPTH_LIMIT );
	SetMaxBlurSamples ( DEFAULT_MAX_BLUR_SAMPLES );
	SetScreenResolution ( DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT );
}

EMMotionBlurMaterial::~EMMotionBlurMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMMotionBlurMaterial::Bind ( const GXTransform& /*transform*/ ) const
{
	if ( !velocityNeighborMaxTexture || !velocityTexture || !depthTexture || !imageTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	const GXMat4& inverseProjectionMatrix = GXCamera::GetActiveCamera ()->GetCurrentInverseProjectionMatrix ();

	glUniform1f ( blurMinimumvelocityLocation, blurMinimumVelocity );
	glUniform1f ( inverseDepthLimitLocation, inverseDepthLimit );
	glUniform1i ( maxBlurSamplesLocation, maxBlurSamples );
	glUniform2f ( inverseScreenResolutionLocation, inverseScreenResolution.x, inverseScreenResolution.y );
	glUniformMatrix4fv ( inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix.arr );

	velocityNeighborMaxTexture->Bind ( VELOCITY_NEIGHBOR_MAX_SLOT );
	velocityTexture->Bind ( VELOCITY_SLOT );
	depthTexture->Bind ( DEPTH_SLOT );
	imageTexture->Bind ( IMAGE_SLOT );
}

GXVoid EMMotionBlurMaterial::Unbind () const
{
	if ( !velocityNeighborMaxTexture || !velocityTexture || !depthTexture || !imageTexture ) return;

	glUseProgram ( 0 );

	velocityNeighborMaxTexture->Unbind ();
	velocityTexture->Unbind ();
	depthTexture->Unbind ();
	imageTexture->Unbind ();
}

GXVoid EMMotionBlurMaterial::SetVelocityNeighborMaxTexture ( GXTexture &texture )
{
	velocityNeighborMaxTexture = &texture;
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

GXVoid EMMotionBlurMaterial::SetBlurMinimumVelocity ( GXFloat velocity )
{
	blurMinimumVelocity = velocity;
}

GXVoid EMMotionBlurMaterial::SetDepthLimit ( GXFloat limit )
{
	inverseDepthLimit = 1.0f / limit;
}

GXVoid EMMotionBlurMaterial::SetMaxBlurSamples ( GXUByte maxSamples )
{
	maxBlurSamples = (GXInt)maxSamples;
}

GXVoid EMMotionBlurMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
	inverseScreenResolution.x = 1.0f / (GXFloat)width;
	inverseScreenResolution.y = 1.0f / (GXFloat)height;
}
