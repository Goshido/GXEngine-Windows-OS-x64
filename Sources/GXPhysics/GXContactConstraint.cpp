// version 1.0

#include <GXPhysics/GXContactConstraint.h>
#include <GXPhysics/GXPhysicsEngine.h>


GXContactConstraint::GXContactConstraint ( GXContact &contact, GXFloat baumgarteFactor )
{
	GXVec3 normal ( contact.GetNormal () );
	const GXVec3& contactPoint = contact.GetContactPoint ();
	const GXRigidBody& firstBody = contact.GetFirstRigidBody ();
	const GXRigidBody& secondBody = contact.GetSecondRigidBody ();

	GXVec3 toPoint;
	toPoint.Substract ( contactPoint, firstBody.GetLocation () );
	
	GXVec3 v;
	v.CrossProduct ( toPoint, normal );

	memcpy ( jacobian[ 0 ].data, &normal, sizeof ( GXVec3 ) );
	memcpy ( jacobian[ 0 ].data + 3, &v, sizeof ( GXVec3 ) );

	toPoint.Substract ( contactPoint, secondBody.GetLocation () );
	v.CrossProduct ( toPoint, normal );

	v.Reverse ();
	normal.Reverse ();

	memcpy ( jacobian[ 1 ].data, &normal, sizeof ( GXVec3 ) );
	memcpy ( jacobian[ 1 ].data + 3, &v, sizeof ( GXVec3 ) );

	bias = ( -baumgarteFactor * contact.GetPenetration () ) / GXPhysicsEngine::GetInstance ().GetTimeStep ();

	minimumLambda = 0.0f;
	maximumLambda = FLT_MAX;
}

GXContactConstraint::~GXContactConstraint ()
{
	// NOTHING
}
