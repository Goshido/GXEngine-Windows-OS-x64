#ifndef EM_PHYSICS_DRIVEN_ACTOR
#define EM_PHYSICS_DRIVEN_ACTOR


#include "EMMesh.h"
#include "EMBlinnPhongCommonPassMaterial.h"
#include <GXPhysics/GXRigidBody.h>
#include <GXPhysics/GXShape.h>


enum class eEMPhysicsDrivenActorType
{
	Box,
	Plane,
	Sphere
};

class EMPhysicsDrivenActor
{
	private:
		EMMesh*							mesh;
		GXRigidBody*					rigidBody;
		EMBlinnPhongCommonPassMaterial	material;
		GXTexture						diffuseTexture;
		GXTexture						normalTexture;
		GXTexture						specularTexture;
		GXTexture						emissionTexture;

	public:
		EMPhysicsDrivenActor ( eGXShapeType type );
		~EMPhysicsDrivenActor ();

		GXRigidBody& GetRigidBody ();
		GXVoid Draw ( GXFloat deltaTime );
};


#endif //EM_PHYSICS_DRIVEN_ACTOR
