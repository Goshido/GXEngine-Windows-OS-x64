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

#define ANY_ALPHA							255

#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
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
	intensityLocation = shaderProgram.GetUniform ( "intensity" );
	hdrColorLocation = shaderProgram.GetUniform ( "hrdColor" );
	ambientColorLocation = shaderProgram.GetUniform ( "ambientColor" );
	toLightDirectionViewLocation = shaderProgram.GetUniform ( "toLightDirectionView" );
	inverseProjectionMatrixLocation = shaderProgram.GetUniform ( "inverseProjectionMatrix" );

	GXVec3 defaultLightDirectionView ( DEFAULT_LIGHT_DIRECTION_VIEW_X, DEFAULT_LIGHT_DIRECTION_VIEW_Y, DEFAULT_LIGHT_DIRECTION_VIEW_Z );
	SetLightDirectionView ( defaultLightDirectionView );

	intensity = DEFAULT_INTENCITY;
	SetHue ( DEFAULT_HUE_R, DEFAULT_HUE_G, DEFAULT_HUE_B );

	GXColorRGB defaultAmbientColor ( DEFAULT_AMBIENT_COLOR_R, DEFAULT_AMBIENT_COLOR_G, DEFAULT_AMBIENT_COLOR_B, ANY_ALPHA );
	SetAmbientColor ( defaultAmbientColor );
}

EMCookTorranceDirectedLightMaterial::~EMCookTorranceDirectedLightMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMCookTorranceDirectedLightMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture || !depthTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	const GXMat4& inverseProjectionMatrix = GXCamera::GetActiveCamera ()->GetCurrentFrameInverseProjectionMatrix ();
	glUniformMatrix4fv ( inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix.data );

	glUniform3fv ( hueLocation, 1, hue.data );
	glUniform1f ( intensityLocation, intensity );
	glUniform3fv ( hdrColorLocation, 1, hdrColor.data );
	glUniform3fv ( ambientColorLocation, 1, ambientColor.data );
	glUniform3fv ( toLightDirectionViewLocation, 1, toLightDirectionView.data );

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

GXVoid EMCookTorranceDirectedLightMaterial::SetAlbedoTexture ( GXTexture2D &texture )
{
	albedoTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetNormalTexture ( GXTexture2D &texture )
{
	normalTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetEmissionTexture ( GXTexture2D &texture )
{
	emissionTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetParameterTexture ( GXTexture2D &texture )
{
	parameterTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetDepthTexture ( GXTexture2D &texture )
{
	depthTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetLightDirectionView ( const GXVec3 &direction )
{
	toLightDirectionView = direction;
	toLightDirectionView.Reverse ();
}

GXVoid EMCookTorranceDirectedLightMaterial::SetHue ( GXUByte red, GXUByte green, GXUByte blue )
{
	hue.From ( red, green, blue, ANY_ALPHA );
	hdrColor.SetRed ( hue.GetRed () * intensity );
	hdrColor.SetGreen ( hue.GetGreen () * intensity );
	hdrColor.SetBlue ( hue.GetBlue () * intensity );
}

GXVoid EMCookTorranceDirectedLightMaterial::SetIntencity ( GXFloat intencity )
{
	this->intensity = intencity;
	hdrColor.SetRed ( hue.GetRed () * intensity );
	hdrColor.SetGreen ( hue.GetGreen () * intensity );
	hdrColor.SetBlue ( hue.GetBlue () * intensity );
}

GXVoid EMCookTorranceDirectedLightMaterial::SetAmbientColor ( const GXColorRGB &color )
{
	ambientColor = color;
}
