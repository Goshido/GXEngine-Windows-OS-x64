//version 1.0

#include <GXPhysics/GXParticleForceRegistry.h>


struct GXParticleForceRegistryLink
{
	GXParticleForceRegistryLink*	next;
	GXParticleForceRegistryLink*	prev;

	GXParticle*						particle;
	GXParticleForceGenerator*		forceGenerator;

	GXParticleForceRegistryLink ( GXParticle* particle, GXParticleForceGenerator* forceGenerator );
};

GXParticleForceRegistryLink::GXParticleForceRegistryLink ( GXParticle* particle, GXParticleForceGenerator* forceGenerator )
{
	this->particle = particle;
	this->forceGenerator = forceGenerator;
}

//--------------------------------------------------------------------------------------------------------

GXParticleForceRegistry::GXParticleForceRegistry ()
{
	registrations = nullptr;
}

GXParticleForceRegistry::~GXParticleForceRegistry ()
{
	Clear ();
}

GXVoid GXParticleForceRegistry::Add ( GXParticle &particle, GXParticleForceGenerator &forceGenerator )
{
	GXParticleForceRegistryLink* reg = new GXParticleForceRegistryLink ( &particle, &forceGenerator );
	reg->prev = nullptr;
	if ( registrations )
	{
		reg->next = registrations;
	}

	registrations = reg;
}

GXVoid GXParticleForceRegistry::Remove ( GXParticle &particle, GXParticleForceGenerator &forceGenerator )
{
	for ( GXParticleForceRegistryLink* p = registrations; p; p = p->next )
	{
		if ( p->particle == &particle && p->forceGenerator == &forceGenerator )
		{
			if ( !registrations->next )
			{
				registrations = nullptr;
			}
			else if ( !p->next )
			{
				p->prev->next = p->next;
			}
			else
			{
				p->prev->next = p->next;
				p->next->prev = p->prev;
			}

			delete p;
			return;
		}
	}
}

GXVoid GXParticleForceRegistry::Clear ()
{
	while ( registrations )
	{
		GXParticleForceRegistryLink* reg = registrations;
		registrations = reg->next;

		delete reg;
	}
}

GXVoid GXParticleForceRegistry::UpdateForces ( GXFloat deltaTIme )
{
	for ( GXParticleForceRegistryLink* p = registrations; p; p = p->next )
		p->forceGenerator->UpdateForce ( *p->particle, deltaTIme );
}
