//version 1.0

#include <GXPhysics/GXParticleContact.h>

GXVoid GXParticleContact::SetParticleA ( GXParticle& particle )
{
	particles[ 0 ] = &particle;
}

GXVoid GXParticleContact::SetParticleB ( GXParticle* particle )
{
	particles[ 1 ] = particle;
}

GXVoid GXParticleContact::SetNormal ( const GXVec3 &normal )
{
	this->normal = normal;
}

GXVoid GXParticleContact::SetPenetration ( GXFloat penetration )
{
	this->penetration = penetration;
}

GXVoid GXParticleContact::SetRestitution ( GXFloat restitution )
{
	this->restitution = restitution;
}

GXVoid GXParticleContact::Resolve ( GXFloat deltaTime )
{
	ResolveVelocity ( deltaTime );
	ResolveInterpenetration ( deltaTime );
}

GXFloat GXParticleContact::CalculateSeparatingVelocity () const
{
	GXVec3 relativeVelocity = particles[ 0 ]->GetVelocity ();

	if ( particles[ 1 ] )
		GXSubVec3Vec3 ( relativeVelocity, relativeVelocity, particles[ 1 ]->GetVelocity () );

	return GXDotVec3Fast ( relativeVelocity, normal );
}

GXVoid GXParticleContact::ResolveVelocity ( GXFloat deltaTime )
{
	GXFloat separatingVelocity = CalculateSeparatingVelocity ();

	if ( separatingVelocity > 0 ) return;

	GXFloat newSeparatingVelocity = -separatingVelocity * restitution;
	GXVec3 accCausedVelocity = particles[ 0 ]->GetAcceleration ();
	if ( particles[ 1 ] )
		GXSubVec3Vec3 ( accCausedVelocity, accCausedVelocity, particles[ 1 ]->GetAcceleration () );

	GXFloat accCausedSepVelocity = GXDotVec3Fast ( accCausedVelocity, normal ) * deltaTime;

	if ( accCausedSepVelocity < 0 )
	{
		newSeparatingVelocity += restitution * accCausedSepVelocity;
		if ( newSeparatingVelocity < 0 )
			newSeparatingVelocity = 0.0f;
	}

	GXFloat deltaVelocity = newSeparatingVelocity - separatingVelocity;

	GXFloat totalInvMass = particles[ 0 ]->GetInverseMass ();
	if ( particles[ 1 ] )
		totalInvMass += particles[ 1 ]->GetInverseMass ();

	if ( totalInvMass <= 0.0f ) return;

	GXFloat impulse = deltaVelocity / totalInvMass;

	GXVec3 impulsePerInvMass;
	GXMulVec3Scalar ( impulsePerInvMass, normal, impulse );

	GXVec3 impulseFactor;
	GXMulVec3Scalar ( impulseFactor, impulsePerInvMass, particles[ 0 ]->GetInverseMass () );
	GXVec3 resultVelocity = particles[ 0 ]->GetVelocity ();
	GXSumVec3Vec3 ( resultVelocity, resultVelocity, impulseFactor );
	particles[ 0 ]->SetVelocity ( resultVelocity );

	if ( particles[ 1 ] )
	{
		GXMulVec3Scalar ( impulseFactor, impulsePerInvMass, -particles[ 1 ]->GetInverseMass () );
		resultVelocity = particles[ 1 ]->GetVelocity ();
		GXSumVec3Vec3 ( resultVelocity, resultVelocity, impulseFactor );
		particles[ 1 ]->SetVelocity ( resultVelocity );
	}
}

GXVoid GXParticleContact::ResolveInterpenetration ( GXFloat deltaTime )
{
	if ( penetration <= 0 ) return;

	GXFloat totalInvMass = particles[ 0 ]->GetInverseMass ();
	if ( particles[ 1 ] )
		totalInvMass += particles[ 1 ]->GetInverseMass ();

	if ( totalInvMass <= 0 ) return;

	GXVec3 movePerInvMass;
	GXMulVec3Scalar ( movePerInvMass, normal, -penetration / totalInvMass );

	GXVec3 moveFactor;
	GXMulVec3Scalar ( moveFactor, movePerInvMass, particles[ 0 ]->GetInverseMass () );
	GXSumVec3Vec3 ( moveFactor, moveFactor, particles[ 0 ]->GetLocation () );
	particles[ 0 ]->SetLocation ( moveFactor );

	if ( particles[ 1 ] )
	{
		GXMulVec3Scalar ( moveFactor, movePerInvMass, particles[ 1 ]->GetInverseMass () );
		GXSumVec3Vec3 ( moveFactor, moveFactor, particles[ 1 ]->GetLocation () );
		particles[ 1 ]->SetLocation ( moveFactor );
	}
}
