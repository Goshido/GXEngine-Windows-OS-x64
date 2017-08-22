#include <GXEngine_Editor_Mobile/EMCookTorranceCommonPassMaterial.h>
#include <GXEngine/GXCamera.h>


#define ALBEDO_SLOT							0

#define DEFAULT_ALBEDO_TEXTURE_SCALE_U		1.0f
#define DEFAULT_ALBEDO_TEXTURE_SCALE_V		1.0f

#define DEFAULT_ALBEDO_TEXTURE_OFFSET_U		0.0f
#define DEFAULT_ALBEDO_TEXTURE_OFFSET_V		0.0f

#define DEFAULT_ALBEDO_COLOR_R				255
#define DEFAULT_ALBEDO_COLOR_G				255
#define DEFAULT_ALBEDO_COLOR_B				255
#define DEFAULT_ALBEDO_COLOR_A				255

#define NORMAL_SLOT							1

#define DEFAULT_NORMAL_TEXTURE_SCALE_U		1.0f
#define DEFAULT_NORMAL_TEXTURE_SCALE_V		1.0f

#define DEFAULT_NORMAL_TEXTURE_OFFSET_U		0.0f
#define DEFAULT_NORMAL_TEXTURE_OFFSET_V		0.0f

#define EMISSION_SLOT						2

#define DEFAULT_EMISSION_TEXTURE_SCALE_U	1.0f
#define DEFAULT_EMISSION_TEXTURE_SCALE_V	1.0f

#define DEFAULT_EMISSION_TEXTURE_OFFSET_U	0.0f
#define DEFAULT_EMISSION_TEXTURE_OFFSET_V	0.0f

#define DEFAULT_EMISSION_COLOR_R			255
#define DEFAULT_EMISSION_COLOR_G			255
#define DEFAULT_EMISSION_COLOR_B			255

#define DEFAULT_EMISSION_SCALE				1.0f

#define PARAMETER_SLOT						3

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

#define DEFAULT_SCREEN_RESOLUTION_WIDTH		1280
#define DEFAULT_SCREEN_RESOLUTION_HEIGHT	720

#define DEFAULT_MAXIMUM_BLUR_SAMPLES		15

#define VERTEX_SHADER			L"Shaders/Editor Mobile/CookTorranceCommonPass_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/Editor Mobile/CookTorranceCommonPass_fs.txt"


EMCookTorranceCommonPassMaterial::EMCookTorranceCommonPassMaterial ()
{
	albedoTexture = nullptr;
	normalTexture = nullptr;
	emissionTexture = nullptr;
	parameterTexture = nullptr;

	static const GLchar* samplerNames[ 4 ] = { "albedoSampler", "normalSampler", "emissionSampler", "parameterSampler" };
	static const GLuint samplerLocations[ 4 ] = { ALBEDO_SLOT, NORMAL_SLOT, EMISSION_SLOT, PARAMETER_SLOT };

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

	SetAlbedoTextureScale ( DEFAULT_ALBEDO_TEXTURE_SCALE_U, DEFAULT_ALBEDO_TEXTURE_SCALE_V );
	SetAlbedoTextureOffset ( DEFAULT_ALBEDO_TEXTURE_OFFSET_U, DEFAULT_ALBEDO_TEXTURE_OFFSET_V );
	SetAlbedoColor ( DEFAULT_ALBEDO_COLOR_R, DEFAULT_ALBEDO_COLOR_G, DEFAULT_ALBEDO_COLOR_B, DEFAULT_ALBEDO_COLOR_A );

	SetNormalTextureScale ( DEFAULT_NORMAL_TEXTURE_SCALE_U, DEFAULT_NORMAL_TEXTURE_SCALE_V );
	SetNormalTextureOffset ( DEFAULT_NORMAL_TEXTURE_OFFSET_U, DEFAULT_NORMAL_TEXTURE_OFFSET_V );

	SetEmissionTextureScale ( DEFAULT_EMISSION_TEXTURE_SCALE_U, DEFAULT_EMISSION_TEXTURE_SCALE_V );
	SetEmissionTextureOffset ( DEFAULT_EMISSION_TEXTURE_OFFSET_U, DEFAULT_EMISSION_TEXTURE_OFFSET_V );
	emissionColorScale = DEFAULT_EMISSION_SCALE;
	SetEmissionColor ( DEFAULT_EMISSION_COLOR_R, DEFAULT_EMISSION_COLOR_G, DEFAULT_EMISSION_COLOR_B );

	SetParameterTextureScale ( DEFAULT_PARAMETER_TEXTURE_SCALE_U, DEFAULT_PARAMETER_TEXTURE_SCALE_V );
	SetParameterTextureOffset ( DEFAULT_PARAMETER_TEXTURE_OFFSET_U, DEFAULT_PARAMETER_TEXTURE_OFFSET_V );
	SetRoughnessScale ( DEFAULT_ROUGHNESS_SCALE );
	SetIndexOfRefractionScale ( DEFAULT_INDEX_OF_REFRACTION_SCALE );
	SetSpecularIntensityScale ( DEFAULT_SPECULAR_INTECITY_SCALE );
	SetMetallicScale ( DEFAULT_METALLIC_SCALE );

	SetDeltaTime ( DEFAULT_DELTA_TIME );
	SetExposure ( DEFAULT_EXPLOSURE );
	SetScreenResolution ( DEFAULT_SCREEN_RESOLUTION_WIDTH, DEFAULT_SCREEN_RESOLUTION_HEIGHT );
	SetMaximumBlurSamples ( DEFAULT_MAXIMUM_BLUR_SAMPLES );
}

