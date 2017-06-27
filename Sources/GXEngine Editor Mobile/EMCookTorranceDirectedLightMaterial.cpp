#include <GXEngine_Editor_Mobile/EMCookTorranceDirectedLightMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_LIGHT_DIRECTION_VIEW_X		0.0f
#define DEFAULT_LIGHT_DIRECTION_VIEW_Y		-1.0f
#define DEFAULT_LIGHT_DIRECTION_VIEW_Z		0.0f

#define DEFAULT_HUE_R						255
#define DEFAULT_HUE_G						255
#define DEFAULT_HUE_B						255

#define DEFAULT_INTENCITY					0.9f

#define DEFAULT_AMBIENT_COLOR_R				0.1f
#define DEFAULT_AMBIENT_COLOR_B				0.1f
#define DEFAULT_AMBIENT_COLOR_G				0.1f

#define ALBEDO_SLOT							0
#define NORMAL_SLOT							1
#define EMISSION_SLOT						2
#define PARAMETER_SLOT						3
#define DEPTH_SLOT							4

#define VERTEX_SHADER						L"Shaders/Editor Mobile/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/CookTorranceDirectedLight_fs.txt"


EMCookTorranceDirectedLightMaterial::EMCookTorranceDirectedLightMaterial ()
{
	albedoTexture = nullptr;
	normalTexture = nullptr;
	emissionTexture = nullptr;
	parameterTexture = nullptr;
	depthTexture = nullptr;

	static const GLchar* samplerNames[ 5 ] = { "albedoSampler", "normalSampler", "emissionSampler", "parameterSampler", "depthSampler" };
	static const GLuint samplerLocations[ 5 ] = { ALBEDO_SLOT, NORMAL_SLOT, EMISSION_SLOT, PARAMETER_SLOT, DEPTH_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 5;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	hueLocation = shaderProgram.GetUniform ( "hue" );
	intencityLocation = shaderProgram.GetUniform ( "intencity" );
	hdrColorLocation = shaderProgram.GetUniform ( "hrdColor" );
	ambientColorLocation = shaderProgram.GetUniform ( "ambientColor" );
	toLightDirectionViewLocation = shaderProgram.GetUniform ( "toLightDirectionView" );
	inverseProjectionMatrixLocation = shaderProgram.GetUniform ( "inverseProjectionMatrix" );

	GXVec3 defaultLightDirectionView ( DEFAULT_LIGHT_DIRECTION_VIEW_X, DEFAULT_LIGHT_DIRECTION_VIEW_Y, DEFAULT_LIGHT_DIRECTION_VIEW_Z );
	SetLightDirectionView ( defaultLightDirectionView );

	intencity = DEFAULT_INTENCITY;
	SetHue ( DEFAULT_HUE_R, DEFAULT_HUE_G, DEFAULT_HUE_B );

	GXVec3 defaultAmbientColor ( DEFAULT_AMBIENT_COLOR_R, DEFAULT_AMBIENT_COLOR_G, DEFAULT_AMBIENT_COLOR_B );
	SetAmbientColor ( defaultAmbientColor );
}

EMCookTorranceDirectedLightMaterial::~EMCookTorranceDirectedLightMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMCookTorranceDirectedLightMaterial::Bind ( const GXTransform &transform )
{
	if ( !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture || !depthTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	const GXMat4& inverseProjectionMatrix = GXCamera::GetActiveCamera ()->GetCurrentFrameInverseProjectionMatrix ();
	glUniformMatrix4fv ( inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix.arr );

	glUniform3fv ( hueLocation, 1, hue.arr );
	glUniform1f ( intencityLocation, intencity );
	glUniform3fv ( hdrColorLocation, 1, hdrColor.arr );
	glUniform3fv ( ambientColorLocation, 1, ambientColor.arr );
	glUniform3fv ( toLightDirectionViewLocation, 1, toLightDirectionView.arr );

	albedoTexture->Bind ( ALBEDO_SLOT );
	normalTexture->Bind ( NORMAL_SLOT );
	emissionTexture->Bind ( EMISSION_SLOT );
	parameterTexture->Bind ( PARAMETER_SLOT );
	depthTexture->Bind ( DEPTH_SLOT );
}

GXVoid EMCookTorranceDirectedLightMaterial::Unbind ()
{
	if ( !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture || !depthTexture ) return;

	glUseProgram ( 0 );

	albedoTexture->Unbind ();
	normalTexture->Unbind ();
	emissionTexture->Unbind ();
	parameterTexture->Unbind ();
	depthTexture->Unbind ();
}

GXVoid EMCookTorranceDirectedLightMaterial::SetAlbedoTexture ( GXTexture &texture )
{
	albedoTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetNormalTexture ( GXTexture &texture )
{
	normalTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetEmissionTexture ( GXTexture &texture )
{
	emissionTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetParameterTexture ( GXTexture &texture )
{
	parameterTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetDepthTexture ( GXTexture &texture )
{
	depthTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetLightDirectionView ( const GXVec3 &direction )
{
	toLightDirectionView.x = -direction.x;
	toLightDirectionView.y = -direction.y;
	toLightDirectionView.z = -direction.z;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetHue ( GXUByte red, GXUByte green, GXUByte blue )
{
	GXColorToVec3 ( hue, red, green, blue );
	GXMulVec3Scalar ( hdrColor, hue, intencity );
}

GXVoid EMCookTorranceDirectedLightMaterial::SetIntencity ( GXFloat intencity )
{
	this->intencity = intencity;
	GXMulVec3Scalar ( hdrColor, hue, intencity );
}

GXVoid EMCookTorranceDirectedLightMaterial::SetAmbientColor ( const GXVec3 &color )
{
	ambientColor = color;
}
