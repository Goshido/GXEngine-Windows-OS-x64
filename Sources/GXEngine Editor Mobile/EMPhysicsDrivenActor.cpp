#include <GXEngine_Editor_Mobile/EMPhysicsDrivenActor.h>
#include <GXPhysics/GXBoxShape.h>
#include <GXPhysics/GXPlaneShape.h>
#include <GXPhysics/GXSphereShape.h>
#include <GXPhysics/GXPolygonShape.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


EMPhysicsDrivenActor::EMPhysicsDrivenActor ( eGXShapeType type )
{
	diffuseTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/gui_folder_icon.png", GX_TRUE, GL_REPEAT );
	normalTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Normals.tex", GX_FALSE, GL_REPEAT );
	emissionTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Emission.tex", GX_FALSE, GL_REPEAT );
	specularTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Specular.tex", GX_FALSE, GL_REPEAT );

	material.SetDiffuseTexture ( diffuseTexture );
	material.SetNormalTexture ( normalTexture );
	material.SetSpecularTexture ( specularTexture );
	material.SetEmissionTexture ( emissionTexture );

	rigidBody = new GXRigidBody ();

	switch ( type )
	{
		case eGXShapeType::Box:
		{
			mesh = new EMMesh ( L"3D Models/Editor Mobile/Unit Cube.stm" );
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
			mesh = new EMMesh ( L"3D Models/Editor Mobile/Unit Cube.stm" );
			GXSphereShape* p = new GXSphereShape ( rigidBody, 0.5f );
			rigidBody->SetShape ( *p );
		}
		break;

		case eGXShapeType::Polygon:
		{
			mesh = new EMMesh ( L"3D Models/Editor Mobile/Unit Cube.stm" );
			//GXPolygonShape* p = new GXPolygonShape ( nullptr, 30.0f, 30.0f );

			GXBoxShape* box = new GXBoxShape ( rigidBody, 100.0f, 1.0f, 100.0f );
			rigidBody->SetShape ( *box );
			rigidBody->EnableKinematic ();
		}
	}
}

EMPhysicsDrivenActor::~EMPhysicsDrivenActor ()
{
	GXTexture::RemoveTexture ( diffuseTexture );
	GXTexture::RemoveTexture ( normalTexture );
	GXTexture::RemoveTexture ( specularTexture );
	GXTexture::RemoveTexture ( emissionTexture );
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

	material.Bind ( *mesh );

	mesh->Render ();

	material.Unbind ();

	mesh->UpdateLastFrameModelMatrix ();
}
