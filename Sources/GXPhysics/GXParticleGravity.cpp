//version 1.0

#include <GXPhysics/GXParticleGravity.h>


GXParticleGravity::GXParticleGravity ( GXFloat x, GXFloat y, GXFloat z )
{
	gravity = GXCreateVec3 ( x, y, z );
}

GXParticleGravity::GXParticleGravity ( const GXVec3 &gravity )
{
	this->gravity = gravity;
}

GXVoid GXParticleGravity::UpdateForce ( GXParticle &particle, GXFloat deltaTime )
{
	GXVec3 gravityForce;
	GXMulVec3Scalar ( gravityForce, gravity, particle.GetMass () );
	particle.AddForce ( gravityForce );
}
