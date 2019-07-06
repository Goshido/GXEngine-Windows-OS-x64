// version 1.3

#include <GXPhysics/GXContactFrictionlessConstraint.h>
#include <GXPhysics/GXPhysicsEngine.h>


GXContactFrictionlessConstraint::GXContactFrictionlessConstraint ( GXContact &contact, GXFloat baumgarteFactor )
{
    // Based on
    // http://allenchou.net/2013/12/game-physics-resolution-contact-constraints/

    const GXVec3& contactPoint = contact.GetContactPoint ();
    const GXRigidBody& firstBody = contact.GetFirstRigidBody ();
    const GXRigidBody& secondBody = contact.GetSecondRigidBody ();
    const GXVec3& normal = contact.GetNormal ();

    GXVec3 reverseNormal ( normal );
    reverseNormal.Reverse ();

    GXVec3 toPoint;
    toPoint.Substract ( secondBody.GetLocation (), contactPoint );

    GXVec3 tmp;
    tmp.CrossProduct ( toPoint, reverseNormal );

    memcpy ( _jacobian[ 1u ]._data, &reverseNormal, sizeof ( GXVec3 ) );
    memcpy ( _jacobian[ 1u ]._data + 3u, &tmp, sizeof ( GXVec3 ) );

    tmp.CrossProduct ( secondBody.GetAngularVelocity (), toPoint );

    GXVec3 alpha ( secondBody.GetLinearVelocity () );
    alpha.Sum ( alpha, tmp );

    toPoint.Substract ( contactPoint, firstBody.GetLocation () );
    tmp.CrossProduct ( toPoint, normal );

    memcpy ( _jacobian[ 0u ]._data, &normal, sizeof ( GXVec3 ) );
    memcpy ( _jacobian[ 0u ]._data + 3u, &tmp, sizeof ( GXVec3 ) );

    tmp.CrossProduct ( firstBody.GetAngularVelocity (), toPoint );

    alpha.Substract ( alpha, firstBody.GetLinearVelocity () );
    alpha.Substract ( alpha, tmp );

    _bias = contact.GetRestitution () * normal.DotProduct ( alpha ) - ( baumgarteFactor * contact.GetPenetration () ) / GXPhysicsEngine::GetInstance ().GetTimeStep ();
    _lambdaRange.Init ( 0.0f, FLT_MAX );
}

GXContactFrictionlessConstraint::~GXContactFrictionlessConstraint ()
{
    // NOTHING
}
