//version 1.0

#ifndef GX_PARTICLE_CONTACT_RESOLVER
#define GX_PARTICLE_CONTACT_RESOLVER


#include "GXParticleContact.h"


class GXParticleContactResolver
{
	protected:
		GXUInt		maxIterations;
		GXUInt		iteration;

	public:
		GXParticleContactResolver ( GXUInt maxIterations );

		GXVoid SetMaxIterations ( GXUInt maxIterations );
		GXVoid ResolveContacts ( GXParticleContact* contacts, GXUInt numContacts, GXFloat deltaTime );
};


#endif GX_PARTICLE_CONTACT_RESOLVER
