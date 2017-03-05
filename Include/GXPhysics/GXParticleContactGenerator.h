//version 1.0

#ifndef GX_PARTICLE_CONTACT_GENERATOR
#define GX_PARTICLE_CONTACT_GENERATOR


#include "GXParticleContact.h"


class GXParticleContactGenerator
{
	public:
		virtual GXUInt AddContact ( GXParticleContact* contact, GXUInt limit ) const = 0;
};


#endif GX_PARTICLE_CONTACT_GENERATOR
