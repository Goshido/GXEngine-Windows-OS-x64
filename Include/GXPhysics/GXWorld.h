//version 1.0

#ifndef GX_WORLD
#define GX_WORLD


#include "GXRigidBody.h"
#include "GXCollisionDetector.h"
#include "GXContactResolver.h"
#include "GXCollisionData.h"
#include "GXForceGenerator.h"


struct GXRigidBodyRegistration;
struct GXContactGeneratorsRegistration;
struct GXForceGeneratorsRegistration;
class GXWorld
{
	private:
		GXBool								isCalculateIterations;

		GXRigidBodyRegistration*			bodies;
		GXContactGeneratorsRegistration*	contactGenerators;
		GXForceGeneratorsRegistration*		forceGenerators;

		GXContactResolver					contactResolver;
		GXCollisionDetector					collisionDetector;
		GXCollisionData						collisions;

	public:
		GXWorld ( GXUInt maxContacts, GXUInt iterations = 0 );
		~GXWorld ();

		GXVoid RegisterRigidBody ( GXRigidBody &body );
		GXVoid UnregisterRigidBody ( GXRigidBody &body );
		GXVoid ClearRigidBodyRegistrations ();

		GXVoid RegisterForceGenerator ( GXRigidBody &body, GXForceGenerator &generator );
		GXVoid UnregisterForceGenerator ( GXRigidBody &body, GXForceGenerator &generator );
		GXVoid ClearForceGeneratorRegistrations ();

		GXVoid RunPhysics ( GXFloat deltaTime );

	private:
		GXRigidBodyRegistration* FindRigidBodyRegistration ( GXRigidBody &body );
		GXForceGeneratorsRegistration* FindForceGeneratorRegistration ( GXRigidBody &body, GXForceGenerator &generator );
};


#endif GX_WORLD
