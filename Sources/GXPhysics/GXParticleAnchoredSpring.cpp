//version 1.0

#include <GXPhysics/GXParticleAnchoredSpring.h>

GXParticleAnchoredSpring::GXParticleAnchoredSpring ( const GXVec3 &anchor, GXFloat hardness, GXFloat restLength )
{
	this->anchor = anchor;
	this->hardness = hardness;
	this->restLength = restLength;
}

GXVoid GXParticleAnchoredSpring::UpdateForce ( GXParticle &particle, GXFloat deltaTime )
{
	GXVec3 delta;
	GXSubVec3Vec3 ( delta, particle.GetLocation (), anchor );

	GXFloat forceFactor = fabsf ( GXLengthVec3 ( delta ) - restLength ) * hardness;
	GXNormalizeVec3 ( delta );
	GXVec3 force;
	GXMulVec3Scalar ( force, delta, -forceFactor );

	particle.AddForce ( force );
}
