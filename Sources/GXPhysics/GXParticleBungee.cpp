//version 1.0

#include <GXPhysics/GXParticleBungee.h>

GXParticleBungee::GXParticleBungee ( GXParticle& other, GXFloat handess, GXFloat restLengh )
{
	this->other = &other;
	this->hardness = hardness;
	this->restLength = restLengh;
}

GXVoid GXParticleBungee::UpdateForce ( GXParticle &particle, GXFloat deltaTime )
{
	GXVec3 delta;
	GXSubVec3Vec3 ( delta, particle.GetLocation (), other->GetLocation () );

	GXFloat magnitude = GXLengthVec3 ( delta );
	if ( magnitude <= restLength ) return;

	GXFloat forceFactor = hardness * ( restLength - magnitude );
	GXNormalizeVec3 ( delta );

	GXVec3 force;
	GXMulVec3Scalar ( force, delta, -forceFactor );
}
