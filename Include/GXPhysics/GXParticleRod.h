//version 1.0

#ifndef GX_PARTICLE_ROD
#define GX_PARTICLE_ROD


#include "GXParticleLink.h"


class GXParticleRod : public GXParticleLink
{
	private:
		GXFloat length;

	public:
		GXParticleRod ( GXFloat length );
		GXUInt FillContacs ( GXParticleContact* contact, GXUInt limit ) const override;
};


#endif GX_PARTICLE_ROD