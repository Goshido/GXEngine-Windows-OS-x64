#include <GXEngine_Editor_Mobile/EMCookTorranceDirectedLightMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_LIGHT_DIRECTION_VIEW_X		0.0f
#define DEFAULT_LIGHT_DIRECTION_VIEW_Y		-1.0f
#define DEFAULT_LIGHT_DIRECTION_VIEW_Z		0.0f

#define DEFAULT_HUE_R						1.0f
#define DEFAULT_HUE_G						1.0f
#define DEFAULT_HUE_B						1.0f
#define DEFAULT_HUE_A						1.0f

#define DEFAULT_INTENSITY					0.9f

#define DEFAULT_AMBIENT_COLOR_R				0.1f
#define DEFAULT_AMBIENT_COLOR_B				0.1f
#define DEFAULT_AMBIENT_COLOR_G				0.1f
#define DEFAULT_AMBIENT_COLOR_A				1.0f

#define ALBEDO_SLOT							0u
#define NORMAL_SLOT							1u
#define EMISSION_SLOT						2u
#define PARAMETER_SLOT						3u
#define DEPTH_SLOT							4u

#define ANY_ALPHA							255u

#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/CookTorranceDirectedLight_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMCookTorranceDirectedLightMaterial::EMCookTorranceDirectedLightMaterial ():
	albedoTexture ( nullptr ),
	normalTexture ( nullptr ),
	emissionTexture ( nullptr ),
	parameterTexture ( nullptr ),
	depthTexture ( nullptr ),
	hue ( DEFAULT_HUE_R, DEFAULT_HUE_G, DEFAULT_HUE_B, DEFAULT_HUE_A ),
	intensity ( DEFAULT_INTENSITY ),
	hdrColor ( DEFAULT_HUE_R * DEFAULT_INTENSITY, DEFAULT_HUE_G * DEFAULT_INTENSITY, DEFAULT_HUE_B * DEFAULT_INTENSITY, DEFAULT_HUE_A ),
	ambientColor ( DEFAULT_AMBIENT_COLOR_R, DEFAULT_AMBIENT_COLOR_B, DEFAULT_AMBIENT_COLOR_G, DEFAULT_AMBIENT_COLOR_A ),
	toLightDirectionView ( -DEFAULT_LIGHT_DIRECTION_VIEW_X, -DEFAULT_LIGHT_DIRECTION_VIEW_Y, -DEFAULT_LIGHT_DIRECTION_VIEW_Z )
{
	static const GLchar* samplerNames[ 5 ] = { "albedoSampler", "normalSampler", "emissionSampler", "parameterSampler", "depthSampler" };
	static const GLuint samplerLocations[ 5 ] = { ALBEDO_SLOT, NORMAL_SLOT, EMISSION_SLOT, PARAMETER_SLOT, DEPTH_SLOT };

	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 5u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	hueLocation = shaderProgram.GetUniform ( "hue" );
	intensityLocation = shaderProgram.GetUniform ( "intensity" );
	hdrColorLocation = shaderProgram.GetUniform ( "hrdColor" );
	ambientColorLocation = shaderProgram.GetUniform ( "ambientColor" );
	toLightDirectionViewLocation = shaderProgram.GetUniform ( "toLightDirectionView" );
	inverseProjectionMatrixLocation = shaderProgram.GetUniform ( "inverseProjectionMatrix" );
}

EMCookTorranceDirectedLightMaterial::~EMCookTorranceDirectedLightMaterial ()
{
	// NOTHING
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

	glUseProgram ( 0u );

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
	hdrColor.data[ 0 ] = hue.data[ 0 ] * intensity;
	hdrColor.data[ 1 ] = hue.data[ 1 ] * intensity;
	hdrColor.data[ 2 ] = hue.data[ 2 ] * intensity;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetIntencity ( GXFloat intencity )
{
	this->intensity = intencity;
	hdrColor.data[ 0 ] = hue.data[ 0 ] * intensity;
	hdrColor.data[ 1 ] = hue.data[ 1 ] * intensity;
	hdrColor.data[ 2 ] = hue.data[ 2 ] * intensity;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetAmbientColor ( const GXColorRGB &color )
{
	ambientColor = color;
}
