// version 1.4

#ifndef GX_PHYSICS_ENGINE
#define GX_PHYSICS_ENGINE


#include "GXWorld.h"
#include <GXCommon/GXThread.h>


class GXPhysicsEngine final : public GXMemoryInspector
{
    private:
        GXWorld                     _world;
        GXFloat                     _time;
        GXBool                      _loopFlag;

        GXThread*                   _thread;

        GXFloat                     _timeStep;
        GXDouble                    _adjustedTimeStep;
        GXDouble                    _timeMultiplier;
        GXFloat                     _sleepTimeout;
        GXFloat                     _maximumLocationChangeSquaredDeviation;
        GXFloat                     _maximumRotationChangeSquaredDeviation;

        static GXPhysicsEngine*     _instance;

    public:
        static GXPhysicsEngine& GetInstance ();
        ~GXPhysicsEngine ();

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
