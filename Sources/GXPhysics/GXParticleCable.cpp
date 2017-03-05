//version 1.0

#include <GXPhysics/GXParticleCable.h>

GXParticleCable::GXParticleCable ( GXFloat maxLength, GXFloat restitution )
{
	this->maxLength = maxLength;
	this->restitution = restitution;
}

GXUInt GXParticleCable::FillContacs ( GXParticleContact* contact, GXUInt /*limit*/ ) const
{
	GXFloat length = GetCurrrentLength ();

	if ( length < maxLength ) return 0;

	contact->SetParticleA ( *particles[ 0 ] );
	contact->SetParticleB ( particles[ 1 ] );

	GXVec3 normal;
	GXSubVec3Vec3 ( normal, particles[ 1 ]->GetLocation (), particles[ 0 ]->GetLocation () );
	GXNormalizeVec3 ( normal );

	contact->SetNormal ( normal );

	contact->SetPenetration ( length - maxLength );
	contact->SetRestitution ( restitution );

	return 1;
}
