//version 1.1

#include <GXPhysics/GXPhysicsEngine.h>

#define DEFAULT_SLEEP_TIMEOUT								0.05f
#define DEFAULT_MAXIMUM_LINEAR_VELOCITY_SQUARED_DEVIATION	0.05f
#define DEFAULT_MAXIMUM_ANGULAR_VELOCITY_SQUARED_DEVIATION	0.05f
#define DEFAULT_TIME_STEP									0.00833f	//120 iterations per second

#define MAX_CONTACTS										16384
#define WORLD_ITERATIONS									50


GXPhysicsEngine* GXPhysicsEngine::instance = nullptr;

GXPhysicsEngine::~GXPhysicsEngine ()
{
	//NOTHING
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

GXVoid GXPhysicsEngine::SetMaximumLinearVelocitySquaredDeviation ( GXFloat squaredDeviation )
{
	maximumLinearVelocitySquaredDeviation = squaredDeviation;
}

GXFloat GXPhysicsEngine::GetMaximumLinearVelocitySquaredDeviation () const
{
	return maximumLinearVelocitySquaredDeviation;
}

GXVoid GXPhysicsEngine::SetMaximumAngularVelocitySquaredDeviation ( GXFloat squaredDeviation )
{
	maximumAngularVelocitySquaredDeviation = squaredDeviation;
}

GXFloat GXPhysicsEngine::GetMaximumAngularelocitySquaredDeviation () const
{
	return maximumAngularVelocitySquaredDeviation;
}

GXVoid GXPhysicsEngine::SetTimeStep ( GXFloat step )
{
	timeStep = step;
}

GXFloat GXPhysicsEngine::GetTimeStep () const
{
	return timeStep;
}

GXWorld& GXPhysicsEngine::GetWorld ()
{
	return world;
}

GXVoid GXPhysicsEngine::RunSimulateLoop ( GXFloat deltaTime )
{
	time += deltaTime;

	while ( time > timeStep )
	{
		world.RunPhysics ( timeStep );
		time -= timeStep;
	}
}

GXPhysicsEngine::GXPhysicsEngine ():
world ( MAX_CONTACTS, WORLD_ITERATIONS )
{
	time = 0.0f;
	SetSleepTimeout ( DEFAULT_SLEEP_TIMEOUT );
	SetMaximumLinearVelocitySquaredDeviation ( DEFAULT_MAXIMUM_LINEAR_VELOCITY_SQUARED_DEVIATION );
	SetMaximumAngularVelocitySquaredDeviation ( DEFAULT_MAXIMUM_ANGULAR_VELOCITY_SQUARED_DEVIATION );
	SetTimeStep ( DEFAULT_TIME_STEP );
}
