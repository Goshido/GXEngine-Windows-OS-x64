#include <GXEngine_Editor_Mobile/EMPhysicsDrivenActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXRenderer.h>
#include <GXPhysics/GXBoxShape.h>
#include <GXPhysics/GXPlaneShape.h>
#include <GXPhysics/GXSphereShape.h>
#include <GXPhysics/GXPolygonShape.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


EMPhysicsDrivenActor::EMPhysicsDrivenActor ( eGXShapeType type )
{
	diffuseTexture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/gui_folder_icon.png", GX_TRUE, GL_REPEAT, GX_FALSE );
	normalTexture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/Default Normals.tex", GX_FALSE, GL_REPEAT, GX_FALSE );
	emissionTexture = GXTexture2D::LoadTexture ( L"Textures/System/Default_Emission.tga", GX_FALSE, GL_REPEAT, GX_FALSE );
	parameterTexture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/Default Cook Torrance parameters.tga", GX_FALSE, GL_REPEAT, GX_FALSE );

	EMRenderer& renderer = EMRenderer::GetInstance ();

	material.SetAlbedoTexture ( diffuseTexture );
	material.SetNormalTexture ( normalTexture );
	material.SetEmissionTexture ( emissionTexture );
	material.SetParameterTexture ( parameterTexture );

	rigidBody = new GXRigidBody ();

	switch ( type )
	{
		case eGXShapeType::Box:
		{
			mesh = new EMMesh ( L"3D Models/System/Unit Cube.stm" );
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
			mesh = new EMMesh ( L"3D Models/System/Unit Cube.stm" );
			GXSphereShape* p = new GXSphereShape ( rigidBody, 0.5f );
			rigidBody->SetShape ( *p );
		}
		break;

		case eGXShapeType::Polygon:
		{
			mesh = new EMMesh ( L"3D Models/System/Unit Cube.stm" );
			//GXPolygonShape* p = new GXPolygonShape ( nullptr, 30.0f, 30.0f );

			GXBoxShape* box = new GXBoxShape ( rigidBody, 100.0f, 1.0f, 100.0f );
			rigidBody->SetShape ( *box );
			rigidBody->EnableKinematic ();
		}
	}
}

EMPhysicsDrivenActor::~EMPhysicsDrivenActor ()
{
	GXTexture2D::RemoveTexture ( diffuseTexture );
	GXTexture2D::RemoveTexture ( normalTexture );
	GXTexture2D::RemoveTexture ( emissionTexture );
	GXTexture2D::RemoveTexture ( parameterTexture );
	GXSafeDelete ( mesh );
	delete rigidBody;
}

GXRigidBody& EMPhysicsDrivenActor::GetRigidBody ()
{
	return *rigidBody;
}

GXVoid EMPhysicsDrivenActor::Draw ( GXFloat deltaTime )
{
	if ( !mesh ) return;

	GXBoxShape& s = (GXBoxShape&)rigidBody->GetShape ();
	EMRenderer& renderer = EMRenderer::GetInstance ();
	GXRenderer& coreRenderer = GXRenderer::GetInstance ();

	mesh->SetLocation ( rigidBody->GetLocation () );
	mesh->SetScale ( s.GetWidth (), s.GetHeight (), s.GetDepth () );

	GXQuat rot = rigidBody->GetRotation ();
	GXQuatRehandCoordinateSystem ( rot );
	mesh->SetRotation ( rot );
	
	renderer.SetObjectMask ( nullptr );

	material.SetDeltaTime ( deltaTime );
	material.SetExposure ( renderer.GetMotionBlurExposure () );
	material.SetScreenResolution ( (GXUShort)coreRenderer.GetWidth (), (GXUShort)coreRenderer.GetHeight () );
	material.SetMaximumBlurSamples ( renderer.GetMaximumMotionBlurSamples () );
	material.Bind ( *mesh );

	mesh->Render ();

	material.Unbind ();

	mesh->UpdateLastFrameModelMatrix ();
}
