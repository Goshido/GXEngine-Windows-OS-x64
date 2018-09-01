// version 1.3

#include <GXPhysics/GXPhysicsEngine.h>
#include <GXCommon/GXTime.h>

#define DEFAULT_SLEEP_TIMEOUT								0.2f
#define DEFAULT_MAXIMUM_LOCATION_CHANGE_SQUARED_DEVIATION	2.0e-5f
#define DEFAULT_MAXIMUM_ROTATION_CHANGE_SQUARED_DEVIATION	1.5e-5f
#define DEFAULT_TIME_STEP									0.016f	// 60 iterations per second
#define DEFAULT_TIME_MULTIPLIER								1.0f

#define MAX_CONTACTS										16384
#define WORLD_ITERATIONS									50
#define THREAD_IDLE											5u		// simulate over 200 iterations per second is not practical


GXPhysicsEngine* GXPhysicsEngine::instance = nullptr;

GXPhysicsEngine::~GXPhysicsEngine ()
{
	loopFlag = GX_FALSE;
	thread->Join ();
	delete thread;
}

GXPhysicsEngine& GXPhysicsEngine::GetInstance ()
{
	if ( !instance )
		instance = new GXPhysicsEngine ();

	return *instance;
}

GXVoid GXPhysicsEngine::SetSleepTimeout ( GXFloat seconds )
{
	sleepTimeout = seconds;
}

GXFloat GXPhysicsEngine::GetSleepTimeout () const
{
	return sleepTimeout;
}

GXVoid GXPhysicsEngine::SetMaximumLocationChangeSquaredDeviation ( GXFloat squaredDeviation )
{
	maximumLocationChangeSquaredDeviation = squaredDeviation;
}

GXFloat GXPhysicsEngine::GetMaximumLocationChangeSquaredDeviation () const
{
	return maximumLocationChangeSquaredDeviation;
}

GXVoid GXPhysicsEngine::SetMaximumRotationChangeSquaredDeviation ( GXFloat squaredDeviation )
{
	maximumRotationChangeSquaredDeviation = squaredDeviation;
}

GXFloat GXPhysicsEngine::GetMaximumRotationChangeSquaredDeviation () const
{
	return maximumRotationChangeSquaredDeviation;
}

GXVoid GXPhysicsEngine::SetTimeStep ( GXFloat step )
{
	timeStep = step;
	adjustedTimeStep = static_cast<GXDouble> ( step * timeMultiplier );
}

GXFloat GXPhysicsEngine::GetTimeStep () const
{
	return timeStep;
}

GXVoid GXPhysicsEngine::SetTimeMultiplier ( GXFloat multiplier )
{
	timeMultiplier = static_cast<GXDouble> ( multiplier );
	adjustedTimeStep = static_cast<GXDouble> ( timeStep * multiplier );
}

GXWorld& GXPhysicsEngine::GetWorld ()
{
	return world;
}

GXVoid GXPhysicsEngine::Start ()
{
	thread->Start ();
}

GXPhysicsEngine::GXPhysicsEngine ():
	world ( MAX_CONTACTS, WORLD_ITERATIONS ),
	time ( 0.0f ),
	loopFlag ( GX_TRUE )
{
	thread = new GXThread ( &GXPhysicsEngine::Simulate, this );
	SetSleepTimeout ( DEFAULT_SLEEP_TIMEOUT );
	SetMaximumLocationChangeSquaredDeviation ( DEFAULT_MAXIMUM_LOCATION_CHANGE_SQUARED_DEVIATION );
	SetMaximumRotationChangeSquaredDeviation ( DEFAULT_MAXIMUM_ROTATION_CHANGE_SQUARED_DEVIATION );
	SetTimeStep ( DEFAULT_TIME_STEP );
	SetTimeMultiplier ( DEFAULT_TIME_MULTIPLIER );
}

GXUPointer GXTHREADCALL GXPhysicsEngine::Simulate ( GXVoid* argument, GXThread &thread )
{
	GXPhysicsEngine* physicsEngine = static_cast<GXPhysicsEngine*> ( argument );

	GXDouble tickToSeconds = 1.0 / GXGetProcessorTicksPerSecond ();
	GXDouble lastTicks = GXGetProcessorTicks ();
	GXDouble time = 0.0;

	while ( physicsEngine->loopFlag )
	{
		GXDouble currentTicks = GXGetProcessorTicks ();
		time += ( currentTicks - lastTicks ) * tickToSeconds * physicsEngine->timeMultiplier;
		lastTicks = currentTicks;

		while ( time > physicsEngine->adjustedTimeStep )
		{
			physicsEngine->world.RunPhysics ( static_cast<GXFloat> ( physicsEngine->adjustedTimeStep ) );
			time -= physicsEngine->adjustedTimeStep;
		}

		thread.Sleep ( THREAD_IDLE );
	}

	return 0u;
}
