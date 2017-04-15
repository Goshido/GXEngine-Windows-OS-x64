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
	currentModelViewProjectionMatrixLocation = shaderProgram.GetUniform ( "currentModelViewProjectionMatrix" );
	currentRotationViewMatrixLocation = shaderProgram.GetUniform ( "currentRotationViewMatrix" );
	currentModelViewMatrixLocation = shaderProgram.GetUniform ( "currentModelViewMatrix" );
	lastFrameModelViewMatrixLocation = shaderProgram.GetUniform ( "lastFrameModelViewMatrix" );
	inverseDeltaTimeLocation = shaderProgram.GetUniform ( "inverseDeltaTime" );

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
}

EMBlinnPhongCommonPassMaterial::~EMBlinnPhongCommonPassMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMBlinnPhongCommonPassMaterial::Bind ( const GXTransform &transform ) const
{
	if ( !diffuseTexture || !normalTexture || !specularTexture || !emissionTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	GXCamera* camera = GXCamera::GetActiveCamera ();

	GXMat4 currentModelViewProjectionMatrix;
	GXMat3 currentRotationViewMatrix;
	GXMat4 currentModelViewMatrix;
	GXMat4 lastFrameModelViewMatrix;

	const GXMat4& currentModelMatrix = transform.GetCurrentModelMatrix ();
	GXMulMat4Mat4 ( currentModelViewProjectionMatrix, currentModelMatrix, camera->GetCurrentViewProjectionMatrix () );

	GXMat4 currentRotationMatrix;
	transform.GetRotation ( currentRotationMatrix );
	const GXMat4& currentViewMatrix = camera->GetCurrentViewMatrix ();

	GXMat3 r;
	r.xv = currentRotationMatrix.xv;
	r.yv = currentRotationMatrix.yv;
	r.zv = currentRotationMatrix.zv;

	GXMat3 v;
	v.xv = currentViewMatrix.xv;
	v.yv = currentViewMatrix.yv;
	v.zv = currentViewMatrix.zv;

	GXMulMat3Mat3 ( currentRotationViewMatrix, r, v );

	GXMulMat4Mat4 ( currentModelViewMatrix, currentModelMatrix, camera->GetCurrentViewMatrix () );

	GXMulMat4Mat4 ( lastFrameModelViewMatrix, transform.GetLastFrameModelMatrix (), camera->GetLastFrameViewMatrix () );

	glUniformMatrix4fv ( currentModelViewProjectionMatrixLocation, 1, GL_FALSE, currentModelViewProjectionMatrix.arr );
	glUniformMatrix3fv ( currentRotationViewMatrixLocation, 1, GL_FALSE, currentRotationViewMatrix.arr );
	glUniformMatrix4fv ( currentModelViewMatrixLocation, 1, GL_FALSE, currentModelViewMatrix.arr );
	glUniformMatrix4fv ( lastFrameModelViewMatrixLocation, 1, GL_FALSE, lastFrameModelViewMatrix.arr );

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

	glUniform1f ( inverseDeltaTimeLocation, inverseDeltaTime );
}

GXVoid EMBlinnPhongCommonPassMaterial::Unbind () const
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
