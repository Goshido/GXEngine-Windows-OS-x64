//version 1.0

#ifndef GX_PARTICLE_DRAG
#define GX_PARTICLE_DRAG


#include "GXParticleForceGenerator.h"


class GXParticleDrag : public GXParticleForceGenerator
{
	private:
		GXFloat		k1;
		GXFloat		k2;

	public:
		explicit GXParticleDrag ( GXFloat k1, GXFloat k2 );
		GXVoid UpdateForce ( GXParticle &particle, GXFloat deltaTime ) override;
};


#endif //GX_PARTICLE_DRAG
