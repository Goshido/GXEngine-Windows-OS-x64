#include <GXEngine_Editor_Mobile/EMBlinnPhongCommonPassMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_DIFFUSE_COLOR_R				255
#define DEFAULT_DIFFUSE_COLOR_G				255
#define DEFAULT_DIFFUSE_COLOR_B				255
#define DEFAULT_DIFFUSE_COLOR_A				255

#define DEFAULT_DIFFUSE_TEXTURE_SCALE_X		1.0f
#define DEFAULT_DIFFUSE_TEXTURE_SCALE_Y		1.0f
#define DEFAULT_DIFFUSE_TEXTURE_OFFSET_X	0.0f
#define DEFAULT_DIFFUSE_TEXTURE_OFFSET_Y	0.0f

#define DIFFUSE_SLOT						0

#define DEFAULT_NORMAL_TEXTURE_SCALE_X		1.0f
#define DEFAULT_NORMAL_TEXTURE_SCALE_Y		1.0f
#define DEFAULT_NORMAL_TEXTURE_OFFSET_X		0.0f
#define DEFAULT_NORMAL_TEXTURE_OFFSET_Y		0.0f

#define NORMAL_SLOT							1

#define DEFAULT_SPECULAR_TEXTURE_SCALE_X	1.0f
#define DEFAULT_SPECULAR_TEXTURE_SCALE_Y	1.0f
#define DEFAULT_SPECULAR_TEXTURE_OFFSET_X	0.0f
#define DEFAULT_SPECULAR_TEXTURE_OFFSET_Y	0.0f

#define SPECULAR_SLOT						2

#define DEFAULT_EMISSION_COLOR_R			255
#define DEFAULT_EMISSION_COLOR_G			255
#define DEFAULT_EMISSION_COLOR_B			255

#define DEFAULT_EMISSION_TEXTURE_SCALE_X	1.0f
#define DEFAULT_EMISSION_TEXTURE_SCALE_Y	1.0f
#define DEFAULT_EMISSION_TEXTURE_OFFSET_X	0.0f
#define DEFAULT_EMISSION_TEXTURE_OFFSET_Y	0.0f

#define EMISSION_SLOT						3

#define DEFAULT_DELTA_TIME					0.1667f
#define DEFAULT_EXPLOSURE_TIME				0.01667f

#define DEFAULT_SCREEN_RESOLUTION_WIDTH		1280
#define DEFAULT_SCREEN_RESOLUTION_HEIGHT	720

#define DEFAULT_MAX_BLUR_SAMPLES			15

#define VERTEX_SHADER						L"Shaders/Editor Mobile/BlinnPhongCommonPass_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/BlinnPhongCommonPass_fs.txt"


