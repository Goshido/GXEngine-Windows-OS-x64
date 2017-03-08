#ifndef EM_PHYSICS_DRIVEN_ACTOR
#define EM_PHYSICS_DRIVEN_ACTOR


#include <GXPhysics/GXRigidBody.h>
#include <GXEngine/GXMesh.h>


enum class eEMPhysicsDrivenActorType
{
	Box,
	Plane
};

class EMPhysicsDrivenActor
{
	private:
		GXMesh*			mesh;
		GXRigidBody*	rigidBody;

	public:
		EMPhysicsDrivenActor ( eEMPhysicsDrivenActorType type );
		~EMPhysicsDrivenActor ();

		GXRigidBody& GetRigidBody ();
		GXVoid Draw ();
};


#endif EM_PHYSICS_DRIVEN_ACTOR
