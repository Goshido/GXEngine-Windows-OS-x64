//version 1.0

#ifndef GX_CONTACT_RESOLVER
#define GX_CONTACT_RESOLVER


#include "GXContact.h"


class GXContactResolver
{
	private:
		GXFloat		positionEpsilon;
		GXUInt		positionIterations;
		GXUInt		positionIterationsUsed;

		GXFloat		velocityEpsilon;
		GXUInt		velocityIterations;
		GXUInt		velocityIterationsUsed;

	public:
		GXContactResolver ( GXUInt iterations );

		GXVoid ResolveContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime );

		GXVoid SetPositionEpsilon ( GXFloat epsilon );
		GXVoid SetVelocityEpsilon ( GXFloat epsilon );

		GXVoid SetVelocityIterations ( GXUInt iterations );
		GXVoid SetPositionIterations ( GXUInt iterations );

	private:
		GXVoid PrepareContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime );
		GXVoid AdjustPositions ( GXContact* contacts, GXUInt numContacts, GXFloat deltaTime );
		GXVoid AdjustVelocities ( GXContact* contacts, GXUInt numContacts, GXFloat deltaTime );
		GXBool IsValid () const;
};


#endif GX_CONTACT_RESOLVER
