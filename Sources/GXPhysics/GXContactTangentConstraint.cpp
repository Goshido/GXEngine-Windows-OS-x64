// version 1.0

#include <GXPhysics/GXContactTangentConstraint.h>


#define CONSTRAINT_BIAS	0.0f


GXContactTangentConstraint::GXContactTangentConstraint ( GXContact &contact, GXUInt linkedContacts )
{
	// Based on
	// http://allenchou.net/2013/12/game-physics-resolution-contact-constraints/

	const GXVec3& contactPoint = contact.GetContactPoint ();
	const GXRigidBody& firstBody = contact.GetFirstRigidBody ();
	const GXRigidBody& secondBody = contact.GetSecondRigidBody ();
	const GXVec3& tangent = contact.GetTangent ();
	const GXVec3& normal = contact.GetNormal ();
	
	GXVec3 toPoint;
	toPoint.Substract ( contactPoint, firstBody.GetLocation () );

	GXVec3 tmp;
	tmp.CrossProduct ( toPoint, tangent );

	memcpy ( jacobian[ 0 ].data, &tangent, sizeof ( GXVec3 ) );
	memcpy ( jacobian[ 0 ].data + 3, &tmp, sizeof ( GXVec3 ) );

	toPoint.Substract ( contactPoint, secondBody.GetLocation () );

	GXVec3 reverseTangent ( tangent );
	reverseTangent.Reverse ();

	tmp.CrossProduct ( toPoint, reverseTangent );

	memcpy ( jacobian[ 1 ].data, &reverseTangent, sizeof ( GXVec3 ) );
	memcpy ( jacobian[ 1 ].data + 3, &tmp, sizeof ( GXVec3 ) );

	bias = CONSTRAINT_BIAS;

	// TODO find solution
	
	GXFloat alpha = fabsf ( contact.GetStaticFriction () * normal.DotProduct ( firstBody.GetTotalForce () ) ) / (GXFloat)linkedContacts;
	lambdaRange.Init ( -alpha, alpha );
}

GXContactTangentConstraint::~GXContactTangentConstraint ()
{
	// NOTHING
}
