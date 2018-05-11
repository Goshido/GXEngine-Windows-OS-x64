#include <GXEngine_Editor_Mobile/EMCookTorranceCommonPassMaterial.h>
#include <GXEngine/GXCamera.h>


#define ALBEDO_SLOT							0u

#define DEFAULT_ALBEDO_TEXTURE_SCALE_U		1.0f
#define DEFAULT_ALBEDO_TEXTURE_SCALE_V		1.0f

#define DEFAULT_ALBEDO_TEXTURE_OFFSET_U		0.0f
#define DEFAULT_ALBEDO_TEXTURE_OFFSET_V		0.0f

#define DEFAULT_ALBEDO_COLOR_R				255u
#define DEFAULT_ALBEDO_COLOR_G				255u
#define DEFAULT_ALBEDO_COLOR_B				255u
#define DEFAULT_ALBEDO_COLOR_A				255u

#define NORMAL_SLOT							1u

#define DEFAULT_NORMAL_TEXTURE_SCALE_U		1.0f
#define DEFAULT_NORMAL_TEXTURE_SCALE_V		1.0f

#define DEFAULT_NORMAL_TEXTURE_OFFSET_U		0.0f
#define DEFAULT_NORMAL_TEXTURE_OFFSET_V		0.0f

#define EMISSION_SLOT						2u

#define DEFAULT_EMISSION_TEXTURE_SCALE_U	1.0f
#define DEFAULT_EMISSION_TEXTURE_SCALE_V	1.0f

#define DEFAULT_EMISSION_TEXTURE_OFFSET_U	0.0f
#define DEFAULT_EMISSION_TEXTURE_OFFSET_V	0.0f

#define DEFAULT_EMISSION_COLOR_R			255u
#define DEFAULT_EMISSION_COLOR_G			255u
#define DEFAULT_EMISSION_COLOR_B			255u

#define DEFAULT_EMISSION_SCALE				1.0f

#define PARAMETER_SLOT						3u

#define DEFAULT_PARAMETER_TEXTURE_SCALE_U	1.0f
#define DEFAULT_PARAMETER_TEXTURE_SCALE_V	1.0f

#define DEFAULT_PARAMETER_TEXTURE_OFFSET_U	0.0f
#define DEFAULT_PARAMETER_TEXTURE_OFFSET_V	0.0f

#define DEFAULT_ROUGHNESS_SCALE				1.0f
#define DEFAULT_INDEX_OF_REFRACTION_SCALE	1.0f
#define DEFAULT_SPECULAR_INTECITY_SCALE		1.0f
#define DEFAULT_METALLIC_SCALE				1.0f

#define DEFAULT_DELTA_TIME					0.1667f
#define DEFAULT_EXPLOSURE					0.01667f

#define DEFAULT_SCREEN_RESOLUTION_WIDTH		1280.0f
#define DEFAULT_SCREEN_RESOLUTION_HEIGHT	720.0f

#define DEFAULT_MAXIMUM_BLUR_SAMPLES		15u

#define ANY_ALPHA							255u
#define UBYTE_COLOR_TO_FLOAT				0.0039215f

