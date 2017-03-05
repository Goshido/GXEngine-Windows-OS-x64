//version 1.0

#ifndef GX_PARTICLE_FAKE_SPRING
#define GX_PARTICLE_FAKE_SPRING


#include "GXParticleForceGenerator.h"


class GXParticleFakeSpring : public GXParticleForceGenerator
{
	private:
		GXVec3		anchor;
		GXFloat		hardness;
		GXFloat		damping;

	public:
		GXParticleFakeSpring ( const GXVec3 &anchor, GXFloat hardness, GXFloat damping );
		GXVoid UpdateForce ( GXParticle &particle, GXFloat deltaTime ) override;
};


#endif //GX_PARTICLE_FAKE_SPRING
