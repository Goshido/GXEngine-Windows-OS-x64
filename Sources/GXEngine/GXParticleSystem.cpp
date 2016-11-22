//version 1.1

#include <GXEngine/GXParticleSystem.h>
#include <GXEngineDLL/GXPhysX.h>


GXVoid GXParticleSystemParams::SetDefault ()
{
	maxParticles = 200;
	lifespan = 1.0f;
	lifespanDeviation = 0.3f;
	maxPerFrameDistance = 20.0f;

	startVelocity = GXCreateVec3 ( 10.0f, 1.0f, 1.0f );
	perFrameAcceleration = GXCreateVec3 ( 0.0f, GX_PHYSICS_GRAVITY_FACTOR, 0.0f );
}