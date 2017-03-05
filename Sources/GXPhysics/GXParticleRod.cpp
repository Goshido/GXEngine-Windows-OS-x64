//version 1.0

#include <GXPhysics/GXParticleRod.h>

GXParticleRod::GXParticleRod ( GXFloat length )
{
	this->length = length;
}

GXUInt GXParticleRod::FillContacs ( GXParticleContact* contact, GXUInt limit ) const
{
	GXFloat currentLength = GetCurrrentLength ();

	if ( currentLength == length ) return 0;

	contact->SetParticleA ( *particles[ 0 ] );
	contact->SetParticleB ( particles[ 1 ] );

	GXVec3 normal;
	GXSubVec3Vec3 ( normal, particles[ 1 ]->GetLocation (), particles[ 0 ]->GetLocation () );
	GXNormalizeVec3 ( normal );

	if ( currentLength > length )
	{
		contact->SetNormal ( normal );
		contact->SetPenetration ( currentLength - length );
	}
	else
	{
		contact->SetNormal ( GXCreateVec3 ( -normal.x, -normal.y, -normal.z ) );
		contact->SetPenetration ( length - currentLength );
	}

	contact->SetRestitution ( 0.0f );

	return 1;
}
