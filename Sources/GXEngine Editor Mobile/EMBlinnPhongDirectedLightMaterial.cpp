#include <GXEngine_Editor_Mobile/EMBlinnPhongDirectedLightMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_TO_LIGHT_DIRECTION_VIEW_X	0.0f
#define DEFAULT_TO_LIGHT_DIRECTION_VIEW_Y	-1.0f
#define DEFAULT_TO_LIGHT_DIRECTION_VIEW_Z	0.0f

#define DEFAULT_COLOR_R						0.9f
#define DEFAULT_COLOR_G						0.9f
#define DEFAULT_COLOR_B						0.9f

#define DEFAULT_AMBIENT_COLOR_R				0.1f
#define DEFAULT_AMBIENT_COLOR_B				0.1f
#define DEFAULT_AMBIENT_COLOR_G				0.1f

#define DIFFUSE_SLOT						0
#define NORMAL_SLOT							1
#define SPECULAR_SLOT						2
#define EMISSION_SLOT						3
#define DEPTH_SLOT							4

#define VERTEX_SHADER						L"Shaders/Editor Mobile/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/DirectedLight_fs.txt"


EMBlinnPhongDirectedLightMaterial::EMBlinnPhongDirectedLightMaterial ()
{
	static const GLchar* samplerNames[ 5 ] = { "diffuseSampler", "normalSampler", "specularSampler", "emissionSampler", "depthSampler" };
	static const GLuint samplerLocations[ 5 ] = { DIFFUSE_SLOT, NORMAL_SLOT, SPECULAR_SLOT, EMISSION_SLOT, DEPTH_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 5;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	toLightDirectionViewLocation = shaderProgram.GetUniform ( "toLightDirectionView" );
	inv_proj_matLocation = shaderProgram.GetUniform ( "inv_proj_mat" );
	colorLocation = shaderProgram.GetUniform ( "color" );
	ambientColorLocation = shaderProgram.GetUniform ( "ambientColor" );

	diffuseTexture = nullptr;
	normalTexture = nullptr;
	specularTexture = nullptr;
	emissionTexture = nullptr;
	depthTexture = nullptr;

	SetToLightDirectionView ( GXCreateVec3 ( DEFAULT_TO_LIGHT_DIRECTION_VIEW_X, DEFAULT_TO_LIGHT_DIRECTION_VIEW_Y, DEFAULT_TO_LIGHT_DIRECTION_VIEW_Z ) );
	SetColor ( GXCreateVec3 ( DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B ) );
	SetAmbientColor ( GXCreateVec3 ( DEFAULT_AMBIENT_COLOR_G, DEFAULT_AMBIENT_COLOR_G, DEFAULT_AMBIENT_COLOR_B ) );
}

EMBlinnPhongDirectedLightMaterial::~EMBlinnPhongDirectedLightMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMBlinnPhongDirectedLightMaterial::Bind ( const GXTransform& /*transform*/ ) const
{
	if ( !diffuseTexture || !normalTexture || !specularTexture || !emissionTexture || !depthTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	const GXMat4& inverseProjectionMatrix = GXCamera::GetActiveCamera ()->GetInverseProjectionMatrix ();
	glUniformMatrix4fv ( inv_proj_matLocation, 1, GL_FALSE, inverseProjectionMatrix.arr );

	glUniform3fv ( toLightDirectionViewLocation, 1, toLightDirectionView.arr );
	glUniform3fv ( colorLocation, 1, color.arr );
	glUniform3fv ( ambientColorLocation, 1, ambientColor.arr );

	diffuseTexture->Bind ( DIFFUSE_SLOT );
	normalTexture->Bind ( NORMAL_SLOT );
	specularTexture->Bind ( SPECULAR_SLOT );
	emissionTexture->Bind ( EMISSION_SLOT );
	depthTexture->Bind ( DEPTH_SLOT );
}

GXVoid EMBlinnPhongDirectedLightMaterial::Unbind () const
{
	if ( !diffuseTexture || !normalTexture || !specularTexture || !emissionTexture || !depthTexture ) return;

	diffuseTexture->Unbind ();
	normalTexture->Unbind ();
	specularTexture->Unbind ();
	emissionTexture->Unbind ();
	depthTexture->Unbind ();

	glUseProgram ( 0 );
}

GXVoid EMBlinnPhongDirectedLightMaterial::SetDiffuseTexture ( GXTexture &texture )
{
	diffuseTexture = &texture;
}

GXVoid EMBlinnPhongDirectedLightMaterial::SetNormalTexture ( GXTexture &texture )
{
	normalTexture = &texture;
}

GXVoid EMBlinnPhongDirectedLightMaterial::SetSpecularTexture ( GXTexture &texture )
{
	specularTexture = &texture;
}

GXVoid EMBlinnPhongDirectedLightMaterial::SetEmissionTexture ( GXTexture &texture )
{
	emissionTexture = &texture;
}

GXVoid EMBlinnPhongDirectedLightMaterial::SetDepthTexture ( GXTexture &texture )
{
	depthTexture = &texture;
}

GXVoid EMBlinnPhongDirectedLightMaterial::SetToLightDirectionView ( const GXVec3 &toLightDirectionView )
{
	this->toLightDirectionView = toLightDirectionView;
}

GXVoid EMBlinnPhongDirectedLightMaterial::SetColor ( const GXVec3 &color )
{
	this->color = color;
}

GXVoid EMBlinnPhongDirectedLightMaterial::SetAmbientColor ( const GXVec3 &color )
{
	this->ambientColor = color;
}
