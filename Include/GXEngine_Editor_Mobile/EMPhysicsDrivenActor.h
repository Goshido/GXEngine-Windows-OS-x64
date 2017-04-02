#ifndef EM_PHYSICS_DRIVEN_ACTOR
#define EM_PHYSICS_DRIVEN_ACTOR


#include <GXPhysics/GXRigidBody.h>
#include <GXPhysics/GXShape.h>
#include <GXEngine/GXRenderable.h>


enum class eEMPhysicsDrivenActorType
{
	Box,
	Plane,
	Sphere
};

class EMPhysicsDrivenActor
{
	private:
		GXRenderable*	mesh;
		GXRigidBody*	rigidBody;

	public:
		EMPhysicsDrivenActor ( eGXShapeType type );
		~EMPhysicsDrivenActor ();

		GXRigidBody& GetRigidBody ();
		GXVoid Draw ();
};


#endif //EM_PHYSICS_DRIVEN_ACTOR
