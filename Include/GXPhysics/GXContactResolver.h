//version 1.1

#ifndef GX_CONTACT_RESOLVER
#define GX_CONTACT_RESOLVER


#include "GXContact.h"


class GXContactResolver
{
	public:
		GXContactResolver ( GXUInt iterations );

		GXVoid ResolveContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime );

	private:
		GXVoid CalculateContactMatrix ( GXMat3 &out, const GXVec3 &contactNormal );
		GXFloat CalculateAngularComponentContactSpace ( const GXVec3 &centerOfMassToContactPoint, const GXRigidBody& rigidBody, const GXVec3 &contactNormal );

		GXVoid ResolveSingleBodyContacts ( GXRigidBody &rigidBody, GXContact* contacts );
		GXVoid ResolveDoubleBodyContacts ( GXContact* contacts );

		GXVoid GetRigidBodyKinematics ( GXVec3 &linearVelocity, GXVec3 &angularVelocity, GXRigidBody &rigidBody, const GXVec3 &impulse, const GXVec3 &centerOfMassToContactPoint );
};


#endif GX_CONTACT_RESOLVER
