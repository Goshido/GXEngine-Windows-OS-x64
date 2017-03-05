//version 1.0

#include <GXPhysics/GXParticleWorld.h>
#include <GXPhysics/GXParticleContactGenerator.h>
#include <GXCommon/GXMemory.h>

struct GXParticleRegistration
{
	GXParticleRegistration*		next;
	GXParticleRegistration*		prev;

	GXParticle*					particle;
};

//-----------------------------------------------------------------------

struct GXContactGeneratorRegistration
{
	GXContactGeneratorRegistration*	next;
	GXContactGeneratorRegistration*	prev;

	GXParticleContactGenerator*		generator;
};

//-----------------------------------------------------------------------

GXParticleWorld::GXParticleWorld ( GXUInt maxContacts, GXUInt maxContactResolverIterations )
: contactResolver ( maxContactResolverIterations )
{
	this->maxContacts = maxContacts;
	contacts = (GXParticleContact*)malloc ( maxContacts * sizeof ( GXParticleContact ) );

	isCalculateIterations = maxContactResolverIterations == 0;
}

GXParticleWorld::~GXParticleWorld ()
{
	free ( contacts );
}

GXVoid GXParticleWorld::BeginFrame ()
{
	for ( GXParticleRegistration* p = particles; p; p = p->next )
		p->particle->ClearForceAccumulator ();
}

GXUInt GXParticleWorld::GenerateContacts ()
{
	GXUInt limit = maxContacts;
	GXParticleContact* nextContact = contacts;

	for ( GXContactGeneratorRegistration* p = contactGenerators; p; p = p->next )
	{
		GXUInt used = p->generator->AddContact ( nextContact, limit );
		limit -= used;
		nextContact += used;

		if ( limit <= 0 ) break;
	}

	return maxContacts - limit;
}

GXVoid GXParticleWorld::Integrate ( GXFloat deltaTime )
{
	for ( GXParticleRegistration* p = particles; p; p = p->next )
		p->particle->Integrate ( deltaTime );
}

GXVoid GXParticleWorld::RunPhysics ( GXFloat deltaTime )
{
	forceRegistry.UpdateForces ( deltaTime );
	Integrate ( deltaTime );
	GXUInt usedContacts = GenerateContacts ();

	if ( isCalculateIterations )
		contactResolver.SetMaxIterations ( usedContacts * 2 );

	contactResolver.ResolveContacts ( contacts, usedContacts, deltaTime );
}
