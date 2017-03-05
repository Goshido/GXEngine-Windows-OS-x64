//version 1.0

#ifndef GX_PARTICLE_FORCE_GENERATOR
#define GX_PARTICLE_FORCE_GENERATOR


#include "GXParticle.h"


class GXParticleForceGenerator
{
public:
	virtual GXVoid UpdateForce ( GXParticle &particle, GXFloat deltaTime ) = 0;
};


#endif //GX_PARTICLE_FORCE_GENERATOR
