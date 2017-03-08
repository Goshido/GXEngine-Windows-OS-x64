#include <GXEngine_Editor_Mobile/EMPhysicsDrivenActor.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXMemory.h>


#define EM_DIFFUSE		0
#define EM_NORMAL		1
#define EM_SPECULAR		2
#define EM_EMISSION		3


class EMPhysicsDrivenActorMesh : public GXMesh
{
	private:
		GLint			mod_view_proj_matLocation;
		GLint			mod_view_matLocation;
		GLuint			sampler;

	public:
		EMPhysicsDrivenActorMesh ();
		~EMPhysicsDrivenActorMesh () override;

		GXVoid Draw () override;

	protected:
		GXVoid Load3DModel () override;
		GXVoid InitUniforms () override;
};

EMPhysicsDrivenActorMesh::EMPhysicsDrivenActorMesh ()
{
	Load3DModel ();
	InitUniforms ();
}

EMPhysicsDrivenActorMesh::~EMPhysicsDrivenActorMesh ()
{
	GXRemoveVAO ( vaoInfo );
}

GXVoid EMPhysicsDrivenActorMesh::Draw ()
{
	if ( !IsVisible () ) return;

	GXMat4 mod_view_mat;
	GXMat4 mod_view_proj_mat;

	GXCamera* activeCamera = GXCamera::GetActiveCamera ();

	GXMulMat4Mat4 ( mod_view_mat, mod_mat, activeCamera->GetViewMatrix () );
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, activeCamera->GetViewProjectionMatrix () );

	glUseProgram ( programs[ 0 ].program );

	glUniformMatrix4fv ( mod_view_matLocation, 1, GL_FALSE, mod_view_mat.arr );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );

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

GXVoid EMPhysicsDrivenActorMesh::Load3DModel ()
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

GXVoid EMPhysicsDrivenActorMesh::InitUniforms ()
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

//------------------------------------------------------------

EMPhysicsDrivenActor::EMPhysicsDrivenActor ( eEMPhysicsDrivenActorType type )
{
	switch ( type )
	{
		case eEMPhysicsDrivenActorType::Box:
			mesh = new EMPhysicsDrivenActorMesh ();
		break;

		case eEMPhysicsDrivenActorType::Plane:
			mesh = nullptr;
		break;
	}
	
	rigidBody = new GXRigidBody ();
}

EMPhysicsDrivenActor::~EMPhysicsDrivenActor ()
{
	GXSafeDelete ( mesh );
	delete rigidBody;
}

GXRigidBody& EMPhysicsDrivenActor::GetRigidBody ()
{
	return *rigidBody;
}

GXVoid EMPhysicsDrivenActor::Draw ()
{
	if ( !mesh ) return;

	mesh->SetLocation ( rigidBody->GetLocation () );
	mesh->SetRotation ( rigidBody->GetRotation () );

	mesh->Draw ();
}
