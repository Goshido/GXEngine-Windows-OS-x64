//version 1.0

#ifndef GX_PARTICLE_BUOYANCY
#define GX_PARTICLE_BUOYANCY


#include "GXParticleForceGenerator.h"


class GXParticleBuoyancy : public GXParticleForceGenerator
{
private:
	GXFloat		maxDepth;
	GXFloat		volume;
	GXFloat		liquidHeight;
	GXFloat		liquidDensity;

public:
	GXParticleBuoyancy ( GXFloat maxDepth, GXFloat volume, GXFloat liquidHeight, GXFloat liquidDensity );
	GXVoid UpdateForce ( GXParticle &particle, GXFloat deltaTime ) override;
};


#endif //GX_PARTICLE_BUOYANCY
