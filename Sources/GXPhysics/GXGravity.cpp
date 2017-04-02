//version 1.0

#include <GXPhysics/GXGravity.h>

GXGravity::GXGravity ( const GXVec3 &gravity )
{
	this->gravity = gravity;
}

GXVoid GXGravity::UpdateForce ( GXRigidBody &body, GXFloat deltaTime )
{
	if ( !body.HasFiniteMass () ) return;

	GXVec3 force;
	GXMulVec3Scalar ( force, gravity, body.GetMass () );

	body.AddForce ( force );
}
