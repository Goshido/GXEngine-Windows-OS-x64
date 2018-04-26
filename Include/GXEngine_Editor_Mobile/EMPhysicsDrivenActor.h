#ifndef EM_PHYSICS_DRIVEN_ACTOR
#define EM_PHYSICS_DRIVEN_ACTOR


#include "EMActor.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include "EMWireframeMaterial.h"
#include <GXEngine/GXMeshGeometry.h>
#include <GXPhysics/GXRigidBody.h>


class EMPhysicsDrivenActor : public EMActor
{
	private:
		EMWireframeMaterial*				wireframeMaterial;

		GXRigidBody							rigidBody;
		GXMeshGeometry						mesh;
		GXTexture2D							albedo;
		GXTexture2D							normal;
		GXTexture2D							emission;
		GXTexture2D							parameter;
		EMCookTorranceCommonPassMaterial	material;

		GXMeshGeometry						unitSphereMesh;
		GXMeshGeometry						unitCubeMesh;

	public:
		explicit EMPhysicsDrivenActor ( const GXWChar* name, const GXTransform &transform );
		~EMPhysicsDrivenActor () override;

		GXVoid OnDrawCommonPass ( GXFloat deltaTime ) override;
		GXVoid OnDrawHudColorPass () override;
		GXVoid OnTransformChanged () override;

		GXVoid SetMesh ( const GXWChar* meshFile );

		GXRigidBody& GetRigidBody ();
		EMCookTorranceCommonPassMaterial& GetMaterial ();

		GXVoid EnablePhysicsDebug ();
		GXVoid DisablePhysicsDebug ();

	private:
		static GXVoid GXCALL OnRigidBodyTransformChanged ( GXVoid* handler, const GXRigidBody &rigidBody );

		EMPhysicsDrivenActor () = delete;
		EMPhysicsDrivenActor ( const EMPhysicsDrivenActor &other ) = delete;
		EMPhysicsDrivenActor& operator = ( const EMPhysicsDrivenActor &other ) = delete;
};


#endif // EM_PHYSICS_DRIVEN_ACTOR
