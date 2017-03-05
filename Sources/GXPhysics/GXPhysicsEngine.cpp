//version 1.0

#include <GXPhysics/GXPhysicsEngine.h>


#define DEFAULT_GRAVITY_X	0.0f
#define DEFAULT_GRAVITY_Y	-9.8f
#define DEFAULT_GRAVITY_Z	0.0f


GXPhysicsEngine* GXPhysicsEngine::instance = nullptr;

GXPhysicsEngine::~GXPhysicsEngine ()
{
	// NOTHING
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

GXPhysicsEngine::GXPhysicsEngine ()
{
	SetGravity ( DEFAULT_GRAVITY_X, DEFAULT_GRAVITY_Y, DEFAULT_GRAVITY_Z );
}
