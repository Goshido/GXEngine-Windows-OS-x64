//version 1.0

#ifndef GX_PARTICLE_WORLD
#define GX_PARTICLE_WORLD


#include "GXParticleForceRegistry.h"
#include "GXParticleContactResolver.h"


struct GXParticleRegistration;
struct GXContactGeneratorRegistration;
class GXParticleWorld
{
	private:
		GXParticleRegistration*			particles;
		GXParticleForceRegistry			forceRegistry;
		GXParticleContactResolver		contactResolver;
		GXContactGeneratorRegistration*	contactGenerators;
		GXParticleContact*				contacts;

		GXUInt							maxContacts;
		bool							isCalculateIterations;

	public:
		GXParticleWorld ( GXUInt maxContacts, GXUInt maxContactResolverIterations );
		~GXParticleWorld ();

		GXVoid BeginFrame ();
		GXUInt GenerateContacts ();
		GXVoid Integrate ( GXFloat deltaTime );
		GXVoid RunPhysics ( GXFloat deltaTime );
};


#endif GX_PARTICLE_WORLD