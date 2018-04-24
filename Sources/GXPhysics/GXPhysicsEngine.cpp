// version 1.1

#include <GXPhysics/GXPhysicsEngine.h>

#define DEFAULT_SLEEP_TIMEOUT								0.2f
#define DEFAULT_MAXIMUM_LOCATION_CHANGE_SQUARED_DEVIATION	2.0e-5f
#define DEFAULT_MAXIMUM_ROTATION_CHANGE_SQUARED_DEVIATION	1.5e-5f
#define DEFAULT_TIME_STEP									0.016f	// 60 iterations per second
#define DEFAULT_TIME_MULTIPLIER								1.0f

#define MAX_CONTACTS										16384
#define WORLD_ITERATIONS									50


GXPhysicsEngine* GXPhysicsEngine::instance = nullptr;

GXPhysicsEngine::~GXPhysicsEngine ()
{
	// NOTHING
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
	adjustedTimeStep = step * timeMultiplier;
}

GXFloat GXPhysicsEngine::GetTimeStep () const
{
	return timeStep;
}

GXVoid GXPhysicsEngine::SetTimeMultiplier ( GXFloat multiplier )
{
	timeMultiplier = multiplier;
	adjustedTimeStep = timeStep * multiplier;
}

GXWorld& GXPhysicsEngine::GetWorld ()
{
	return world;
}

GXVoid GXPhysicsEngine::RunSimulateLoop ( GXFloat deltaTime )
{
	time += deltaTime * timeMultiplier;

	while ( time > adjustedTimeStep )
	{
		world.RunPhysics ( adjustedTimeStep );
		time -= adjustedTimeStep;
	}
}

GXPhysicsEngine::GXPhysicsEngine ():
	world ( MAX_CONTACTS, WORLD_ITERATIONS ),
	time ( 0.0f )
{
	SetSleepTimeout ( DEFAULT_SLEEP_TIMEOUT );
	SetMaximumLocationChangeSquaredDeviation ( DEFAULT_MAXIMUM_LOCATION_CHANGE_SQUARED_DEVIATION );
	SetMaximumRotationChangeSquaredDeviation ( DEFAULT_MAXIMUM_ROTATION_CHANGE_SQUARED_DEVIATION );
	SetTimeStep ( DEFAULT_TIME_STEP );
	SetTimeMultiplier ( DEFAULT_TIME_MULTIPLIER );
}