EMBlinnPhongCommonPassMaterial::EMBlinnPhongCommonPassMaterial ()
{
	static const GLchar* samplerNames[ 4 ] = { "diffuseSampler", "normalSampler", "specularSampler", "emissionSampler" };
	static const GLuint samplerLocations[ 4 ] = { DIFFUSE_SLOT, NORMAL_SLOT, SPECULAR_SLOT, EMISSION_SLOT };

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

	diffuseTextureScaleOffsetLocation = shaderProgram.GetUniform ( "diffuseTextureScaleOffset" );
	diffuseColorLocation = shaderProgram.GetUniform ( "diffuseColor" );
	normalTextureScaleOffsetLocation = shaderProgram.GetUniform ( "normalTextureScaleOffset" );
	specularTextureScaleOffsetLocation = shaderProgram.GetUniform ( "specularTextureScaleOffset" );
	emissionTextureScaleOffsetLocation = shaderProgram.GetUniform ( "emissionTextureScaleOffset" );
	emissionColorLocation = shaderProgram.GetUniform ( "emissionColor" );
	currentFrameModelViewProjectionMatrixLocation = shaderProgram.GetUniform ( "currentFrameModelViewProjectionMatrix" );
	currentFrameRotationViewMatrixLocation = shaderProgram.GetUniform ( "currentFrameRotationViewMatrix" );
	lastFrameModelViewProjectionMatrixLocation = shaderProgram.GetUniform ( "lastFrameModelViewProjectionMatrix" );
	inverseDeltaTimeLocation = shaderProgram.GetUniform ( "inverseDeltaTime" );
	explosureTimeLocation = shaderProgram.GetUniform ( "explosureTime" );
	screenResolutionLocation = shaderProgram.GetUniform ( "screenResolution" );
	maxBlurSamplesLocation = shaderProgram.GetUniform ( "maxBlurSamples" );

	diffuseTexture = nullptr;
	SetDiffuseTextureColor ( DEFAULT_DIFFUSE_COLOR_R, DEFAULT_DIFFUSE_COLOR_G, DEFAULT_DIFFUSE_COLOR_B, DEFAULT_DIFFUSE_COLOR_A );
	SetDiffuseTextureScale ( DEFAULT_DIFFUSE_TEXTURE_SCALE_X, DEFAULT_DIFFUSE_TEXTURE_SCALE_Y );
	SetDiffuseTextureOffset ( DEFAULT_DIFFUSE_TEXTURE_OFFSET_X, DEFAULT_DIFFUSE_TEXTURE_OFFSET_Y );

	normalTexture = nullptr;
	SetNormalTextureScale ( DEFAULT_NORMAL_TEXTURE_SCALE_X, DEFAULT_NORMAL_TEXTURE_SCALE_Y );
	SetNormalTextureOffset ( DEFAULT_NORMAL_TEXTURE_OFFSET_X, DEFAULT_NORMAL_TEXTURE_OFFSET_Y );

	specularTexture = nullptr;
	SetSpecularTextureScale ( DEFAULT_SPECULAR_TEXTURE_SCALE_X, DEFAULT_SPECULAR_TEXTURE_SCALE_Y );
	SetSpecularTextureOffset ( DEFAULT_SPECULAR_TEXTURE_OFFSET_X, DEFAULT_SPECULAR_TEXTURE_OFFSET_Y );

	emissionTexture = nullptr;
	SetEmissionTextureColor ( DEFAULT_EMISSION_COLOR_R, DEFAULT_EMISSION_COLOR_G, DEFAULT_EMISSION_COLOR_B );
	SetEmissionTextureScale ( DEFAULT_EMISSION_TEXTURE_SCALE_X, DEFAULT_EMISSION_TEXTURE_SCALE_Y );
	SetEmissionTextureOffset ( DEFAULT_EMISSION_TEXTURE_OFFSET_X, DEFAULT_EMISSION_TEXTURE_OFFSET_Y );

	SetDeltaTime ( DEFAULT_DELTA_TIME );

	SetExplosureTime ( DEFAULT_EXPLOSURE_TIME );
	SetScreenResolution ( DEFAULT_SCREEN_RESOLUTION_WIDTH, DEFAULT_SCREEN_RESOLUTION_HEIGHT );
	SetMaxBlurSamples ( DEFAULT_MAX_BLUR_SAMPLES );
}