EMCookTorranceCommonPassMaterial::~EMCookTorranceCommonPassMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
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
	GXMulMat4Mat4 ( currentFrameModelViewProjectionMatrix, currentFrameModelMatrix, camera->GetCurrentFrameViewProjectionMatrix () );

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

	GXMulMat3Mat3 ( currentFrameRotationViewMatrix, r, v );

	GXMulMat4Mat4 ( lastFrameModelViewProjectionMatrix, transform.GetLastFrameModelMatrix (), camera->GetLastFrameViewProjectionMatrix () );

	glUniformMatrix4fv ( currentFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, currentFrameModelViewProjectionMatrix.arr );
	glUniformMatrix3fv ( currentFrameRotationViewMatrixLocation, 1, GL_FALSE, currentFrameRotationViewMatrix.arr );
	glUniformMatrix4fv ( lastFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, lastFrameModelViewProjectionMatrix.arr );

	glUniform1f ( inverseDeltaTimeLocation, inverseDeltaTime );
	glUniform1f ( exposureLocation, exposure );
	glUniform2fv ( screenResolutionLocation, 1, screenResolution.arr );
	glUniform1f ( maximumBlurSamplesLocation, maximumBlurSamples );
	glUniform1f ( inverseMaximumBlurSamplesLocation, inverseMaximumBlurSamples );

	albedoTexture->Bind ( ALBEDO_SLOT );
	glUniform4fv ( albedoTextureScaleOffsetLocation, 1, albedoTextureScaleOffset.arr );
	glUniform4fv ( albedoColorLocation, 1, albedoColor.arr );

	normalTexture->Bind ( NORMAL_SLOT );
	glUniform4fv ( normalTextureScaleOffsetLocation, 1, normalTextureScaleOffset.arr );

	emissionTexture->Bind ( EMISSION_SLOT );
	glUniform4fv ( emissionTextureScaleOffsetLocation, 1, emissionTextureScaleOffset.arr );
	glUniform3fv ( emissionScaledColorLocation, 1, emissionScaledColor.arr );

	parameterTexture->Bind ( PARAMETER_SLOT );
	glUniform4fv ( parameterTextureScaleOffsetLocation, 1, parameterTextureScaleOffset.arr );
	glUniform4fv ( parameterScaleLocation, 1, parameterScale.arr );
}

