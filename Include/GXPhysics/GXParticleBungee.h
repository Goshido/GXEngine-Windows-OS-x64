//version 1.0

#ifndef GX_PARTICLE_BUNGEE
#define GX_PARTICLE_BUNGEE


#include "GXParticleForceGenerator.h"


class GXParticleBungee : public GXParticleForceGenerator
{
private:
	GXParticle*		other;
	GXFloat			hardness;
	GXFloat			restLength;

public:
	GXParticleBungee ( GXParticle& other, GXFloat handess, GXFloat restLenght );
	GXVoid UpdateForce ( GXParticle &particle, GXFloat deltaTime ) override;
};


#endif //GX_PARTICLE_BUNGEE