#define VERTEX_SHADER			L"Shaders/Editor Mobile/CookTorranceCommonPass_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/Editor Mobile/CookTorranceCommonPass_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMCookTorranceCommonPassMaterial::EMCookTorranceCommonPassMaterial ():
	albedoTexture ( nullptr ),
	normalTexture ( nullptr ),
	emissionTexture ( nullptr ),
	parameterTexture ( nullptr ),
	sampler ( GL_REPEAT, eGXResampling::Trilinear, 16.0f ),
	albedoColor ( static_cast<GXUByte> ( DEFAULT_ALBEDO_COLOR_R ), static_cast<GXUByte> ( DEFAULT_ALBEDO_COLOR_G ), static_cast<GXUByte> ( DEFAULT_ALBEDO_COLOR_B ), static_cast<GXUByte> ( DEFAULT_ALBEDO_COLOR_A ) ),
	albedoTextureScaleOffset ( DEFAULT_ALBEDO_TEXTURE_SCALE_U, DEFAULT_ALBEDO_TEXTURE_SCALE_V, DEFAULT_ALBEDO_TEXTURE_OFFSET_U, DEFAULT_ALBEDO_TEXTURE_OFFSET_V ),
	normalTextureScaleOffset ( DEFAULT_NORMAL_TEXTURE_SCALE_U, DEFAULT_NORMAL_TEXTURE_SCALE_V, DEFAULT_NORMAL_TEXTURE_OFFSET_U, DEFAULT_NORMAL_TEXTURE_OFFSET_V ),
	emissionColor ( static_cast<GXUByte> ( DEFAULT_EMISSION_COLOR_R ), static_cast<GXUByte> ( DEFAULT_EMISSION_COLOR_G ), static_cast<GXUByte> ( DEFAULT_EMISSION_COLOR_B ), static_cast<GXUByte> ( ANY_ALPHA ) ),
	emissionColorScale ( DEFAULT_EMISSION_SCALE ),
	emissionScaledColor ( DEFAULT_EMISSION_SCALE * DEFAULT_EMISSION_COLOR_R * UBYTE_COLOR_TO_FLOAT, DEFAULT_EMISSION_SCALE * DEFAULT_EMISSION_COLOR_G * UBYTE_COLOR_TO_FLOAT, DEFAULT_EMISSION_SCALE * DEFAULT_EMISSION_COLOR_B * UBYTE_COLOR_TO_FLOAT, ANY_ALPHA ),
	emissionTextureScaleOffset ( DEFAULT_EMISSION_TEXTURE_SCALE_U, DEFAULT_EMISSION_TEXTURE_SCALE_V, DEFAULT_EMISSION_TEXTURE_OFFSET_U, DEFAULT_EMISSION_TEXTURE_OFFSET_V ),
	parameterScale ( DEFAULT_ROUGHNESS_SCALE, DEFAULT_INDEX_OF_REFRACTION_SCALE, DEFAULT_SPECULAR_INTECITY_SCALE, DEFAULT_METALLIC_SCALE ),
	parameterTextureScaleOffset ( DEFAULT_PARAMETER_TEXTURE_SCALE_U, DEFAULT_PARAMETER_TEXTURE_SCALE_V, DEFAULT_PARAMETER_TEXTURE_OFFSET_U, DEFAULT_PARAMETER_TEXTURE_OFFSET_V ),
	inverseDeltaTime ( 1.0f / DEFAULT_DELTA_TIME ),
	exposure ( DEFAULT_EXPLOSURE ),
	screenResolution ( DEFAULT_SCREEN_RESOLUTION_WIDTH, DEFAULT_SCREEN_RESOLUTION_HEIGHT ),
	inverseMaximumBlurSamples ( 1.0f / static_cast<GXFloat> ( DEFAULT_MAXIMUM_BLUR_SAMPLES ) )
{
	static const GLchar* samplerNames[ 4 ] = { "albedoSampler", "normalSampler", "emissionSampler", "parameterSampler" };
	static const GLuint samplerLocations[ 4 ] = { ALBEDO_SLOT, NORMAL_SLOT, EMISSION_SLOT, PARAMETER_SLOT };

	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 4u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	albedoTextureScaleOffsetLocation = shaderProgram.GetUniform ( "albedoTextureScaleOffset" );
	albedoColorLocation = shaderProgram.GetUniform ( "albedoColor" );
	normalTextureScaleOffsetLocation = shaderProgram.GetUniform ( "normalTextureScaleOffset" );
	emissionTextureScaleOffsetLocation = shaderProgram.GetUniform ( "emissionTextureScaleOffset" );
	emissionScaledColorLocation = shaderProgram.GetUniform ( "emissionScaledColor" );
	parameterTextureScaleOffsetLocation = shaderProgram.GetUniform ( "parameterTextureScaleOffset" );
	parameterScaleLocation = shaderProgram.GetUniform ( "parameterScale" );
	inverseDeltaTimeLocation = shaderProgram.GetUniform ( "inverseDeltaTime" );
	exposureLocation = shaderProgram.GetUniform ( "exposure" );
	screenResolutionLocation = shaderProgram.GetUniform ( "screenResolution" );
	maximumBlurSamplesLocation = shaderProgram.GetUniform ( "maximumBlurSamples" );
	inverseMaximumBlurSamplesLocation = shaderProgram.GetUniform ( "inverseMaximumBlurSamples" );
	currentFrameModelViewProjectionMatrixLocation = shaderProgram.GetUniform ( "currentFrameModelViewProjectionMatrix" );
	currentFrameRotationViewMatrixLocation = shaderProgram.GetUniform ( "currentFrameRotationViewMatrix" );
	lastFrameModelViewProjectionMatrixLocation = shaderProgram.GetUniform ( "lastFrameModelViewProjectionMatrix" );
}

