// vesrsion 1.0

#include <GXPhysics/GXParticle.h>
#include <limits>


GXVoid GXParticle::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	location = GXCreateVec3 ( x, y, z );
}

GXVoid GXParticle::SetLocation ( const GXVec3 &location )
{
	this->location = location;
}

const GXVec3& GXParticle::GetLocation () const
{
	return location;
}

GXVoid GXParticle::SetVelocity ( GXFloat x, GXFloat y, GXFloat z )
{
	velocity = GXCreateVec3 ( x, y, z );
}

GXVoid GXParticle::SetVelocity ( const GXVec3 &velocity )
{
	this->velocity = velocity;
}

const GXVec3& GXParticle::GetVelocity () const
{
	return velocity;
}

GXVoid GXParticle::SetAcceleration ( GXFloat x, GXFloat y, GXFloat z )
{
	acceleration = GXCreateVec3 ( x, y, z );
}

GXVoid GXParticle::SetAcceleration ( const GXVec3 &acceleration )
{
	this->acceleration = acceleration;
}

const GXVec3& GXParticle::GetAcceleration () const
{
	return acceleration;
}

GXVoid GXParticle::SetMass ( GXFloat mass )
{
	this->mass = mass;

	if ( mass <= 0.0f )
		invMass = FLT_MAX;
	else
		invMass = 1.0f / mass;
}

GXFloat GXParticle::GetMass () const
{
	return mass;
}

GXFloat GXParticle::GetInverseMass () const
{
	return invMass;
}

GXBool GXParticle::HasFiniteMass ()
{
	return invMass > 0.0f;
}

GXVoid GXParticle::SetDumping ( GXFloat damping )
{
	this->damping = damping;
}

GXVoid GXParticle::AddForce ( const GXVec3 &force )
{
	GXSumVec3Vec3 ( totalForce, totalForce, force );
}

GXVoid GXParticle::ClearForceAccumulator ()
{
	totalForce = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
}

GXVoid GXParticle::Integrate ( GXFloat deltaTime )
{
	GXVec3 locationDelta;
	GXMulVec3Scalar ( locationDelta, velocity, deltaTime );
	GXSumVec3Vec3 ( location, location, locationDelta );

	GXVec3 currentAcceleration;
	GXVec3 forceFactor;
	GXMulVec3Scalar ( forceFactor, totalForce, invMass );

	GXSumVec3Vec3 ( currentAcceleration, acceleration, forceFactor );
	GXVec3 velocityDelta;
	GXMulVec3Scalar ( velocityDelta, currentAcceleration, deltaTime );

	GXSumVec3Vec3 ( velocity, velocity, velocityDelta );

	GXFloat dampingFactor = powf ( damping, deltaTime );
	GXMulVec3Scalar ( velocity, velocity, dampingFactor );

	ClearForceAccumulator ();
}
