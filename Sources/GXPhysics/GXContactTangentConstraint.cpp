// version 1.2

#include <GXPhysics/GXContactTangentConstraint.h>


#define CONSTRAINT_BIAS	0.0f


GXContactTangentConstraint::GXContactTangentConstraint ( GXContact &contact, GXFloat inverseLinkedContacts, GXFloat squareThreshold )
{
	const GXVec3& contactPoint = contact.GetContactPoint ();
	const GXRigidBody& firstBody = contact.GetFirstRigidBody ();
	const GXRigidBody& secondBody = contact.GetSecondRigidBody ();
	const GXVec3& tangent = contact.GetTangent ();
	const GXVec3& normal = contact.GetNormal ();
	
	GXVec3 toPoint;
	toPoint.Substract ( contactPoint, firstBody.GetLocation () );

	GXVec3 tmp;
	tmp.CrossProduct ( toPoint, tangent );

	memcpy ( jacobian[ 0 ]._data, &tangent, sizeof ( GXVec3 ) );
	memcpy ( jacobian[ 0 ]._data + 3, &tmp, sizeof ( GXVec3 ) );

	GXVec3 relativeVelocity ( firstBody.GetLinearVelocity () );
	tmp.CrossProduct ( firstBody.GetAngularVelocity (), toPoint );
	relativeVelocity.Sum ( relativeVelocity, tmp );

	toPoint.Substract ( contactPoint, secondBody.GetLocation () );

	GXVec3 reverseTangent ( tangent );
	reverseTangent.Reverse ();

	tmp.CrossProduct ( toPoint, reverseTangent );

	memcpy ( jacobian[ 1 ]._data, &reverseTangent, sizeof ( GXVec3 ) );
	memcpy ( jacobian[ 1 ]._data + 3, &tmp, sizeof ( GXVec3 ) );

	relativeVelocity.Substract ( relativeVelocity, secondBody.GetLinearVelocity () );
	tmp.CrossProduct ( secondBody.GetAngularVelocity (), toPoint );
	relativeVelocity.Substract ( relativeVelocity, tmp );

	bias = CONSTRAINT_BIAS;

	GXMat3 toContactSpace;
	toContactSpace.SetX ( tangent );
	toContactSpace.SetY ( contact.GetBitangent () );
	toContactSpace.SetZ ( normal );

	toContactSpace.MultiplyMatrixVector ( tmp, relativeVelocity );

	GXVec2 planarVelocity ( tmp._data[ 0 ], tmp._data[ 1 ] );
	GXFloat friction;

	if ( planarVelocity.SquaredLength () < squareThreshold )
		friction = contact.GetStaticFriction ();
	else
		friction = contact.GetDynamicFriction ();

	GXFloat limit = fabsf ( friction * normal.DotProduct ( firstBody.GetTotalForce () ) * inverseLinkedContacts );
	lambdaRange.Init ( -limit, limit );
}

GXContactTangentConstraint::~GXContactTangentConstraint ()
{
	// NOTHING
}
