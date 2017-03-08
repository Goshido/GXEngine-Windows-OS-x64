//version 1.0

#include <GXPhysics/GXPhysicsEngine.h>


#define DEFAULT_GRAVITY_X		0.0f
#define DEFAULT_GRAVITY_Y		-9.8f
#define DEFAULT_GRAVITY_Z		0.0f

#define DEFAULT_SLEEP_EPSILON	0.1f

#define MAX_CONTACTS			16384
#define WORLD_ITERATIONS		50


GXPhysicsEngine* GXPhysicsEngine::instance = nullptr;

GXPhysicsEngine::~GXPhysicsEngine ()
{
	//NOTHING
}

GXPhysicsEngine* GXPhysicsEngine::GetInstance ()
{
	if ( !instance )
		instance = new GXPhysicsEngine ();

	return instance;
}

GXVoid GXPhysicsEngine::SetGravity ( GXFloat x, GXFloat y, GXFloat z )
{
	gravity = GXCreateVec3 ( x, y, z );
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

GXWorld& GXPhysicsEngine::GetWorld ()
{
	return world;
}

GXVoid GXPhysicsEngine::RunSimulateLoop ( GXFloat deltaTime )
{
	world.RunPhysics ( deltaTime );
}

GXPhysicsEngine::GXPhysicsEngine ()
: world ( MAX_CONTACTS, WORLD_ITERATIONS )
{
	SetGravity ( DEFAULT_GRAVITY_X, DEFAULT_GRAVITY_Y, DEFAULT_GRAVITY_Z );
	SetSleepEpsilon ( DEFAULT_SLEEP_EPSILON );
}
