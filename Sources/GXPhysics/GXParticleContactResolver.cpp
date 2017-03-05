//version 1.0

#include <GXPhysics/GXParticleContactResolver.h>


GXParticleContactResolver::GXParticleContactResolver ( GXUInt maxIterations )
{
	SetMaxIterations ( maxIterations );
	iteration = 0;
}

GXVoid GXParticleContactResolver::SetMaxIterations ( GXUInt maxIterations )
{
	this->maxIterations = maxIterations;
}

GXVoid GXParticleContactResolver::ResolveContacts ( GXParticleContact* contacts, GXUInt numContacts, GXFloat deltaTime )
{
	iteration = 0;
	while ( iteration < maxIterations )
	{
		GXFloat max = 0.0f;
		GXUInt maxIndex = numContacts;

		for ( GXUInt i = 0; i < numContacts; i++ )
		{
			GXFloat separationVelocity = contacts[ i ].CalculateSeparatingVelocity ();
			if ( separationVelocity < max )
			{
				max = separationVelocity;
				maxIndex = i;
			}
		}

		contacts[ maxIndex ].Resolve ( deltaTime );
		iteration++;
	}
}
