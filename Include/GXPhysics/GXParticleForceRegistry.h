//version 1.0

#ifndef GX_PARTICLE_FORCE_REGISTRY
#define GX_PARTICLE_FORCE_REGISTRY


#include "GXParticleForceGenerator.h"

struct GXParticleForceRegistryLink;
class GXParticleForceRegistry
{
	private:
		GXParticleForceRegistryLink*	registrations;

	public:
		GXParticleForceRegistry ();
		~GXParticleForceRegistry ();

		GXVoid Add ( GXParticle &particle, GXParticleForceGenerator &forceGenerator );
		GXVoid Remove ( GXParticle &particle, GXParticleForceGenerator &forceGenerator );
		GXVoid Clear ();

		GXVoid UpdateForces ( GXFloat deltaTIme );
};


#endif //GX_PARTICLE_FORCE_REGISTRY
