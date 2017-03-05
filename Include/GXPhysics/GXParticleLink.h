//version 1.0

#ifndef GX_PARTICLE_LINK
#define GX_PARTICLE_LINK


#include "GXParticleContact.h"


class GXParticleLink
{
	protected:
		GXParticle*		particles[ 2 ];

	public:
		virtual GXUInt FillContacs ( GXParticleContact* contact, GXUInt limit ) const = 0;

	protected:
		GXFloat GetCurrrentLength () const;
};


#endif GX_PARTICLE_LINK
