//version 1.1

#include <GXPhysics/GXContactResolver.h>
#include <GXCommon/GXLogger.h>


GXContactResolver::GXContactResolver ( GXUInt iterations )
{
	//NOTHING
}

GXVoid GXContactResolver::ResolveContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime )
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
		}

		if ( firstRigidBody.IsKinematic () )
		{
			GXVec3 newLocation;
			GXSumVec3ScaledVec3 ( newLocation, secondRigidBody.GetLocation (), contact.GetPenetration (), contact.GetNormal () );
			secondRigidBody.SetLocation ( newLocation );
		}
		else
		{
			GXVec3 reverseContactNormal = contact.GetNormal ();
			GXReverseVec3 ( reverseContactNormal );

			GXVec3 newLocation;
			GXSumVec3ScaledVec3 ( newLocation, firstRigidBody.GetLocation (), contact.GetPenetration (), reverseContactNormal );
			firstRigidBody.SetLocation ( newLocation );
		}
	}
}

GXVoid GXContactResolver::CalculateContactMatrix ( GXMat3 &out, const GXVec3 &contactNormal )
{
	GXVec3 tangent;
	GXVec3 bitangent;

	GXVec3 alpha;

	if ( fabsf ( contactNormal.y ) > fabsf ( contactNormal.x ) && fabsf ( contactNormal.y ) > fabsf ( contactNormal.z ) )
		alpha = GXVec3::GetAbsoluteX ();
	else
		alpha = GXVec3::GetAbsoluteY ();

	GXCrossVec3Vec3 ( tangent, alpha, contactNormal );
	GXNormalizeVec3 ( tangent );
	GXCrossVec3Vec3 ( bitangent, contactNormal, tangent );

	out.SetX ( tangent );
	out.SetY ( bitangent );
	out.SetZ ( contactNormal );
}

GXFloat GXContactResolver::CalculateAngularComponentContactSpace ( const GXVec3 &centerOfMassToContactPoint, const GXRigidBody& rigidBody, const GXVec3 &contactNormal )
{
	GXVec3 angularMomentum;
	GXCrossVec3Vec3 ( angularMomentum, centerOfMassToContactPoint, contactNormal );

	GXVec3 angularVelocity;
	GXMulVec3Mat3 ( angularVelocity, angularMomentum, rigidBody.GetInverseInertiaTensorWorld () );

	GXVec3 velocity;
	GXCrossVec3Vec3 ( velocity, angularVelocity, centerOfMassToContactPoint );

	return GXDotVec3 ( velocity, contactNormal );
}

GXVoid GXContactResolver::ResolveSingleBodyContacts ( GXRigidBody &rigidBody, GXContact* contacts )
{
	GXVec3 contactNormal = contacts[ 0 ].GetNormal ();
	if ( &rigidBody == &contacts[ 0 ].GetFirstRigidBody () )
		GXReverseVec3 ( contactNormal );

	GXMat3 contactMatrix;
	CalculateContactMatrix ( contactMatrix, contactNormal );

	GXMat3 inverseContactMatrix;
	GXSetMat3Transponse ( inverseContactMatrix, contactMatrix );

	GXUInt linkedContacts = contacts[ 0 ].GetLinkedContacts ();

	GXVec3 totalLinearVelocity ( 0.0f, 0.0f, 0.0f );
	GXVec3 totalAngularVelocity ( 0.0f, 0.0f, 0.0f );

	for ( GXUInt i = 0; i < linkedContacts; i++ )
	{
		GXContact& contact = contacts[ i ];

		GXFloat linearComponent = rigidBody.GetInverseMass ();

		GXVec3 rigidBodyCenterOfMassToContactPoint;
		GXSubVec3Vec3 ( rigidBodyCenterOfMassToContactPoint, contact.GetContactPoint (), rigidBody.GetLocation () );

		GXFloat deltaVelocity = linearComponent + CalculateAngularComponentContactSpace ( rigidBodyCenterOfMassToContactPoint, rigidBody, contactNormal );

		GXVec3 contactVelocity;
		GXCrossVec3Vec3 ( contactVelocity, rigidBody.GetAngularVelocity (), rigidBodyCenterOfMassToContactPoint );
		GXSumVec3Vec3 ( contactVelocity, contactVelocity, rigidBody.GetLinearVelocity () );

		GXVec3 contactVelocityContactSpace;
		GXMulVec3Mat3 ( contactVelocityContactSpace, contactVelocity, inverseContactMatrix );

		if ( &rigidBody == &contact.GetFirstRigidBody () )
			GXReverseVec3 ( contactVelocityContactSpace );

		GXFloat desiredVelocity = -contactVelocityContactSpace.z * ( 1.0f + contact.GetRestitution () );

		GXVec3 impulse;
		GXMulVec3Scalar ( impulse, contactNormal, desiredVelocity / deltaVelocity );

		if ( &rigidBody == &contact.GetFirstRigidBody () )
			GXReverseVec3 ( impulse );

		GXVec3 linearVelocity;
		GXVec3 angularVelocity;
		GetRigidBodyKinematics ( linearVelocity, angularVelocity, rigidBody, impulse, rigidBodyCenterOfMassToContactPoint );

		GXSumVec3Vec3 ( totalLinearVelocity, totalLinearVelocity, linearVelocity );
		GXSumVec3Vec3 ( totalAngularVelocity, totalAngularVelocity, angularVelocity );
	}

	GXFloat inverseLinkedContacts = 1.0f / (GXFloat)linkedContacts;

	GXMulVec3Scalar ( totalLinearVelocity, totalLinearVelocity, inverseLinkedContacts );
	GXMulVec3Scalar ( totalAngularVelocity, totalAngularVelocity, inverseLinkedContacts );

	rigidBody.SetLinearVelocity ( totalLinearVelocity );
	rigidBody.SetAngularVelocity ( totalAngularVelocity );
}

GXVoid GXContactResolver::ResolveDoubleBodyContacts ( GXContact* contacts )
{
	GXUInt todo = 0;
	//TODO
}

GXVoid GXContactResolver::GetRigidBodyKinematics ( GXVec3 &linearVelocity, GXVec3 &angularVelocity, GXRigidBody &rigidBody, const GXVec3 &impulse, const GXVec3 &centerOfMassToContactPoint )
{
	GXVec3 newVelocity;
	GXSumVec3ScaledVec3 ( linearVelocity, rigidBody.GetLinearVelocity (), rigidBody.GetInverseMass (), impulse );

	GXVec3 angularMomentum;
	GXCrossVec3Vec3 ( angularMomentum, impulse, centerOfMassToContactPoint );

	GXVec3 deltaAngularVelocity;
	GXMulVec3Mat3 ( deltaAngularVelocity, angularMomentum, rigidBody.GetInverseInertiaTensorWorld () );

	GXSumVec3Vec3 ( angularVelocity, rigidBody.GetAngularVelocity (), deltaAngularVelocity );
}