GXVoid EMCookTorranceCommonPassMaterial::Unbind ()
{
	if ( !albedoTexture || !normalTexture || !emissionTexture || !parameterTexture ) return;

	glUseProgram ( 0 );

	albedoTexture->Unbind ();
	normalTexture->Unbind ();
	emissionTexture->Unbind ();
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
	albedoTextureScaleOffset.x = scaleU;
	albedoTextureScaleOffset.y = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetAlbedoTextureScale ( GXVec2 &scaleUV ) const
{
	scaleUV.x = albedoTextureScaleOffset.x;
	scaleUV.y = albedoTextureScaleOffset.y;
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
	albedoTextureScaleOffset.z = offsetU;
	albedoTextureScaleOffset.w = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetAlbedoTextureOffset ( GXVec2 &offsetUV ) const
{
	offsetUV.x = albedoTextureScaleOffset.z;
	offsetUV.y = albedoTextureScaleOffset.w;
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXColorToVec4 ( albedoColor, red, green, blue, alpha );
}

GXVoid EMCookTorranceCommonPassMaterial::GetAlbedoColor ( GXUByte &red, GXUByte &green, GXUByte &blue, GXUByte &alpha ) const
{
	red = (GXUByte)( albedoColor.r * 255.0f );
	green = (GXUByte)( albedoColor.g * 255.0f );
	blue = (GXUByte)( albedoColor.b * 255.0f );
	alpha = (GXUByte)( albedoColor.a * 255.0f );
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
	normalTextureScaleOffset.x = scaleU;
	normalTextureScaleOffset.y = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetNormalTextureScale ( GXVec2 &scaleUV ) const
{
	scaleUV.x = normalTextureScaleOffset.x;
	scaleUV.y = normalTextureScaleOffset.y;
}

GXVoid EMCookTorranceCommonPassMaterial::SetNormalTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
	normalTextureScaleOffset.z = offsetU;
	normalTextureScaleOffset.w = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetNormalTextureOffset ( GXVec2 &offsetUV ) const
{
	offsetUV.x = normalTextureScaleOffset.z;
	offsetUV.y = normalTextureScaleOffset.w;
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
	emissionTextureScaleOffset.x = scaleU;
	emissionTextureScaleOffset.y = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetEmissionTextureScale ( GXVec2 &scaleUV ) const
{
	scaleUV.x = emissionTextureScaleOffset.x;
	scaleUV.y = emissionTextureScaleOffset.y;
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
	emissionTextureScaleOffset.z = offsetU;
	emissionTextureScaleOffset.w = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetEmissionTextureOffset ( GXVec2 &offsetUV ) const
{
	offsetUV.x = emissionTextureScaleOffset.z;
	offsetUV.y = emissionTextureScaleOffset.w;
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionColor ( GXUByte red, GXUByte green, GXUByte blue )
{
	GXColorToVec3 ( emissionColor, red, green, blue );
	GXMulVec3Scalar ( emissionScaledColor, emissionColor, emissionColorScale );
}

GXVoid EMCookTorranceCommonPassMaterial::GetEmissionColor ( GXUByte &red, GXUByte &green, GXUByte &blue ) const
{
	red = (GXUByte)( emissionColor.r * 255.0f );
	green = (GXUByte)( emissionColor.g * 255.0f );
	blue = (GXUByte)( emissionColor.b * 255.0f );
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionColorScale ( GXFloat scale )
{
	emissionColorScale = scale;
	GXMulVec3Scalar ( emissionScaledColor, emissionColor, emissionColorScale );
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
	parameterTextureScaleOffset.x = scaleU;
	parameterTextureScaleOffset.y = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetParameterTextureScale ( GXVec2 &scaleUV ) const
{
	scaleUV.x = parameterTextureScaleOffset.x;
	scaleUV.y = parameterTextureScaleOffset.y;
}

GXVoid EMCookTorranceCommonPassMaterial::SetParameterTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
	parameterTextureScaleOffset.z = offsetU;
	parameterTextureScaleOffset.w = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetParameterTextureOffset ( GXVec2 &offsetUV ) const
{
	offsetUV.x = parameterTextureScaleOffset.z;
	offsetUV.y = parameterTextureScaleOffset.w;
}

GXVoid EMCookTorranceCommonPassMaterial::SetRoughnessScale ( GXFloat scale )
{
	parameterScale.x = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetRoughnessScale () const
{
	return parameterScale.x;
}

GXVoid EMCookTorranceCommonPassMaterial::SetIndexOfRefractionScale ( GXFloat scale )
{
	parameterScale.y = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetIndexOfRefractionScale () const
{
	return parameterScale.y;
}

GXVoid EMCookTorranceCommonPassMaterial::SetSpecularIntensityScale ( GXFloat scale )
{
	parameterScale.z = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetSpecularIntensityScale () const
{
	return parameterScale.z;
}

GXVoid EMCookTorranceCommonPassMaterial::SetMetallicScale ( GXFloat scale )
{
	parameterScale.w = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetMetallicScale () const
{
	return parameterScale.w;
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
	screenResolution.x = (GXFloat)width;
	screenResolution.y = (GXFloat)height;
}

GXVoid EMCookTorranceCommonPassMaterial::SetMaximumBlurSamples ( GXUByte samples )
{
	maximumBlurSamples = (GXFloat)samples;
	inverseMaximumBlurSamples = 1.0f / maximumBlurSamples;
}
