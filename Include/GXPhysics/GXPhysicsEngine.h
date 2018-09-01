// version 1.3

#ifndef GX_PHYSICS_ENGINE
#define GX_PHYSICS_ENGINE


#include "GXWorld.h"
#include <GXCommon/GXThread.h>


class GXPhysicsEngine final
{
	private:
		GXWorld					world;
		GXFloat					time;
		GXBool					loopFlag;

		GXThread*				thread;

		GXFloat					timeStep;
		GXDouble				adjustedTimeStep;
		GXDouble				timeMultiplier;
		GXFloat					sleepTimeout;
		GXFloat					maximumLocationChangeSquaredDeviation;
		GXFloat					maximumRotationChangeSquaredDeviation;

		static GXPhysicsEngine*	instance;

	public:
		~GXPhysicsEngine ();
		static GXPhysicsEngine& GetInstance ();

		// Timeout when rigid body intergation is considered to be in sleep state.
		// Timeout started when rigid body linear and angular velocity is near
		// to zero values. Timeout restarted after linear or angular velocity
		// squared magnitude more than maximum squared deviation values.
		// Rigid body integrator must use this value. Seconds. s.
		GXVoid SetSleepTimeout ( GXFloat seconds );
		GXFloat GetSleepTimeout () const;

		// Maximum location change squared deviation. Value is used to move rigid body
		// intergation to sleep state. Rigid body integrator must use this value.
		// Dimensionless quantity.
		GXVoid SetMaximumLocationChangeSquaredDeviation ( GXFloat squaredDeviation );
		GXFloat GetMaximumLocationChangeSquaredDeviation () const;

		// Maximum rotation change squared deviation. Value is used to move rigid body
		// intergation to sleep state. Rigid body integrator must use this value.
		// Dimensionless quantity.
		GXVoid SetMaximumRotationChangeSquaredDeviation ( GXFloat squaredDeviation );
		GXFloat GetMaximumRotationChangeSquaredDeviation () const;

		// Fixed physics simulation time step. Value makes sense when simulation thread
		// does not take processor time for long period. When simulation thead takes
		// processor time it simulates all wasted simulation loops using this value.
		// Seconds. s.
		GXVoid SetTimeStep ( GXFloat step );
		GXFloat GetTimeStep () const;

		// Value used to speed up or slow down time used in simulation. Value also affects
		// time step value implicitly in RunSimulateLoop method. Dimensionless quantity.
		GXVoid SetTimeMultiplier ( GXFloat multiplier );

		GXWorld& GetWorld ();

		// Start simulation.
		GXVoid Start ();

	private:
		GXPhysicsEngine ();

		static GXUPointer GXTHREADCALL Simulate ( GXVoid* argument, GXThread &thread );

		GXPhysicsEngine ( const GXPhysicsEngine &other ) = delete;
		GXPhysicsEngine& operator = ( const GXPhysicsEngine &other ) = delete;
};


#endif // GX_PHYSICS_ENGINE
