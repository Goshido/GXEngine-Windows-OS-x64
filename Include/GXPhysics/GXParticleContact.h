//version 1.0

#ifndef GX_PARTICLE_CONTACT
#define GX_PARTICLE_CONTACT


#include "GXParticle.h"

class GXParticleContact
{
	private:
		GXParticle*				particles[ 2 ]; //nullptr means contact with scenery
		GXFloat					restitution;
		GXVec3					normal; //from first particle perspective
		GXFloat					penetration;

	public:
		GXVoid SetParticleA ( GXParticle& particle );
		GXVoid SetParticleB ( GXParticle* particle );

		GXVoid SetNormal ( const GXVec3 &normal );
		GXVoid SetPenetration ( GXFloat penetration );
		GXVoid SetRestitution ( GXFloat restitution );

		GXVoid Resolve ( GXFloat deltaTime );
		GXFloat CalculateSeparatingVelocity () const;

	private:
		GXVoid ResolveVelocity ( GXFloat deltaTime );
		GXVoid ResolveInterpenetration ( GXFloat deltaTime );
};


#endif //GX_PARTICLE_CONTACT
