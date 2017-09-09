//version 1.0

#include <GXPhysics/GXPhysicsEngine.h>


#define DEFAULT_GRAVITY_X		0.0f
#define DEFAULT_GRAVITY_Y		-9.8f
#define DEFAULT_GRAVITY_Z		0.0f

#define DEFAULT_SLEEP_EPSILON	0.1f
#define DEFAULT_TIME_STEP		0.0155f

#define MAX_CONTACTS			16384
#define WORLD_ITERATIONS		50


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

GXVoid GXPhysicsEngine::SetGravity ( GXFloat x, GXFloat y, GXFloat z )
{
	gravity.Init ( x, y, z );
}

const GXVec3& GXPhysicsEngine::GetGravity () const
{
	return gravity;
}

GXVoid GXPhysicsEngine::SetSleepEpsilon ( GXFloat epsilon )
{
	sleepEpsilon = epsilon;
}

GXFloat GXPhysicsEngine::GetSleepEpsilon () const
{
	return sleepEpsilon;
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
	GXFloat time = deltaTime;
	while ( time > timeStep )
	{
		world.RunPhysics ( timeStep );
		time -= timeStep;
	}

	if ( time > 0.0f )
		world.RunPhysics ( time );
}

GXPhysicsEngine::GXPhysicsEngine ()
: world ( MAX_CONTACTS, WORLD_ITERATIONS )
{
	SetGravity ( DEFAULT_GRAVITY_X, DEFAULT_GRAVITY_Y, DEFAULT_GRAVITY_Z );
	SetTimeStep ( DEFAULT_TIME_STEP );
	SetSleepEpsilon ( DEFAULT_SLEEP_EPSILON );
}
