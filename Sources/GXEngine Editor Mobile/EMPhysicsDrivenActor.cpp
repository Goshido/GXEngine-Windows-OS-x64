#include <GXEngine_Editor_Mobile/EMPhysicsDrivenActor.h>
#include <GXEngine/GXTexture.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXCamera.h>
#include <GXPhysics/GXBoxShape.h>
#include <GXPhysics/GXPlaneShape.h>
#include <GXPhysics/GXSphereShape.h>
#include <GXPhysics/GXPolygonShape.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


#define DIFFUSE_SLOT	0
#define NORMAL_SLOT		1
#define SPECULAR_SLOT	2
#define EMISSION_SLOT	3


class EMPhysicsDrivenActorMesh : public GXRenderable
{
	private:
		GXMeshGeometry		mesh;

		GXTexture			diffuseTexture;
		GXTexture			normalTexture;
		GXTexture			specularTexture;
		GXTexture			emissionTexture;

		GXShaderProgram		shaderProgram;
		GLint				mod_view_proj_matLocation;
		GLint				mod_view_matLocation;

		GLuint				sampler;

	public:
		EMPhysicsDrivenActorMesh ();
		~EMPhysicsDrivenActorMesh () override;

		GXVoid Render () override;

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid UpdateBounds () override;
};

EMPhysicsDrivenActorMesh::EMPhysicsDrivenActorMesh ()
{
	InitGraphicResources ();
}

EMPhysicsDrivenActorMesh::~EMPhysicsDrivenActorMesh ()
{
	GXMeshGeometry::RemoveMeshGeometry ( mesh );

	GXTexture::RemoveTexture ( diffuseTexture );
	GXTexture::RemoveTexture ( normalTexture );
	GXTexture::RemoveTexture ( emissionTexture );
	GXTexture::RemoveTexture ( specularTexture );

	GXShaderProgram::RemoveShaderProgram ( shaderProgram );

	glDeleteSamplers ( 1, &sampler );
}

GXVoid EMPhysicsDrivenActorMesh::Render ()
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

	mesh.Render ();

	glBindSampler ( DIFFUSE_SLOT, 0 );
	diffuseTexture.Unbind ();

	glBindSampler ( NORMAL_SLOT, 0 );
	normalTexture.Unbind ();

	glBindSampler ( EMISSION_SLOT, 0 );
	emissionTexture.Unbind ();

	glBindSampler ( SPECULAR_SLOT, 0 );
	specularTexture.Unbind ();

	glUseProgram ( 0 );
}

GXVoid EMPhysicsDrivenActorMesh::InitGraphicResources ()
{
	mesh = GXMeshGeometry::LoadFromStm ( L"3D Models/Editor Mobile/Unit Cube.stm" );
	UpdateBounds ();

	diffuseTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/gui_file_icon.png", GX_TRUE );
	normalTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Normals.tex", GX_TRUE );
	emissionTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Specular.tex", GX_TRUE );
	specularTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Emission.tex", GX_TRUE );

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

GXVoid EMPhysicsDrivenActorMesh::UpdateBounds ()
{
	//TODO
}

//------------------------------------------------------------

EMPhysicsDrivenActor::EMPhysicsDrivenActor ( eGXShapeType type )
{
	rigidBody = new GXRigidBody ();

	switch ( type )
	{
		case eGXShapeType::Box:
		{
			mesh = new EMPhysicsDrivenActorMesh ();
			GXBoxShape* box = new GXBoxShape ( rigidBody, 1.0f, 1.0f, 1.0f );
			rigidBody->SetShape ( *box );
		}
		break;

		case eGXShapeType::Plane:
		{
			mesh = nullptr;

			GXPlane plane;
			plane.a = 0.0f;
			plane.b = 1.0f;
			plane.c = 0.0f;
			plane.d = -2.0f;

			GXPlaneShape* p = new GXPlaneShape ( nullptr, plane );
			rigidBody->SetShape ( *p );
		}
		break;

		case eGXShapeType::Sphere:
		{
			mesh = new EMPhysicsDrivenActorMesh ();
			GXSphereShape* p = new GXSphereShape ( rigidBody, 0.5f );
			rigidBody->SetShape ( *p );
		}
		break;

		case eGXShapeType::Polygon:
		{
			mesh = new EMPhysicsDrivenActorMesh ();
			//GXPolygonShape* p = new GXPolygonShape ( nullptr, 30.0f, 30.0f );

			GXBoxShape* box = new GXBoxShape ( rigidBody, 100.0f, 1.0f, 100.0f );
			rigidBody->SetShape ( *box );
			rigidBody->EnableKinematic ();
		}
	}
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

	GXBoxShape& s = (GXBoxShape&)rigidBody->GetShape ();

	mesh->SetLocation ( rigidBody->GetLocation () );
	mesh->SetScale ( s.GetWidth (), s.GetHeight (), s.GetDepth () );

	GXQuat rot = rigidBody->GetRotation ();
	GXQuatRehandCoordinateSystem ( rot );
	mesh->SetRotation ( rot );

	mesh->Render ();
}
