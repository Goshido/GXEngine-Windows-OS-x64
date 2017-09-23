//version 1.1

#ifndef GX_CONTACT_RESOLVER
#define GX_CONTACT_RESOLVER


#include "GXContact.h"

//Basic physics concept from https://en.wikipedia.org/wiki/Collision_response
class GXContactResolver
{
	private:
		GXFloat		angularMoveLimit;

	public:
		GXContactResolver ();

		GXVoid ResolveContacts ( GXContact* contactArray, GXUInt numContacts );
		GXVoid SetAngularMoveLimit ( GXFloat limit );

	private:
		GXVoid ResolveSingleBodyContacts ( GXContact* contacts );
		GXVoid ResolveDoubleBodyContacts ( GXContact* contacts );

		GXVoid GetRigidBodyKinematicsWorld ( GXVec3 &linearVelocityWorld, GXVec3 &angularVelocityWorld, const GXRigidBody &rigidBody, const GXVec3 &impulseWorld, const GXVec3 &centerOfMassToContactPointWorld );
		GXVoid GetContactVelocityWorld ( GXVec3 &contactVelocityWorld, const GXRigidBody &rigidBodyA, const GXVec3 &rigidBodyACenterOfMassToContactPointWorld, const GXRigidBody &rigidBodyB, const GXVec3 &rigidBodyBCenterOfMassToContactPointWorld );
		GXVoid ResolvePenetrationWorld ( GXVec3 &deltaLocationWorld, GXVec3 &deltaRotationWorld, const GXRigidBody &rigidBody, const GXVec3 &centerOfMassToContactPointWorld, const GXVec3 &contactNormalWorld, GXFloat contactPenetration );
};


#endif GX_CONTACT_RESOLVER
