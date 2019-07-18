#ifndef EM_PHYSICS_DRIVEN_ACTOR
#define EM_PHYSICS_DRIVEN_ACTOR


#include "EMActor.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include "EMWireframeMaterial.h"
#include <GXEngine/GXMeshGeometry.h>
#include <GXPhysics/GXRigidBody.h>


class EMPhysicsDrivenActor final : public EMActor
{
    private:
        EMWireframeMaterial*                _wireframeMaterial;
        GXTexture2D                         _albedo;
        GXTexture2D                         _normal;
        GXTexture2D                         _emission;
        GXTexture2D                         _parameter;

        GXRigidBody                         _rigidBody;
        GXMeshGeometry                      _mesh;
        EMCookTorranceCommonPassMaterial    _material;

        GXMeshGeometry                      _unitSphereMesh;
        GXMeshGeometry                      _unitCubeMesh;

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
