//version 1.0

#ifndef GX_PARTICLE_CABLE
#define GX_PARTICLE_CABLE


#include "GXParticleLink.h"


class GXParticleCable : public GXParticleLink
{
	private:
		GXFloat		maxLength;
		GXFloat		restitution;

	public:
		GXParticleCable ( GXFloat maxLength, GXFloat restitution );
		GXUInt FillContacs ( GXParticleContact* contact, GXUInt limit ) const override;
};


#endif GX_PARTICLE_CABLE
