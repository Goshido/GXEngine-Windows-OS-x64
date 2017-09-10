//version 1.1

#include <GXPhysics/GXContactResolver.h>
#include <GXCommon/GXLogger.h>


GXContactResolver::GXContactResolver ( GXUInt /*iterations*/ )
{
	//NOTHING
}

GXVoid GXContactResolver::ResolveContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat /*deltaTime*/ )
{
	GXUInt i = 0;
	while ( i < numContacts )
	{
		GXContact& contact = contactArray[ i ];

		GXUInt linkedContacts = contact.GetLinkedContacts ();
		GXRigidBody& firstRigidBody = contact.GetFirstRigidBody ();
		GXRigidBody& secondRigidBody = contact.GetSecondRigidBody ();

		if ( firstRigidBody.IsKinematic () )
			ResolveSingleBodyContacts ( secondRigidBody, contactArray + i );
		else if ( secondRigidBody.IsKinematic () )
			ResolveSingleBodyContacts ( firstRigidBody, contactArray + i );
		else
			ResolveDoubleBodyContacts ( contactArray + 1 );

		i += linkedContacts;

		if ( contact.GetPenetration () > 100.0f )
		{
			GXUInt wtf = 0;
			GXLogW ( L"%u\n", wtf );
		}

		if ( firstRigidBody.IsKinematic () )
		{
			GXVec3 newLocation;
			newLocation.Sum ( secondRigidBody.GetLocation (), contact.GetPenetration (), contact.GetNormal () );
			secondRigidBody.SetLocation ( newLocation );
		}
		else
		{
			GXVec3 reverseContactNormal = contact.GetNormal ();
			reverseContactNormal.Reverse ();

			GXVec3 newLocation;
			newLocation.Sum ( firstRigidBody.GetLocation (), contact.GetPenetration (), reverseContactNormal );
			firstRigidBody.SetLocation ( newLocation );
		}
	}
}

GXVoid GXContactResolver::CalculateContactMatrix ( GXMat3 &out, const GXVec3 &contactNormal )
{
	GXVec3 tangent;
	GXVec3 bitangent;

	GXVec3 alpha;

	if ( fabsf ( contactNormal.GetY () ) > fabsf ( contactNormal.GetX () ) && fabsf ( contactNormal.GetY () ) > fabsf ( contactNormal.GetZ () ) )
		alpha = GXVec3::GetAbsoluteX ();
	else
		alpha = GXVec3::GetAbsoluteY ();

	tangent.CrossProduct ( alpha, contactNormal );
	tangent.Normalize ();
	bitangent.CrossProduct ( contactNormal, tangent );

	out.SetX ( tangent );
	out.SetY ( bitangent );
	out.SetZ ( contactNormal );
}

GXFloat GXContactResolver::CalculateAngularComponentContactSpace ( const GXVec3 &centerOfMassToContactPoint, const GXRigidBody& rigidBody, const GXVec3 &contactNormal )
{
	GXVec3 angularMomentum;
	angularMomentum.CrossProduct ( centerOfMassToContactPoint, contactNormal );

	GXVec3 angularVelocity;
	rigidBody.GetInverseInertiaTensorWorld ().Multiply ( angularVelocity, angularMomentum );

	GXVec3 velocity;
	velocity.CrossProduct ( angularVelocity, centerOfMassToContactPoint );

	return velocity.DotProduct ( contactNormal );
}

GXVoid GXContactResolver::ResolveSingleBodyContacts ( GXRigidBody &rigidBody, GXContact* contacts )
{
	GXVec3 contactNormal = contacts[ 0 ].GetNormal ();
	if ( &rigidBody == &contacts[ 0 ].GetFirstRigidBody () )
		contactNormal.Reverse ();

	GXMat3 contactMatrix;
	CalculateContactMatrix ( contactMatrix, contactNormal );

	GXMat3 inverseContactMatrix;
	inverseContactMatrix.Transponse ( contactMatrix );

	GXUInt linkedContacts = contacts[ 0 ].GetLinkedContacts ();

	GXVec3 totalLinearVelocity ( 0.0f, 0.0f, 0.0f );
	GXVec3 totalAngularVelocity ( 0.0f, 0.0f, 0.0f );

	for ( GXUInt i = 0; i < linkedContacts; i++ )
	{
		GXContact& contact = contacts[ i ];

		GXFloat linearComponent = rigidBody.GetInverseMass ();

		GXVec3 rigidBodyCenterOfMassToContactPoint;
		rigidBodyCenterOfMassToContactPoint.Substract ( contact.GetContactPoint (), rigidBody.GetLocation () );

		GXFloat deltaVelocity = linearComponent + CalculateAngularComponentContactSpace ( rigidBodyCenterOfMassToContactPoint, rigidBody, contactNormal );

		GXVec3 contactVelocity;
		contactVelocity.CrossProduct ( rigidBody.GetAngularVelocity (), rigidBodyCenterOfMassToContactPoint );
		contactVelocity.Sum ( contactVelocity, rigidBody.GetLinearVelocity () );

		GXVec3 contactVelocityContactSpace;
		inverseContactMatrix.Multiply ( contactVelocityContactSpace, contactVelocity );

		if ( &rigidBody == &contact.GetFirstRigidBody () )
			contactVelocityContactSpace.Reverse ();

		GXFloat desiredVelocity = -contactVelocityContactSpace.GetZ () * ( 1.0f + contact.GetRestitution () );

		GXVec3 impulse;
		impulse.Multiply ( contactNormal, desiredVelocity / deltaVelocity );

		if ( &rigidBody == &contact.GetFirstRigidBody () )
			impulse.Reverse ();

		GXVec3 linearVelocity;
		GXVec3 angularVelocity;
		GetRigidBodyKinematics ( linearVelocity, angularVelocity, rigidBody, impulse, rigidBodyCenterOfMassToContactPoint );

		totalLinearVelocity.Sum ( totalLinearVelocity, linearVelocity );
		totalAngularVelocity.Sum ( totalAngularVelocity, angularVelocity );
	}

	GXFloat inverseLinkedContacts = 1.0f / (GXFloat)linkedContacts;

	totalLinearVelocity.Multiply ( totalLinearVelocity, inverseLinkedContacts );
	totalAngularVelocity.Multiply ( totalAngularVelocity, inverseLinkedContacts );

	rigidBody.SetLinearVelocity ( totalLinearVelocity );
	rigidBody.SetAngularVelocity ( totalAngularVelocity );
}

GXVoid GXContactResolver::ResolveDoubleBodyContacts ( GXContact* /*contacts*/ )
{
	GXUInt todo = 0;
	GXLogW ( L"%u\n", todo );
	//TODO
}

GXVoid GXContactResolver::GetRigidBodyKinematics ( GXVec3 &linearVelocity, GXVec3 &angularVelocity, GXRigidBody &rigidBody, const GXVec3 &impulse, const GXVec3 &centerOfMassToContactPoint )
{
	GXVec3 newVelocity;
	linearVelocity.Sum ( rigidBody.GetLinearVelocity (), rigidBody.GetInverseMass (), impulse );

	GXVec3 angularMomentum;
	angularMomentum.CrossProduct ( impulse, centerOfMassToContactPoint );

	GXVec3 deltaAngularVelocity;
	rigidBody.GetInverseInertiaTensorWorld ().Multiply ( deltaAngularVelocity, angularMomentum );

	angularVelocity.Sum ( rigidBody.GetAngularVelocity (), deltaAngularVelocity );
}
