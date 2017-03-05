//version 1.0

#ifndef GX_PARTICLE_SPRING
#define GX_PARTICLE_SPRING


#include "GXParticleForceGenerator.h"

class GXParticleSpring : public GXParticleForceGenerator
{
	private:
		GXParticle*		other;
		GXFloat			hardness;
		GXFloat			restLength;

	public:
		GXParticleSpring ( GXParticle& other, GXFloat hardness, GXFloat restLength );
		GXVoid UpdateForce ( GXParticle &particle, GXFloat deltaTime ) override;

};


#endif //GX_PARTICLE_SPRING

