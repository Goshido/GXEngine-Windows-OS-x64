//version 1.0

#include <GXPhysics/GXParticleBuoyancy.h>


GXParticleBuoyancy::GXParticleBuoyancy ( GXFloat maxDepth, GXFloat volume, GXFloat liquidHeight, GXFloat liquidDensity )
{
	this->maxDepth = maxDepth;
	this->volume = volume;
	this->liquidHeight = liquidHeight;
	this->liquidDensity = liquidDensity;
}

GXVoid GXParticleBuoyancy::UpdateForce ( GXParticle &particle, GXFloat deltaTime )
{
	GXVec3 particleLocation = particle.GetLocation ();
	GXFloat depth = particleLocation.y;

	GXVec3 force ( 0.0f, 0.0f, 0.0f );

	if ( depth >= liquidHeight + maxDepth )
	{
		force.y = liquidDensity * volume;
		particle.AddForce ( force );
		return;
	}

	force.y = liquidDensity * volume * ( depth - maxDepth - liquidHeight ) * 0.5f * maxDepth;
	particle.AddForce ( force );
}
