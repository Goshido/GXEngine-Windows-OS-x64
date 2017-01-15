#include <GXEngine_Editor_Mobile/EMUnitActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXTextureStorage.h>
#include <GXCommon/GXStrings.h>


#define EM_DIFFUSE		0
#define EM_NORMAL		1
#define EM_SPECULAR		2
#define EM_EMISSION		3


class EMUnitActorMesh : public GXMesh
{
	private:
		GLint			mod_view_proj_matLocation;
		GLint			mod_view_matLocation;

		GLuint			sampler;

	public:
		EMUnitActorMesh ();
		virtual ~EMUnitActorMesh ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};

EMUnitActorMesh::EMUnitActorMesh ()
{
	Load3DModel ();
	InitUniforms ();
}

EMUnitActorMesh::~EMUnitActorMesh ()
{
	GXRemoveVAO ( vaoInfo );
	glDeleteSamplers ( 1, &sampler );
}

GXVoid EMUnitActorMesh::Draw ()
{
	if ( !IsVisible () ) return;

	GXMat4 mod_view_mat;
	GXMat4 mod_view_proj_mat;

	GXCamera* activeCamera = GXCamera::GetActiveCamera ();

	GXMulMat4Mat4 ( mod_view_mat, mod_mat, activeCamera->GetViewMatrix () );
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, activeCamera->GetViewProjectionMatrix () );

	glUseProgram ( programs[ 0 ].program );

	glUniformMatrix4fv ( mod_view_matLocation, 1, GL_FALSE, mod_view_mat.A );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindSampler ( 0, sampler );
	glBindTexture ( GL_TEXTURE_2D, textures[ EM_DIFFUSE ].texObj );

	glActiveTexture ( GL_TEXTURE1 );
	glBindSampler ( 1, sampler );
	glBindTexture ( GL_TEXTURE_2D, textures[ EM_NORMAL ].texObj );

	glActiveTexture ( GL_TEXTURE2 );
	glBindSampler ( 2, sampler );
	glBindTexture ( GL_TEXTURE_2D, textures[ EM_SPECULAR ].texObj );

	glActiveTexture ( GL_TEXTURE3 );
	glBindSampler ( 3, sampler );
	glBindTexture ( GL_TEXTURE_2D, textures[ EM_EMISSION ].texObj );

	glBindVertexArray ( vaoInfo.vao );

	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glBindVertexArray ( 0 );

	glActiveTexture ( GL_TEXTURE0 );
	glBindSampler ( 0, 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glActiveTexture ( GL_TEXTURE1 );
	glBindSampler ( 1, 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glActiveTexture ( GL_TEXTURE2 );
	glBindSampler ( 2, 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glActiveTexture ( GL_TEXTURE3 );
	glBindSampler ( 3, 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glUseProgram ( 0 );
}

GXVoid EMUnitActorMesh::Load3DModel ()
{
	GXGetVAOFromNativeStaticMesh ( vaoInfo, L"3D Models/Editor Mobile/Unit Cube.stm" );

	AllocateTextures ( 4 );
	GXLoadTexture ( L"Textures/Editor Mobile/Default Diffuse.tex", textures[ EM_DIFFUSE ] );
	GXLoadTexture ( L"Textures/Editor Mobile/Default Normals.tex", textures[ EM_NORMAL ] );
	GXLoadTexture ( L"Textures/Editor Mobile/Default Specular.tex", textures[ EM_SPECULAR ] );
	GXLoadTexture ( L"Textures/Editor Mobile/Default Emission.tex", textures[ EM_EMISSION ] );

	AllocateShaderPrograms ( 1 );
	GXGetShaderProgram ( programs[ 0 ], L"Shaders/Editor Mobile/StaticMesh_vs.txt", 0, L"Shaders/Editor Mobile/StaticMesh_fs.txt" );

	GXGLSamplerStruct ss;
	ss.anisotropy = 16.0f;
	ss.resampling = GX_SAMPLER_RESAMPLING_TRILINEAR;
	ss.wrap = GL_REPEAT;
	sampler = GXCreateSampler ( ss );
}

GXVoid EMUnitActorMesh::InitUniforms ()
{
	if ( !programs[ 0 ].isSamplersTuned )
	{
		const GLuint samplerIndexes[ 4 ] = { 0, 1, 2, 3 };
		const GLchar* samplerNames[ 4 ] = { "diffuseSampler", "normalSampler", "specularSampler", "emissionSampler" };

		GXTuneShaderSamplers ( programs[ 0 ], samplerIndexes, samplerNames, 4 );
	}

	mod_view_proj_matLocation = GXGetUniformLocation ( programs[ 0 ].program, "mod_view_proj_mat" );
	mod_view_matLocation = GXGetUniformLocation ( programs[ 0 ].program, "mod_view_mat" );
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
	mesh->Draw ();
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
