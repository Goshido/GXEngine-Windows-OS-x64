//version 1.0

#ifndef GX_CONTACT_RESOLVER
#define GX_CONTACT_RESOLVER


#include "GXContact.h"


class GXContactResolver
{
	private:
		GXUInt		positionIterations;
		GXUInt		positionIterationsUsed;

		GXFloat		positionEpsilon;

	public:
		GXVoid ResolveContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime );

	private:
		GXVoid PrepareContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime );
		GXVoid AdjustPositions ( GXContact* contacts, GXUInt numContacts, GXFloat deltaTime );
};


#endif GX_CONTACT_RESOLVER
