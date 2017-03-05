//version 1.0

#include <GXPhysics/GXParticleSpring.h>

GXParticleSpring::GXParticleSpring ( GXParticle& other, GXFloat hardness, GXFloat restLength )
{
	this->other = &other;
	this->hardness = hardness;
	this->restLength = restLength;
}

GXVoid GXParticleSpring::UpdateForce ( GXParticle &particle, GXFloat deltaTime )
{
	GXVec3 delta;
	GXSumVec3Vec3 ( delta, particle.GetLocation (), other->GetLocation () );

	GXFloat deltaLength = GXLengthVec3 ( delta );
	GXFloat forceFactor = fabsf ( deltaLength - restLength ) * hardness;

	GXNormalizeVec3 ( delta );
	GXVec3 force;
	GXMulVec3Scalar ( force, delta, -forceFactor );

	particle.AddForce ( force );
}
