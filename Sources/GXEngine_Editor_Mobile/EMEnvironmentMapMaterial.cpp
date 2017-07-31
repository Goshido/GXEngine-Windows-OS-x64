#include <GXEngine_Editor_Mobile/EMEnvironmentMapMaterial.h>
#include <GXEngine/GXCamera.h>


#define VERTEX_SHADER		L"Shaders/Editor Mobile/EnvironmentMap_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/EnvironmentMap_fs.txt"

#define ENVIRONMENT_SLOT	0
#define DEPTH_SLOT			1


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
}

EMEnvironmentMapMaterial::~EMEnvironmentMapMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMEnvironmentMapMaterial::Bind ( const GXTransform &transform )
{
	if ( !environmentTexture || !depthTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	GXMat4 modelViewProjectionMatrix;
	GXCamera* c = GXCamera::GetActiveCamera ();
	GXMulMat4Mat4 ( modelViewProjectionMatrix, transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );

	glUniformMatrix4fv ( modelViewProjectionMatrixLocation, 1, GL_FALSE, modelViewProjectionMatrix.arr );
	glUniform2fv ( inverseScreenResolutionLocation, 1, inverseScreenResolution.arr );

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
	inverseScreenResolution.x = 1.0f / (GXFloat)width;
	inverseScreenResolution.y = 1.0f / (GXFloat)height;
}
