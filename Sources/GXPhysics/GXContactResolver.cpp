//version 1.1

#include <GXPhysics/GXContactResolver.h>
#include <GXCommon/GXLogger.h>


GXContactResolver::GXContactResolver ( GXUInt /*iterations*/ )
{
	//NOTHING
}

GXVoid GXContactResolver::ResolveContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime )
{
	GXUInt i = 0;
	while ( i < numContacts )
	{
		GXContact& contact = contactArray[ i ];

		if ( contact.GetPenetration () > 100.0f )
		{
			GXUInt wtf = 0;
			GXLogW ( L"%u\n", wtf );
			continue;
		}

		GXUInt linkedContacts = contact.GetLinkedContacts ();
		GXRigidBody& firstRigidBody = contact.GetFirstRigidBody ();
		GXRigidBody& secondRigidBody = contact.GetSecondRigidBody ();

		if ( firstRigidBody.IsKinematic () )
			ResolveSingleBodyContacts ( secondRigidBody, contactArray + i, deltaTime );
		else if ( secondRigidBody.IsKinematic () )
			ResolveSingleBodyContacts ( firstRigidBody, contactArray + i, deltaTime );
		else
			ResolveDoubleBodyContacts ( contactArray + 1 );

		i += linkedContacts;
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
	rigidBody.GetTransposeInverseInertiaTensorWorld ().Multiply ( angularVelocity, angularMomentum );

	GXVec3 velocity;
	velocity.CrossProduct ( angularVelocity, centerOfMassToContactPoint );

	return velocity.DotProduct ( contactNormal );
}

GXVoid GXContactResolver::ResolveSingleBodyContacts ( GXRigidBody &rigidBody, GXContact* contacts, GXFloat /*deltaTime*/ )
{
	GXVec3 deltaLocationWorld;
	GXVec3 deltaRotationWorld;

	GXVec3 deltaLinearVelocityWorld;
	GXVec3 deltaAngularVelocityWorld;

	GXUInt i = 0;
	GXUInt linkedContacts = contacts->GetLinkedContacts ();

	GXMat3 contactMatrix;
	CalculateContactMatrix ( contactMatrix, contacts->GetNormal () );

	GXMat3 inverseContactMatrix;
	inverseContactMatrix.Transponse ( contactMatrix );

	while ( i < linkedContacts )
	{
		const GXContact& contact = contacts[ i ];

		GXVec3 rigidBodyCenterOfMassToContactPointWorld;
		rigidBodyCenterOfMassToContactPointWorld.Substract ( contact.GetContactPoint (), rigidBody.GetLocation () );

		GXVec3 unitNormalAngularMomentumWorld;
		unitNormalAngularMomentumWorld.CrossProduct ( rigidBodyCenterOfMassToContactPointWorld, contact.GetNormal () );

		GXVec3 deltaAngularVelocityFromUnitNormalAngularMomentumWorld;
		rigidBody.GetTransposeInverseInertiaTensorWorld ().Multiply ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, unitNormalAngularMomentumWorld );

		GXVec3 deltaLinearVelocityFromUnitNormalAngularMomentumWorld;
		deltaLinearVelocityFromUnitNormalAngularMomentumWorld.CrossProduct ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, rigidBodyCenterOfMassToContactPointWorld );

		GXFloat angularInertia = deltaLinearVelocityFromUnitNormalAngularMomentumWorld.DotProduct ( contact.GetNormal () );
		GXFloat linearInertia = rigidBody.GetInverseMass ();
		GXFloat totalInertia = angularInertia + linearInertia;
		GXFloat inverseTotalInertia = 1.0f / totalInertia;

		GXFloat linearMove = contact.GetPenetration () * linearInertia * inverseTotalInertia;
		GXFloat angularMove = contact.GetPenetration () * angularInertia * inverseTotalInertia;

		if ( fabsf ( angularMove ) > 0.2f )
		{
			GXFloat totalMove = linearMove + angularMove;

			if ( angularMove < 0.0f )
			{
				angularMove = -0.2f;
			}
			else
			{
				angularMove = 0.2f;
			}

			linearMove = totalMove - angularMove;
		}

		deltaLocationWorld.Sum ( deltaLocationWorld, linearMove, contact.GetNormal () );

		GXVec3 currentDeltaRotationWorld;
		currentDeltaRotationWorld.Multiply ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, angularMove / angularInertia );
		deltaRotationWorld.Sum ( deltaRotationWorld, currentDeltaRotationWorld );

		GXVec3 contactVelocityWorld;
		contactVelocityWorld.CrossProduct ( rigidBody.GetAngularVelocity (), rigidBodyCenterOfMassToContactPointWorld );
		contactVelocityWorld.Sum ( contactVelocityWorld, rigidBody.GetLinearVelocity () );

		GXVec3 contactVelocityContactSpace;
		inverseContactMatrix.Multiply ( contactVelocityContactSpace, contactVelocityWorld );

		GXFloat desiredVelocity = -contactVelocityContactSpace.GetZ () - contact.GetRestitution ();

		GXVec3 impulseWorld;
		impulseWorld.Multiply ( contact.GetNormal (), desiredVelocity / totalInertia );

		GXVec3 linearVelocityWorld;
		GXVec3 angularVelocityWorld;

		GetRigidBodyKinematics ( linearVelocityWorld, angularVelocityWorld, rigidBody, impulseWorld, rigidBodyCenterOfMassToContactPointWorld );

		deltaLinearVelocityWorld.Sum ( deltaLinearVelocityWorld, linearVelocityWorld );
		deltaAngularVelocityWorld.Sum ( deltaAngularVelocityWorld, angularVelocityWorld );

		i++;
	}

	GXVec3 newLocation;
	newLocation.Sum ( rigidBody.GetLocation (), deltaLocationWorld );
	rigidBody.SetLocation ( newLocation );

	deltaRotationWorld.Multiply ( deltaRotationWorld, 0.5f );
	GXQuat alpha ( deltaRotationWorld.GetX (), deltaRotationWorld.GetY (), deltaRotationWorld.GetZ (), 0.0f );
	GXQuat betta;
	betta.Multiply ( alpha, rigidBody.GetRotation () );
	GXQuat newRotation;
	newRotation.Sum ( rigidBody.GetRotation (), betta );

	rigidBody.SetRotaton ( newRotation );

	GXVec3 newLinearVelocityWorld;
	newLinearVelocityWorld.Sum ( rigidBody.GetLinearVelocity (), 1.0f / (GXFloat)i, deltaLinearVelocityWorld );
	rigidBody.SetLinearVelocity ( newLinearVelocityWorld );

	GXVec3 newAngularVelocityWorld;
	newAngularVelocityWorld.Sum ( rigidBody.GetAngularVelocity (), 1.0f / (GXFloat)i, deltaAngularVelocityWorld );
	rigidBody.SetAngularVelocity ( newAngularVelocityWorld );
}

GXVoid GXContactResolver::ResolveDoubleBodyContacts ( GXContact* /*contacts*/ )
{
	GXUInt todo = 0;
	GXLogW ( L"%u\n", todo );
	//TODO
}

GXVoid GXContactResolver::GetRigidBodyKinematics ( GXVec3 &linearVelocity, GXVec3 &angularVelocity, const GXRigidBody &rigidBody, const GXVec3 &impulse, const GXVec3 &centerOfMassToContactPoint )
{
	GXVec3 newVelocity;
	linearVelocity.Multiply ( impulse, rigidBody.GetInverseMass () );

	GXVec3 angularMomentum;
	angularMomentum.CrossProduct ( centerOfMassToContactPoint, impulse );

	rigidBody.GetTransposeInverseInertiaTensorWorld ().Multiply ( angularVelocity, angularMomentum );
}