EMCookTorranceCommonPassMaterial::~EMCookTorranceCommonPassMaterial ()
{
	// NOTHING
}

GXVoid EMCookTorranceCommonPassMaterial::Bind ( const GXTransform &transform )
{
	if ( !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* camera = GXCamera::GetActiveCamera ();

	GXMat4 currentFrameModelViewProjectionMatrix;
	GXMat3 currentFrameRotationViewMatrix;
	GXMat4 lastFrameModelViewProjectionMatrix;

	const GXMat4& currentFrameModelMatrix = transform.GetCurrentFrameModelMatrix ();
	currentFrameModelViewProjectionMatrix.Multiply ( currentFrameModelMatrix, camera->GetCurrentFrameViewProjectionMatrix () );

	GXMat4 currentFrameRotationMatrix;
	transform.GetRotation ( currentFrameRotationMatrix );
	const GXMat4& currentFrameViewMatrix = camera->GetCurrentFrameViewMatrix ();

	GXVec3 tmp;
	GXMat3 r;
	currentFrameRotationMatrix.GetX ( tmp );
	r.SetX ( tmp );

	currentFrameRotationMatrix.GetY ( tmp );
	r.SetY ( tmp );

	currentFrameRotationMatrix.GetZ ( tmp );
	r.SetZ ( tmp );

	GXMat3 v;
	currentFrameViewMatrix.GetX ( tmp );
	v.SetX ( tmp );

	currentFrameViewMatrix.GetY ( tmp );
	v.SetY ( tmp );

	currentFrameViewMatrix.GetZ ( tmp );
	v.SetZ ( tmp );

	currentFrameRotationViewMatrix.Multiply ( r, v );
	lastFrameModelViewProjectionMatrix.Multiply ( transform.GetLastFrameModelMatrix (), camera->GetLastFrameViewProjectionMatrix () );

	glUniformMatrix4fv ( currentFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, currentFrameModelViewProjectionMatrix.data );
	glUniformMatrix3fv ( currentFrameRotationViewMatrixLocation, 1, GL_FALSE, currentFrameRotationViewMatrix.data );
	glUniformMatrix4fv ( lastFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, lastFrameModelViewProjectionMatrix.data );

	glUniform1f ( inverseDeltaTimeLocation, inverseDeltaTime );
	glUniform1f ( exposureLocation, exposure );
	glUniform2fv ( screenResolutionLocation, 1, screenResolution.data );
	glUniform1f ( maximumBlurSamplesLocation, maximumBlurSamples );
	glUniform1f ( inverseMaximumBlurSamplesLocation, inverseMaximumBlurSamples );

	albedoTexture->Bind ( ALBEDO_SLOT );
	sampler.Bind ( ALBEDO_SLOT );
	glUniform4fv ( albedoTextureScaleOffsetLocation, 1, albedoTextureScaleOffset.data );
	glUniform4fv ( albedoColorLocation, 1, albedoColor.data );

	normalTexture->Bind ( NORMAL_SLOT );
	sampler.Bind ( NORMAL_SLOT );
	glUniform4fv ( normalTextureScaleOffsetLocation, 1, normalTextureScaleOffset.data );

	emissionTexture->Bind ( EMISSION_SLOT );
	sampler.Bind ( EMISSION_SLOT );
	glUniform4fv ( emissionTextureScaleOffsetLocation, 1, emissionTextureScaleOffset.data );
	glUniform3fv ( emissionScaledColorLocation, 1, emissionScaledColor.data );

	parameterTexture->Bind ( PARAMETER_SLOT );
	sampler.Bind ( PARAMETER_SLOT );
	glUniform4fv ( parameterTextureScaleOffsetLocation, 1, parameterTextureScaleOffset.data );
	glUniform4fv ( parameterScaleLocation, 1, parameterScale.data );
}

GXVoid EMCookTorranceCommonPassMaterial::Unbind ()
{
	if ( !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture ) return;

	glUseProgram ( 0u );

	sampler.Unbind ( ALBEDO_SLOT );
	albedoTexture->Unbind ();

	sampler.Unbind ( NORMAL_SLOT );
	normalTexture->Unbind ();

	sampler.Unbind ( EMISSION_SLOT );
	emissionTexture->Unbind ();

	sampler.Unbind ( PARAMETER_SLOT );
	parameterTexture->Unbind ();
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoTexture ( GXTexture2D &texture )
{
	albedoTexture = &texture;
}

GXTexture2D* EMCookTorranceCommonPassMaterial::GetAlbedoTexture ()
{
	return albedoTexture;
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoTextureScale ( GXFloat scaleU, GXFloat scaleV )
{
	albedoTextureScaleOffset.data[ 0 ] = scaleU;
	albedoTextureScaleOffset.data[ 1 ] = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetAlbedoTextureScale ( GXVec2 &scaleUV ) const
{
	scaleUV.Init ( albedoTextureScaleOffset.data[ 0 ], albedoTextureScaleOffset.data[ 1 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
	albedoTextureScaleOffset.data[ 2 ] = offsetU;
	albedoTextureScaleOffset.data[ 3 ] = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetAlbedoTextureOffset ( GXVec2 &offsetUV ) const
{
	offsetUV.Init ( albedoTextureScaleOffset.data[ 2 ], albedoTextureScaleOffset.data[ 3 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	albedoColor.From ( red, green, blue, alpha );
}

GXVoid EMCookTorranceCommonPassMaterial::GetAlbedoColor ( GXUByte &red, GXUByte &green, GXUByte &blue, GXUByte &alpha ) const
{
	albedoColor.ConvertToUByte ( red, green, blue, alpha );
}

GXVoid EMCookTorranceCommonPassMaterial::SetNormalTexture ( GXTexture2D &texture )
{
	normalTexture = &texture;
}

GXTexture2D* EMCookTorranceCommonPassMaterial::GetNormalTexture ()
{
	return normalTexture;
}

GXVoid EMCookTorranceCommonPassMaterial::SetNormalTextureScale ( GXFloat scaleU, GXFloat scaleV )
{
	normalTextureScaleOffset.data[ 0 ] = scaleU;
	normalTextureScaleOffset.data[ 1 ] = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetNormalTextureScale ( GXVec2 &scaleUV ) const
{
	scaleUV.Init ( normalTextureScaleOffset.data[ 0 ], normalTextureScaleOffset.data[ 1 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetNormalTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
	normalTextureScaleOffset.data[ 2 ] = offsetU;
	normalTextureScaleOffset.data[ 3 ] = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetNormalTextureOffset ( GXVec2 &offsetUV ) const
{
	offsetUV.Init ( normalTextureScaleOffset.data[ 2 ], normalTextureScaleOffset.data[ 3 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionTexture ( GXTexture2D &texture )
{
	emissionTexture = &texture;
}

GXTexture2D* EMCookTorranceCommonPassMaterial::GetEmissionTexture ()
{
	return emissionTexture;
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionTextureScale ( GXFloat scaleU, GXFloat scaleV )
{
	emissionTextureScaleOffset.data[ 0 ] = scaleU;
	emissionTextureScaleOffset.data[ 1 ] = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetEmissionTextureScale ( GXVec2 &scaleUV ) const
{
	scaleUV.Init ( emissionTextureScaleOffset.data[ 0 ], emissionTextureScaleOffset.data[ 1 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
	emissionTextureScaleOffset.data[ 2 ] = offsetU;
	emissionTextureScaleOffset.data[ 3 ] = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetEmissionTextureOffset ( GXVec2 &offsetUV ) const
{
	offsetUV.Init ( emissionTextureScaleOffset.data[ 2 ], emissionTextureScaleOffset.data[ 3 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionColor ( GXUByte red, GXUByte green, GXUByte blue )
{
	emissionColor.From ( red, green, blue, ANY_ALPHA );
	emissionScaledColor.SetRed ( emissionColor.GetRed () * emissionColorScale );
	emissionScaledColor.SetGreen ( emissionColor.GetGreen () * emissionColorScale );
	emissionScaledColor.SetBlue ( emissionColor.GetBlue () * emissionColorScale );
}

GXVoid EMCookTorranceCommonPassMaterial::GetEmissionColor ( GXUByte &red, GXUByte &green, GXUByte &blue ) const
{
	GXUByte anyAlpha;
	emissionColor.ConvertToUByte ( red, green, blue, anyAlpha );
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionColorScale ( GXFloat scale )
{
	emissionColorScale = scale;
	emissionScaledColor.SetRed ( emissionColor.GetRed () * emissionColorScale );
	emissionScaledColor.SetGreen ( emissionColor.GetGreen () * emissionColorScale );
	emissionScaledColor.SetBlue ( emissionColor.GetBlue () * emissionColorScale );
}

GXFloat EMCookTorranceCommonPassMaterial::GetEmissionColorScale () const
{
	return emissionColorScale;
}

GXVoid EMCookTorranceCommonPassMaterial::SetParameterTexture ( GXTexture2D &texture )
{
	parameterTexture = &texture;
}

GXTexture2D* EMCookTorranceCommonPassMaterial::GetParameterTexture ()
{
	return parameterTexture;
}

GXVoid EMCookTorranceCommonPassMaterial::SetParameterTextureScale ( GXFloat scaleU, GXFloat scaleV )
{
	parameterTextureScaleOffset.data[ 0 ] = scaleU;
	parameterTextureScaleOffset.data[ 1 ] = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetParameterTextureScale ( GXVec2 &scaleUV ) const
{
	scaleUV.Init ( parameterTextureScaleOffset.data[ 0 ], parameterTextureScaleOffset.data[ 1 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetParameterTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
	parameterTextureScaleOffset.data[ 2 ] = offsetU;
	parameterTextureScaleOffset.data[ 3 ] = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetParameterTextureOffset ( GXVec2 &offsetUV ) const
{
	offsetUV.Init ( parameterTextureScaleOffset.data[ 2 ], parameterTextureScaleOffset.data[ 3 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetRoughnessScale ( GXFloat scale )
{
	parameterScale.data[ 0 ] = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetRoughnessScale () const
{
	return parameterScale.data[ 0 ];
}

GXVoid EMCookTorranceCommonPassMaterial::SetIndexOfRefractionScale ( GXFloat scale )
{
	parameterScale.data[ 1 ] = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetIndexOfRefractionScale () const
{
	return parameterScale.data[ 1 ];
}

GXVoid EMCookTorranceCommonPassMaterial::SetSpecularIntensityScale ( GXFloat scale )
{
	parameterScale.data[ 2 ] = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetSpecularIntensityScale () const
{
	return parameterScale.data[ 2 ];
}

GXVoid EMCookTorranceCommonPassMaterial::SetMetallicScale ( GXFloat scale )
{
	parameterScale.data[ 3 ] = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetMetallicScale () const
{
	return parameterScale.data[ 3 ];
}

GXVoid EMCookTorranceCommonPassMaterial::SetDeltaTime ( GXFloat seconds )
{
	inverseDeltaTime = 1.0f / seconds;
}

GXVoid EMCookTorranceCommonPassMaterial::SetExposure ( GXFloat seconds )
{
	exposure = seconds;
}

GXVoid EMCookTorranceCommonPassMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
	screenResolution.Init ( static_cast<GXFloat> ( width ), static_cast<GXFloat> ( height ) );
}

GXVoid EMCookTorranceCommonPassMaterial::SetMaximumBlurSamples ( GXUByte samples )
{
	maximumBlurSamples = static_cast<GXFloat> ( samples );
	inverseMaximumBlurSamples = 1.0f / maximumBlurSamples;
}
