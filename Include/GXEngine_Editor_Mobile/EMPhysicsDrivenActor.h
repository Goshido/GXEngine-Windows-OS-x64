#ifndef EM_PHYSICS_DRIVEN_ACTOR
#define EM_PHYSICS_DRIVEN_ACTOR


#include "EMMesh.h"
#include "EMCookTorranceCommonPassMaterial.h"
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
		EMMesh*								mesh;
		GXRigidBody*						rigidBody;
		EMCookTorranceCommonPassMaterial	material;
		GXTexture							diffuseTexture;
		GXTexture							normalTexture;
		GXTexture							emissionTexture;
		GXTexture							parameterTexture;

	public:
		explicit EMPhysicsDrivenActor ( eGXShapeType type );
		~EMPhysicsDrivenActor ();

		GXRigidBody& GetRigidBody ();
		GXVoid Draw ( GXFloat deltaTime );
};


#endif //EM_PHYSICS_DRIVEN_ACTOR
