#include <GXEngine_Editor_Mobile/EMMotionBlurMaterial.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_DEPTH_LIMIT				1.0f
#define DEFAULT_MAX_BLUR_SAMPLES		15u
#define DEFAULT_SCREEN_WIDTH			1280u
#define DEFAULT_SCREEN_HEIGHT			720u

#define VELOCITY_NEIGHBOR_MAX_SLOT		0u
#define VELOCITY_SLOT					1u
#define DEPTH_SLOT						2u
#define IMAGE_SLOT						3u

#define VERTEX_SHADER					L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER					nullptr
#define FRAGMENT_SHADER					L"Shaders/Editor Mobile/MotionBlur_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMMotionBlurMaterial::EMMotionBlurMaterial ():
	velocityNeighborMaxTexture ( nullptr ),
	velocityTexture ( nullptr ),
	depthTexture ( nullptr ),
	imageTexture ( nullptr ),
	depthLimit ( DEFAULT_DEPTH_LIMIT ),
	inverseDepthLimit ( 1.0f / DEFAULT_DEPTH_LIMIT ),
	maxBlurSamples ( static_cast<GXFloat> ( DEFAULT_MAX_BLUR_SAMPLES ) ),
	inverseScreenResolution ( 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_WIDTH ), 1.0f / static_cast<GXFloat> ( DEFAULT_SCREEN_WIDTH ) )
{
	static const GLchar* samplerNames[ 4 ] = { "velocityNeighborMaxSampler", "velocitySampler", "depthSampler", "imageSampler" };
	static const GLuint samplerLocations[ 4 ] = { VELOCITY_NEIGHBOR_MAX_SLOT, VELOCITY_SLOT, DEPTH_SLOT, IMAGE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 4u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	inverseDepthLimitLocation = shaderProgram.GetUniform ( "inverseDepthLimit" );
	maxBlurSamplesLocation = shaderProgram.GetUniform ( "maxBlurSamples" );
	inverseScreenResolutionLocation = shaderProgram.GetUniform ( "inverseScreenResolution" );
	inverseProjectionMatrixLocation = shaderProgram.GetUniform ( "inverseProjectionMatrix" );
}

EMMotionBlurMaterial::~EMMotionBlurMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMMotionBlurMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !velocityNeighborMaxTexture || !velocityTexture || !depthTexture || !imageTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	const GXMat4& inverseProjectionMatrix = GXCamera::GetActiveCamera ()->GetCurrentFrameInverseProjectionMatrix ();

	glUniform1f ( inverseDepthLimitLocation, inverseDepthLimit );
	glUniform1f ( maxBlurSamplesLocation, maxBlurSamples );
	glUniform2fv ( inverseScreenResolutionLocation, 1, inverseScreenResolution.data );
	glUniformMatrix4fv ( inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix.data );

	velocityNeighborMaxTexture->Bind ( VELOCITY_NEIGHBOR_MAX_SLOT );
	velocityTexture->Bind ( VELOCITY_SLOT );
	depthTexture->Bind ( DEPTH_SLOT );
	imageTexture->Bind ( IMAGE_SLOT );
}

GXVoid EMMotionBlurMaterial::Unbind ()
{
	if ( !velocityNeighborMaxTexture || !velocityTexture || !depthTexture || !imageTexture ) return;

	glUseProgram ( 0u );

	velocityNeighborMaxTexture->Unbind ();
	velocityTexture->Unbind ();
	depthTexture->Unbind ();
	imageTexture->Unbind ();
}

GXVoid EMMotionBlurMaterial::SetVelocityNeighborMaxTexture ( GXTexture2D &texture )
{
	velocityNeighborMaxTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetVelocityTexture ( GXTexture2D &texture )
{
	velocityTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetDepthTexture ( GXTexture2D &texture )
{
	depthTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetImageTexture ( GXTexture2D &texture )
{
	imageTexture = &texture;
}

GXVoid EMMotionBlurMaterial::SetDepthLimit ( GXFloat meters )
{
	if ( meters <= 0.0f )
	{
		GXLogW ( L"EMMotionBlurMaterial::SetDepthLimit::Error - ������� �� ������������� ������� �� �������!\n" );
		return;
	}

	depthLimit = meters;
	inverseDepthLimit = 1.0f / meters;
}

GXFloat EMMotionBlurMaterial::GetDepthLimit () const
{
	return depthLimit;
}

GXVoid EMMotionBlurMaterial::SetMaxBlurSamples ( GXUByte samples )
{
	if ( samples == 0u )
	{
		GXLogW ( L"EMMotionBlurMaterial::SetMaxBlurSamples::Error - ������� ������� ������������ ���������� �������!\n" );
		return;
	}

	maxBlurSamples = static_cast<GXFloat> ( samples );
}

GXUByte EMMotionBlurMaterial::GetMaxBlurSamples () const
{
	return (GXUByte)maxBlurSamples;
}

GXVoid EMMotionBlurMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
	inverseScreenResolution.data[ 0 ] = 1.0f / static_cast<GXFloat> ( width );
	inverseScreenResolution.data[ 1 ] = 1.0f / static_cast<GXFloat> ( height );
}
