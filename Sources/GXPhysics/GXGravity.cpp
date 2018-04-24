// version 1.1

#include <GXPhysics/GXGravity.h>


GXGravity::GXGravity ( const GXVec3 &gravity )
{
	this->gravity = gravity;
}

GXGravity::~GXGravity ()
{
	// NOTHING
}

GXVoid GXGravity::UpdateForce ( GXRigidBody &body, GXFloat /*deltaTime*/ )
{
	if ( body.IsKinematic () ) return;

	GXVec3 force;
	force.Multiply ( gravity, body.GetMass () );

	body.AddForce ( force );
}
