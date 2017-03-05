//version 1.0

#include <GXPhysics/GXParticleFakeSpring.h>


GXParticleFakeSpring::GXParticleFakeSpring ( const GXVec3 &anchor, GXFloat hardness, GXFloat damping )
{
	this->anchor = anchor;
	this->hardness = hardness;
	this->damping = damping;
}

GXVoid GXParticleFakeSpring::UpdateForce ( GXParticle &particle, GXFloat deltaTime )
{
	if ( !particle.HasFiniteMass () ) return;

	GXVec3 delta;
	GXSubVec3Vec3 ( delta, particle.GetLocation (), anchor );

	GXFloat gamma = 0.5f * sqrtf ( 4.0f * hardness - damping * damping );
	if ( gamma == 0.0f ) return;

	GXVec3 v1;
	GXMulVec3Scalar ( v1, delta, damping * ( 2.0f * gamma ) );
	GXVec3 v2;
	GXMulVec3Scalar ( v2, particle.GetVelocity (), 1.0f / gamma );
	GXVec3 v3;
	GXSumVec3Vec3 ( v3, v1, v2 );

	GXVec3 v4;
	GXMulVec3Scalar ( v4, delta, cosf ( gamma * deltaTime ) );
	GXVec3 v5;
	GXMulVec3Scalar ( v5, v3, sinf ( gamma * deltaTime ) );

	GXVec3 target;
	GXSumVec3Vec3 ( target, v4, v5 );

	GXMulVec3Scalar ( target, target, expf ( -0.5f * deltaTime * damping ) );

	GXVec3 v6;
	GXSubVec3Vec3 ( v6, target, delta );
	GXMulVec3Scalar ( v6, v6, 1.0f / ( deltaTime * deltaTime ) );

	GXVec3 v7;
	GXMulVec3Scalar ( v7, particle.GetVelocity (), deltaTime );

	GXVec3 acceleration;
	GXSumVec3Vec3 ( acceleration, v6, v7 );

	GXVec3 force;
	GXMulVec3Scalar ( force, acceleration, particle.GetMass () );

	particle.AddForce ( force );
}
