//version 1.0

#ifndef GX_REAL_PARTICLES
#define GX_REAL_PARTICLES


#include "GXParticleSystem.h"
#include "GXMesh.h"
#include "GXPhysics.h"


class GXRealParticles : public GXMesh
{
	private:
		GXFloat				lifespan;
		GXFloat				lifespanDeviation;
		GXUInt				maxParticles;

		PxU32*				particlesInd;
		PxVec3*				particlesPos;
		PxVec3*				particlesVel;

		GXFloat*			particlesLifeLeft;
		GXVec3*				particlesSize;

		GXVec3				startVelocity;
		PxU32*				newPaticlesInd;
		PxVec3*				newPaticlesPos;
		PxVec3*				newPaticlesVel;

		GLuint				mod_view_proj_mat_Location;
		GLuint				particle_size_Location;
		GLuint				life_left_Location;

		GXDword				delta;

		PxParticleSystem*	particleSystem;

	public:
		GXRealParticles ( GXParticleSystemParams &psParams );
		virtual ~GXRealParticles ();

		virtual GXVoid Draw ();
		GXVoid SetVelocityModulus ( GXFloat x, GXFloat z, GXFloat y );

		GXVoid Simulate ( GXDword delta );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		GXVoid UpdateParticleSystem ();
};


#endif	//GX_REAL_PARTICLES