//version 1.1

#ifndef GX_PARTICLE_SYSTEM
#define GX_PARTICLE_SYSTEM


#include <GXCommon/GXMath.h>


struct GXParticleSystemParams
{
	GXFloat				lifespan;
	GXFloat				lifespanDeviation;
	GXUInt				maxParticles;
	GXVec3				startVelocity;
	GXVec3				perFrameAcceleration;
	GXFloat				maxPerFrameDistance;

	GXVoid SetDefault ();
};


struct GXIdealParticleSystemParams
{
	GXFloat			lifeTime;
	GXFloat			delayTime;

	GXFloat			deviation;
	GXUInt			maxParticles;
	GXFloat			maxVelocity;
	GXVec3			acceleration;
	GXFloat			size;

	GXVec3*			distribution;
	GXVec3*			startVelocities;
	GXVec3*			startPositions;
	GXFloat*		startLifeTime;
};


#endif	//GX_PARTICLE_SYSTEM