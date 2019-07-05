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
	sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
	hue ( DEFAULT_HUE_R, DEFAULT_HUE_G, DEFAULT_HUE_B, DEFAULT_HUE_A ),
	intensity ( DEFAULT_INTENSITY ),
	hdrColor ( DEFAULT_HUE_R * DEFAULT_INTENSITY, DEFAULT_HUE_G * DEFAULT_INTENSITY, DEFAULT_HUE_B * DEFAULT_INTENSITY, DEFAULT_HUE_A ),
	ambientColor ( DEFAULT_AMBIENT_COLOR_R, DEFAULT_AMBIENT_COLOR_B, DEFAULT_AMBIENT_COLOR_G, DEFAULT_AMBIENT_COLOR_A ),
	toLightDirectionView ( -DEFAULT_LIGHT_DIRECTION_VIEW_X, -DEFAULT_LIGHT_DIRECTION_VIEW_Y, -DEFAULT_LIGHT_DIRECTION_VIEW_Z )
{
	static const GLchar* samplerNames[ 5 ] = { "albedoSampler", "normalSampler", "emissionSampler", "parameterSampler", "depthSampler" };
	static const GLuint samplerLocations[ 5 ] = { ALBEDO_SLOT, NORMAL_SLOT, EMISSION_SLOT, PARAMETER_SLOT, DEPTH_SLOT };

	GXShaderProgramInfo si;
	si._vertexShader = VERTEX_SHADER;
	si._geometryShader = GEOMETRY_SHADER;
	si._fragmentShader = FRAGMENT_SHADER;
	si._samplers = 5u;
	si._samplerNames = samplerNames;
	si._samplerLocations = samplerLocations;
	si._transformFeedbackOutputs = 0;
	si._transformFeedbackOutputNames = nullptr;

	_shaderProgram.Init ( si );

	hueLocation = _shaderProgram.GetUniform ( "hue" );
	intensityLocation = _shaderProgram.GetUniform ( "intensity" );
	hdrColorLocation = _shaderProgram.GetUniform ( "hrdColor" );
	ambientColorLocation = _shaderProgram.GetUniform ( "ambientColor" );
	toLightDirectionViewLocation = _shaderProgram.GetUniform ( "toLightDirectionView" );
	inverseProjectionMatrixLocation = _shaderProgram.GetUniform ( "inverseProjectionMatrix" );
}

EMCookTorranceDirectedLightMaterial::~EMCookTorranceDirectedLightMaterial ()
{
	// NOTHING
}

GXVoid EMCookTorranceDirectedLightMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture || !depthTexture ) return;

	glUseProgram ( _shaderProgram.GetProgram () );

	const GXMat4& inverseProjectionMatrix = GXCamera::GetActiveCamera ()->GetCurrentFrameInverseProjectionMatrix ();
	glUniformMatrix4fv ( inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix._data );

	glUniform3fv ( hueLocation, 1, hue._data );
	glUniform1f ( intensityLocation, intensity );
	glUniform3fv ( hdrColorLocation, 1, hdrColor._data );
	glUniform3fv ( ambientColorLocation, 1, ambientColor._data );
	glUniform3fv ( toLightDirectionViewLocation, 1, toLightDirectionView._data );

	albedoTexture->Bind ( ALBEDO_SLOT );
	sampler.Bind ( ALBEDO_SLOT );

	normalTexture->Bind ( NORMAL_SLOT );
	sampler.Bind ( NORMAL_SLOT );

	emissionTexture->Bind ( EMISSION_SLOT );
	sampler.Bind ( EMISSION_SLOT );

	parameterTexture->Bind ( PARAMETER_SLOT );
	sampler.Bind ( PARAMETER_SLOT );

	depthTexture->Bind ( DEPTH_SLOT );
	sampler.Bind ( DEPTH_SLOT );
}

GXVoid EMCookTorranceDirectedLightMaterial::Unbind ()
{
	if ( !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture || !depthTexture ) return;

	sampler.Unbind ( ALBEDO_SLOT );
	albedoTexture->Unbind ();

	sampler.Unbind ( NORMAL_SLOT );
	normalTexture->Unbind ();

	sampler.Unbind ( EMISSION_SLOT );
	emissionTexture->Unbind ();

	sampler.Unbind ( PARAMETER_SLOT );
	parameterTexture->Unbind ();

	sampler.Unbind ( DEPTH_SLOT );
	depthTexture->Unbind ();

	glUseProgram ( 0u );
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
	hdrColor._data[ 0 ] = hue._data[ 0 ] * intensity;
	hdrColor._data[ 1 ] = hue._data[ 1 ] * intensity;
	hdrColor._data[ 2 ] = hue._data[ 2 ] * intensity;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetIntencity ( GXFloat intencity )
{
	this->intensity = intencity;
	hdrColor._data[ 0 ] = hue._data[ 0 ] * intensity;
	hdrColor._data[ 1 ] = hue._data[ 1 ] * intensity;
	hdrColor._data[ 2 ] = hue._data[ 2 ] * intensity;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetAmbientColor ( const GXColorRGB &color )
{
	ambientColor = color;
}
