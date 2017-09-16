//version 1.1

#ifndef GX_PHYSICS_ENGINE
#define GX_PHYSICS_ENGINE


#include "GXWorld.h"


class GXPhysicsEngine
{
	private:
		static GXPhysicsEngine*	instance;

		GXWorld					world;
		GXFloat					sleepTimeout;
		GXFloat					maximumLinearVelocitySquaredDeviation;
		GXFloat					maximumAngularVelocitySquaredDeviation;
		GXFloat					timeStep;
		GXFloat					time;

	public:
		~GXPhysicsEngine ();
		static GXPhysicsEngine& GetInstance ();

		//Timeout when rigid body intergation is considered to be in sleep state.
		//Timeout started when rigid body linear and angular velocity is near
		//to zero values. Timeout restarted after linear or angular velocity
		//squared magnitude more than maximum squared deviation values.
		//Rigid body integrator must use this value. Seconds. s.
		GXVoid SetSleepTimeout ( GXFloat seconds );
		GXFloat GetSleepTimeout () const;

		//Maximum linear velocity squared deviation. Value is used to move rigid body
		//intergation to sleep state. Rigid body integrator must use this value.
		//Dimensionless quantity.
		GXVoid SetMaximumLinearVelocitySquaredDeviation ( GXFloat squaredDeviation );
		GXFloat GetMaximumLinearVelocitySquaredDeviation () const;

		//Maximum angular velocity squared deviation. Value is used to move rigid body
		//intergation to sleep state. Rigid body integrator must use this value.
		//Dimensionless quantity.
		GXVoid SetMaximumAngularVelocitySquaredDeviation ( GXFloat squaredDeviation );
		GXFloat GetMaximumAngularelocitySquaredDeviation () const;

		//Fixed physics simulation time step. Value makes sense when simulation thread
		//does not take processor time for long period. When simulation thead takes
		//processor time it simulates all wasted simulation loops using this value.
		//Seconds. s.
		GXVoid SetTimeStep ( GXFloat step );
		GXFloat GetTimeStep () const;

		GXWorld& GetWorld ();

		GXVoid RunSimulateLoop ( GXFloat deltaTime );

	private:
		GXPhysicsEngine ();
};


#endif //GX_PHYSICS_ENGINE
