//vesrcion 1.0

#ifndef GX_PARTICLE_ANCHORED_SPRING
#define GX_PARTICLE_ANCHORED_SPRING


#include "GXParticleForceGenerator.h"


class GXParticleAnchoredSpring : public GXParticleForceGenerator
{
	private:
		GXVec3		anchor;
		GXFloat		hardness;
		GXFloat		restLength;

	public:
		GXParticleAnchoredSpring ( const GXVec3 &anchor, GXFloat hardness, GXFloat restLength );
		GXVoid UpdateForce ( GXParticle &particle, GXFloat deltaTime ) override;
};


#endif GX_PARTICLE_ANCHORED_SPRING