EMBlinnPhongCommonPassMaterial::~EMBlinnPhongCommonPassMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMBlinnPhongCommonPassMaterial::Bind ( const GXTransform &transform )
{
	if ( !diffuseTexture || !normalTexture || !specularTexture || !emissionTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* camera = GXCamera::GetActiveCamera ();

	GXMat4 currentFrameModelViewProjectionMatrix;
	GXMat3 currentFrameRotationViewMatrix;
	GXMat4 lastFrameModelViewProjectionMatrix;

	const GXMat4& currentFrameModelMatrix = transform.GetCurrentModelMatrix ();
	GXMulMat4Mat4 ( currentFrameModelViewProjectionMatrix, currentFrameModelMatrix, camera->GetCurrentFrameViewProjectionMatrix () );

	GXMat4 currentFrameRotationMatrix;
	transform.GetRotation ( currentFrameRotationMatrix );
	const GXMat4& currentFrameViewMatrix = camera->GetCurrentFrameViewMatrix ();

	GXMat3 r;
	r.xv = currentFrameRotationMatrix.xv;
	r.yv = currentFrameRotationMatrix.yv;
	r.zv = currentFrameRotationMatrix.zv;

	GXMat3 v;
	v.xv = currentFrameViewMatrix.xv;
	v.yv = currentFrameViewMatrix.yv;
	v.zv = currentFrameViewMatrix.zv;

	GXMulMat3Mat3 ( currentFrameRotationViewMatrix, r, v );

	GXMulMat4Mat4 ( lastFrameModelViewProjectionMatrix, transform.GetLastFrameModelMatrix (), camera->GetLastFrameViewProjectionMatrix () );

	glUniformMatrix4fv ( currentFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, currentFrameModelViewProjectionMatrix.arr );
	glUniformMatrix3fv ( currentFrameRotationViewMatrixLocation, 1, GL_FALSE, currentFrameRotationViewMatrix.arr );
	glUniformMatrix4fv ( lastFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, lastFrameModelViewProjectionMatrix.arr );

	glUniform1f ( inverseDeltaTimeLocation, inverseDeltaTime );
	glUniform1f ( explosureTimeLocation, explosureTime );
	glUniform2fv ( screenResolutionLocation, 1, screenResolution.arr );
	glUniform1f ( maxBlurSamplesLocation, maxBlurSamples );

	diffuseTexture->Bind ( DIFFUSE_SLOT );
	glUniform4fv ( diffuseColorLocation, 1, diffuseColor.arr );
	glUniform4fv ( diffuseTextureScaleOffsetLocation, 1, diffuseTextureScaleOffset.arr );

	normalTexture->Bind ( NORMAL_SLOT );
	glUniform4fv ( normalTextureScaleOffsetLocation, 1, normalTextureScaleOffset.arr );

	specularTexture->Bind ( SPECULAR_SLOT );
	glUniform4fv ( specularTextureScaleOffsetLocation, 1, specularTextureScaleOffset.arr );

	emissionTexture->Bind ( EMISSION_SLOT );
	glUniform3fv ( emissionColorLocation, 1, emissionColor.arr );
	glUniform4fv ( emissionTextureScaleOffsetLocation, 1, emissionTextureScaleOffset.arr );
}

GXVoid EMBlinnPhongCommonPassMaterial::Unbind ()
{
	if ( !diffuseTexture || !normalTexture || !specularTexture || !emissionTexture ) return;

	diffuseTexture->Unbind ();
	normalTexture->Unbind ();
	specularTexture->Unbind ();
	emissionTexture->Unbind ();

	glUseProgram ( 0 );
}

GXVoid EMBlinnPhongCommonPassMaterial::SetDiffuseTexture ( GXTexture &texture )
{
	diffuseTexture = &texture;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetDiffuseTextureScale ( GXFloat x, GXFloat y )
{
	diffuseTextureScaleOffset.x = x;
	diffuseTextureScaleOffset.y = y;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetDiffuseTextureOffset ( GXFloat x, GXFloat y )
{
	diffuseTextureScaleOffset.z = x;
	diffuseTextureScaleOffset.w = y;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetDiffuseTextureColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXColorToVec4 ( diffuseColor, red, green, blue, alpha );
}

GXVoid EMBlinnPhongCommonPassMaterial::SetNormalTexture ( GXTexture &texture )
{
	normalTexture = &texture;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetNormalTextureScale ( GXFloat x, GXFloat y )
{
	normalTextureScaleOffset.x = x;
	normalTextureScaleOffset.y = y;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetNormalTextureOffset ( GXFloat x, GXFloat y )
{
	normalTextureScaleOffset.z = x;
	normalTextureScaleOffset.w = y;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetSpecularTexture ( GXTexture &texture )
{
	specularTexture = &texture;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetSpecularTextureScale ( GXFloat x, GXFloat y )
{
	specularTextureScaleOffset.x = x;
	specularTextureScaleOffset.y = y;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetSpecularTextureOffset ( GXFloat x, GXFloat y )
{
	specularTextureScaleOffset.z = x;
	specularTextureScaleOffset.w = y;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetEmissionTexture ( GXTexture &texture )
{
	emissionTexture = &texture;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetEmissionTextureScale ( GXFloat x, GXFloat y )
{
	emissionTextureScaleOffset.x = x;
	emissionTextureScaleOffset.y = y;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetEmissionTextureOffset ( GXFloat x, GXFloat y )
{
	emissionTextureScaleOffset.z = x;
	emissionTextureScaleOffset.w = y;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetEmissionTextureColor ( GXUByte red, GXUByte green, GXUByte blue )
{
	GXColorToVec3 ( emissionColor, red, green, blue );
}

GXVoid EMBlinnPhongCommonPassMaterial::SetDeltaTime ( GXFloat deltaTime )
{
	inverseDeltaTime = 1.0f / deltaTime;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetExplosureTime ( GXFloat time )
{
	explosureTime = time;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
	screenResolution.x = (GXFloat)width;
	screenResolution.y = (GXFloat)height;
}

GXVoid EMBlinnPhongCommonPassMaterial::SetMaxBlurSamples ( GXUByte samples )
{
	maxBlurSamples = (GXFloat)samples;
}
