// version 1.3

#include <GXPhysics/GXGravity.h>


GXGravity::GXGravity ( const GXVec3 &gravity ):
    _gravity ( gravity )
{
    // NOTHING
}

GXGravity::~GXGravity ()
{
    // NOTHING
}

GXVoid GXGravity::UpdateForce ( GXRigidBody &body, GXFloat /*deltaTime*/ )
{
    if ( body.IsKinematic () ) return;

    GXVec3 force;
    force.Multiply ( _gravity, body.GetMass () );

    body.AddForce ( force );
}
