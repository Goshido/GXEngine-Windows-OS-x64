// version 1.4

#include <GXPhysics/GXSpring.h>


GXSpring::GXSpring ( const GXVec3 &connectionPointLocal, GXRigidBody &otherBody, const GXVec3 &otherConnectionPointLocal, GXFloat hardness, GXFloat restLength ):
    _connectionPointLocal ( connectionPointLocal ),
    _otherConnectionPointLocal ( otherConnectionPointLocal ),
    _otherBody ( &otherBody ),
    _hardness ( hardness ),
    _restLength ( restLength )
{
    // NOTHING
}

GXSpring::~GXSpring ()
{
    // NOTHING
}

GXVoid GXSpring::UpdateForce ( GXRigidBody &body, GXFloat /*deltaTime*/ )
{
    GXVec3 a;
    body.TranslatePointToWorld ( a, _connectionPointLocal );

    GXVec3 b;
    _otherBody->TranslatePointToWorld ( b, _otherConnectionPointLocal );

    GXVec3 delta;
    delta.Substract ( a, b );

    const GXFloat stress = fabsf ( delta.Length () - _restLength ) * _hardness;
    delta.Normalize ();

    GXVec3 force;
    force.Multiply ( delta, -stress );

    body.AddForceAtPointWorld ( force, a );
}
