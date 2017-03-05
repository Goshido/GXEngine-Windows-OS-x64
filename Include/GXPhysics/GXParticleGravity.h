//version 1.0

#ifndef GX_PARTICLE_GRAVITY
#define GX_PARTICLE_GRAVITY


#include "GXParticleForceGenerator.h"


class GXParticleGravity : GXParticleForceGenerator
{
	private:
		GXVec3		gravity;

	public:
		GXParticleGravity ( GXFloat x, GXFloat y, GXFloat z );
		GXParticleGravity ( const GXVec3 &gravity );

		GXVoid UpdateForce ( GXParticle &particle, GXFloat deltaTime ) override;
};


#endif //GX_PARTICLE_GRAVITY
