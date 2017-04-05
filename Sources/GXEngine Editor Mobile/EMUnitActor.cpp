#include <GXEngine_Editor_Mobile/EMUnitActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXTexture.h>
#include <GXEngine/GXRenderable.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


#define DIFFUSE_SLOT		0
#define NORMAL_SLOT			1
#define SPECULAR_SLOT		2
#define EMISSION_SLOT		3


class EMUnitActorMesh : public GXRenderable
{
	private:
		GXShaderProgram		shaderProgram;
		GLint				mod_view_proj_matLocation;
		GLint				mod_view_matLocation;

		GXMeshGeometry		meshGeometry;

		GXTexture			diffuseTexture;
		GXTexture			normalTexture;
		GXTexture			emissionTexture;
		GXTexture			specularTexture;

		GLuint				sampler;

	public:
		EMUnitActorMesh ();
		~EMUnitActorMesh () override;

		GXVoid Render () override;

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid UpdateBounds () override;
};

EMUnitActorMesh::EMUnitActorMesh ()
{
	InitGraphicResources ();
}

EMUnitActorMesh::~EMUnitActorMesh ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );

	GXTexture::RemoveTexture ( diffuseTexture );
	GXTexture::RemoveTexture ( normalTexture );
	GXTexture::RemoveTexture ( emissionTexture );
	GXTexture::RemoveTexture ( specularTexture );

	GXMeshGeometry::RemoveMeshGeometry ( meshGeometry );

	glDeleteSamplers ( 1, &sampler );
}

GXVoid EMUnitActorMesh::Render ()
{
	GXCamera* activeCamera = GXCamera::GetActiveCamera ();

	GXMat4 mod_view_mat;
	GXMat4 mod_view_proj_mat;

	GXMulMat4Mat4 ( mod_view_mat, mod_mat, activeCamera->GetViewMatrix () );
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, activeCamera->GetViewProjectionMatrix () );

	glUseProgram ( shaderProgram.GetProgram () );

	glUniformMatrix4fv ( mod_view_matLocation, 1, GL_FALSE, mod_view_mat.arr );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );

	glBindSampler ( DIFFUSE_SLOT, sampler );
	diffuseTexture.Bind ( DIFFUSE_SLOT );

	glBindSampler ( NORMAL_SLOT, sampler );
	normalTexture.Bind ( NORMAL_SLOT );

	glBindSampler ( SPECULAR_SLOT, sampler );
	specularTexture.Bind ( SPECULAR_SLOT );

	glBindSampler ( EMISSION_SLOT, sampler );
	emissionTexture.Bind ( EMISSION_SLOT );

	meshGeometry.Render ();

	glBindSampler ( DIFFUSE_SLOT, 0 );
	diffuseTexture.Unbind ();

	glBindSampler ( NORMAL_SLOT, 0 );
	normalTexture.Unbind ();

	glBindSampler ( SPECULAR_SLOT, 0 );
	specularTexture.Unbind ();

	glBindSampler ( EMISSION_SLOT, 0 );
	emissionTexture.Unbind ();
	
	glUseProgram ( 0 );
}

GXVoid EMUnitActorMesh::InitGraphicResources ()
{
	meshGeometry = GXMeshGeometry::LoadFromStm ( L"3D Models/Editor Mobile/Unit Cube.stm" );
	UpdateBounds ();

	diffuseTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Diffuse.tex", GX_FALSE );
	normalTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Normals.tex", GX_FALSE );
	specularTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Specular.tex", GX_FALSE );
	emissionTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Emission.tex", GX_FALSE );

	static const GLchar* samplerNames[ 4 ] = { "diffuseSampler", "normalSampler", "specularSampler", "emissionSampler" };
	static const GLuint samplerLocations[ 4 ] = { 0, 1, 2, 3 };

	GXShaderProgramInfo si;
	si.vs = L"Shaders/Editor Mobile/StaticMesh_vs.txt";
	si.gs = nullptr;
	si.fs = L"Shaders/Editor Mobile/StaticMesh_fs.txt";
	si.numSamplers = 4;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	mod_view_proj_matLocation = shaderProgram.GetUniform ( "mod_view_proj_mat" );
	mod_view_matLocation = shaderProgram.GetUniform ( "mod_view_mat" );

	GXGLSamplerInfo samplerInfo;
	samplerInfo.anisotropy = 16.0f;
	samplerInfo.resampling = eGXSamplerResampling::Trilinear;
	samplerInfo.wrap = GL_REPEAT;
	sampler = GXCreateSampler ( samplerInfo );
}

GXVoid EMUnitActorMesh::UpdateBounds ()
{
	//TODO
}

//-------------------------------------------------------------------------------------------

EMUnitActor::EMUnitActor ( const GXWChar* name, const GXMat4 &transform ):
EMActor ( name, EM_UNIT_ACTOR_CLASS, transform )
{
	mesh = new EMUnitActorMesh ();
	OnTransformChanged ();
}

EMUnitActor::~EMUnitActor ()
{
	delete mesh;
}

GXVoid EMUnitActor::OnDrawCommonPass ()
{
	EMRenderer::GetInstance ()->SetObjectMask ( (GXUPointer)this );
	mesh->Render ();
}

GXVoid EMUnitActor::OnSave ( GXUByte** data )
{
	EMActorHeader* header = (EMActorHeader*)data;

	header->type = type;
	memcpy ( &header->origin, &transform, sizeof ( GXMat4 ) );
	header->isVisible = isVisible;
	header->nameOffset = sizeof ( EMActorHeader );

	GXUTF8* nameUTF8;
	GXUInt nameSize = GXToUTF8 ( &nameUTF8, name );

	memcpy ( data + sizeof ( EMActorHeader ), nameUTF8, nameSize );
	free ( nameUTF8 );

	header->size = sizeof ( EMActorHeader ) + nameSize;
}

GXVoid EMUnitActor::OnLoad ( const GXUByte* data )
{
	EMActorHeader* header = (EMActorHeader*)data;

	isVisible = header->isVisible;

	GXSafeFree ( name );
	GXToWcs ( &name, (GXUTF8*)( data + header->nameOffset ) );

	memcpy ( &transform, &header->origin, sizeof ( GXMat4 ) );
}

GXUInt EMUnitActor::OnRequeredSaveSize ()
{
	GXUInt total = sizeof ( EMActorHeader );

	GXUTF8* nameUTF8;
	total += GXToUTF8 ( &nameUTF8, name );

	free ( nameUTF8 );

	return total;
}

GXVoid EMUnitActor::OnTransformChanged ()
{
	mesh->SetLocation ( transform.wv );

	GXMat4 cleanRotation;
	GXSetMat4ClearRotation ( cleanRotation, transform );

	mesh->SetRotation ( cleanRotation );
}
