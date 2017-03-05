//version 1.0

#include <GXPhysics/GXParticleDrag.h>

GXParticleDrag::GXParticleDrag ( GXFloat k1, GXFloat k2 )
{
	this->k1 = k1;
	this->k2 = k2;
}

GXVoid GXParticleDrag::UpdateForce ( GXParticle &particle, GXFloat /*deltaTime*/ )
{
	GXVec3 velocity = particle.GetVelocity ();
	GXFloat speed = GXLengthVec3 ( velocity );
	GXFloat drag = speed * ( k1 + k2 * speed );

	GXNormalizeVec3 ( velocity );

	GXVec3 force;
	GXMulVec3Scalar ( force, velocity, -drag );
	particle.AddForce ( force );
}
