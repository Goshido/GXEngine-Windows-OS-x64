// version 1.4

#include <GXPhysics/GXPhysicsEngine.h>
#include <GXCommon/GXTime.h>


#define DEFAULT_SLEEP_TIMEOUT                                   0.2f
#define DEFAULT_MAXIMUM_LOCATION_CHANGE_SQUARED_DEVIATION       2.0e-5f
#define DEFAULT_MAXIMUM_ROTATION_CHANGE_SQUARED_DEVIATION       1.5e-5f
#define DEFAULT_TIME_STEP                                       0.016f      // 60 iterations per second
#define DEFAULT_TIME_MULTIPLIER                                 1.0f

#define MAX_CONTACTS                                            16384
#define WORLD_ITERATIONS                                        50
#define THREAD_IDLE                                             5u          // simulate over 200 iterations per second is not practical

//---------------------------------------------------------------------------------------------------------------------

GXPhysicsEngine* GXPhysicsEngine::_instance = nullptr;

GXPhysicsEngine& GXPhysicsEngine::GetInstance ()
{
    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXPhysicsEngine" )
        _instance = new GXPhysicsEngine ();
    }

    return *_instance;
}

GXPhysicsEngine::~GXPhysicsEngine ()
{
    _loopFlag = GX_FALSE;
    _thread->Join ();
    delete _thread;
}

GXVoid GXPhysicsEngine::SetSleepTimeout ( GXFloat seconds )
{
    _sleepTimeout = seconds;
}

GXFloat GXPhysicsEngine::GetSleepTimeout () const
{
    return _sleepTimeout;
}

GXVoid GXPhysicsEngine::SetMaximumLocationChangeSquaredDeviation ( GXFloat squaredDeviation )
{
    _maximumLocationChangeSquaredDeviation = squaredDeviation;
}

GXFloat GXPhysicsEngine::GetMaximumLocationChangeSquaredDeviation () const
{
    return _maximumLocationChangeSquaredDeviation;
}

GXVoid GXPhysicsEngine::SetMaximumRotationChangeSquaredDeviation ( GXFloat squaredDeviation )
{
    _maximumRotationChangeSquaredDeviation = squaredDeviation;
}

GXFloat GXPhysicsEngine::GetMaximumRotationChangeSquaredDeviation () const
{
    return _maximumRotationChangeSquaredDeviation;
}

GXVoid GXPhysicsEngine::SetTimeStep ( GXFloat step )
{
    _timeStep = step;
    _adjustedTimeStep = static_cast<GXDouble> ( step * _timeMultiplier );
}

GXFloat GXPhysicsEngine::GetTimeStep () const
{
    return _timeStep;
}

GXVoid GXPhysicsEngine::SetTimeMultiplier ( GXFloat multiplier )
{
    _timeMultiplier = static_cast<GXDouble> ( multiplier );
    _adjustedTimeStep = static_cast<GXDouble> ( _timeStep * multiplier );
}

GXWorld& GXPhysicsEngine::GetWorld ()
{
    return _world;
}

GXVoid GXPhysicsEngine::Start ()
{
    _thread->Start ();
}

GXPhysicsEngine::GXPhysicsEngine ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXPhysicsEngine" )
    _world ( MAX_CONTACTS, WORLD_ITERATIONS ),
    _time ( 0.0f ),
    _loopFlag ( GX_TRUE )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXThread" )
    _thread = new GXThread ( &GXPhysicsEngine::Simulate, this );

    SetSleepTimeout ( DEFAULT_SLEEP_TIMEOUT );
    SetMaximumLocationChangeSquaredDeviation ( DEFAULT_MAXIMUM_LOCATION_CHANGE_SQUARED_DEVIATION );
    SetMaximumRotationChangeSquaredDeviation ( DEFAULT_MAXIMUM_ROTATION_CHANGE_SQUARED_DEVIATION );
    SetTimeStep ( DEFAULT_TIME_STEP );
    SetTimeMultiplier ( DEFAULT_TIME_MULTIPLIER );
}

GXUPointer GXTHREADCALL GXPhysicsEngine::Simulate ( GXVoid* argument, GXThread &thread )
{
    GXPhysicsEngine* physicsEngine = static_cast<GXPhysicsEngine*> ( argument );

    const GXDouble tickToSeconds = 1.0 / GXGetProcessorTicksPerSecond ();
    GXDouble lastTicks = GXGetProcessorTicks ();
    GXDouble time = 0.0;

    while ( physicsEngine->_loopFlag )
    {
        const GXDouble currentTicks = GXGetProcessorTicks ();
        time += ( currentTicks - lastTicks ) * tickToSeconds * physicsEngine->_timeMultiplier;
        lastTicks = currentTicks;

        while ( time > physicsEngine->_adjustedTimeStep )
        {
            physicsEngine->_world.RunPhysics ( static_cast<GXFloat> ( physicsEngine->_adjustedTimeStep ) );
            time -= physicsEngine->_adjustedTimeStep;
        }

        thread.Sleep ( THREAD_IDLE );
    }

    return 0u;
}
