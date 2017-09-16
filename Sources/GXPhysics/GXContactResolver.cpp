//version 1.1

#include <GXPhysics/GXContactResolver.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_ANGULAR_MOVE_LIMIT		0.1f
#define ANGULAR_INERTIA_EPSILON			0.0001f
#define PENETRATION_CORRECTOR			1.0e-5f
#define LINEAR_MOVE_CORRECTOR			1.0e-5f


GXContactResolver::GXContactResolver ( GXUInt /*iterations*/ )
{
	SetAngularMoveLimit ( DEFAULT_ANGULAR_MOVE_LIMIT );
}

GXVoid GXContactResolver::ResolveContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime )
{
	GXUInt i = 0;
	while ( i < numContacts )
	{
		GXContact& contact = contactArray[ i ];
		GXUInt linkedContacts = contact.GetLinkedContacts ();

		GXRigidBody& firstRigidBody = contact.GetFirstRigidBody ();

		if ( firstRigidBody.IsKinematic () )
			ResolveDoubleBodyContacts ( contactArray + 1 );
		else
			ResolveSingleBodyContacts ( firstRigidBody, contactArray + i, deltaTime );

		i += linkedContacts;
	}
}

GXVoid GXContactResolver::SetAngularMoveLimit ( GXFloat limit )
{
	angularMoveLimit = limit;
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

	while ( i < linkedContacts )
	{
		const GXContact& contact = contacts[ i ];

		GXVec3 rigidBodyCenterOfMassToContactPointWorld;
		rigidBodyCenterOfMassToContactPointWorld.Substract ( contact.GetContactPoint (), rigidBody.GetLocation () );

		GXVec3 contactVelocityWorld;
		contactVelocityWorld.CrossProduct ( rigidBody.GetAngularVelocity (), rigidBodyCenterOfMassToContactPointWorld );
		contactVelocityWorld.Sum ( contactVelocityWorld, rigidBody.GetLinearVelocity () );

		if ( contactVelocityWorld.DotProduct ( contact.GetNormal () ) > 0.0f )
		{
			i++;
			continue;
		}

		GXVec3 unitNormalAngularMomentumWorld;
		unitNormalAngularMomentumWorld.CrossProduct ( rigidBodyCenterOfMassToContactPointWorld, contact.GetNormal () );

		GXVec3 deltaAngularVelocityFromUnitNormalAngularMomentumWorld;
		rigidBody.GetTransposeInverseInertiaTensorWorld ().Multiply ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, unitNormalAngularMomentumWorld );

		GXVec3 deltaLinearVelocityFromUnitNormalAngularMomentumWorld;
		deltaLinearVelocityFromUnitNormalAngularMomentumWorld.CrossProduct ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, rigidBodyCenterOfMassToContactPointWorld );

		GXFloat angularInertia = deltaLinearVelocityFromUnitNormalAngularMomentumWorld.DotProduct ( contact.GetNormal () ) + ANGULAR_INERTIA_EPSILON;
		GXFloat linearInertia = rigidBody.GetInverseMass ();
		GXFloat inverseTotalInertia = 1.0f / ( angularInertia + linearInertia );

		GXFloat linearMove = contact.GetPenetration () * linearInertia * inverseTotalInertia;
		GXFloat angularMove = contact.GetPenetration () * angularInertia * inverseTotalInertia;

		if ( fabsf ( angularMove ) > angularMoveLimit )
		{
			GXFloat totalMove = linearMove + angularMove;

			if ( angularMove < 0.0f )
				angularMove = -angularMoveLimit;
			else
				angularMove = angularMoveLimit;

			linearMove = totalMove - angularMove;
		}

		deltaLocationWorld.Sum ( deltaLocationWorld, linearMove, contact.GetNormal () );

		GXVec3 currentDeltaRotationWorld;
		currentDeltaRotationWorld.Multiply ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, angularMove / angularInertia );
		deltaRotationWorld.Sum ( deltaRotationWorld, currentDeltaRotationWorld );

		GXFloat alpha = -( 1.0f + contact.GetRestitution () ) * contactVelocityWorld.DotProduct ( contact.GetNormal () );

		GXVec3 betta;
		betta.CrossProduct ( rigidBodyCenterOfMassToContactPointWorld, contact.GetNormal () );

		GXVec3 gamma;
		rigidBody.GetTransposeInverseInertiaTensorWorld ().Multiply ( gamma, betta );

		betta.CrossProduct ( gamma, rigidBodyCenterOfMassToContactPointWorld );
		GXFloat yotta = rigidBody.GetInverseMass () + contact.GetNormal ().DotProduct ( betta );

		GXVec3 impulseWorld;
		impulseWorld.Multiply ( contact.GetNormal (), alpha / yotta );

		GXVec3 linearVelocityWorld;
		GXVec3 angularVelocityWorld;

		GetRigidBodyKinematics ( linearVelocityWorld, angularVelocityWorld, rigidBody, impulseWorld, rigidBodyCenterOfMassToContactPointWorld );

		deltaLinearVelocityWorld.Sum ( deltaLinearVelocityWorld, linearVelocityWorld );
		deltaAngularVelocityWorld.Sum ( deltaAngularVelocityWorld, angularVelocityWorld );

		i++;
	}

	GXFloat inverseLinkedContacts = 1.0f / (GXFloat)i;

	GXVec3 newLocation;
	newLocation.Sum ( rigidBody.GetLocation (), inverseLinkedContacts, deltaLocationWorld );
	rigidBody.SetLocation ( newLocation );

	deltaRotationWorld.Multiply ( deltaRotationWorld, -0.5f * inverseLinkedContacts );
	GXQuat alpha ( deltaRotationWorld.GetX (), deltaRotationWorld.GetY (), deltaRotationWorld.GetZ (), 0.0f );
	GXQuat betta;
	betta.Multiply ( alpha, rigidBody.GetRotation () );
	GXQuat newRotation;
	newRotation.Sum ( rigidBody.GetRotation (), betta );

	rigidBody.SetRotaton ( newRotation );

	GXVec3 newLinearVelocityWorld;
	newLinearVelocityWorld.Sum ( rigidBody.GetLinearVelocity (), deltaLinearVelocityWorld );
	rigidBody.SetLinearVelocity ( newLinearVelocityWorld );

	// Too complex. Need to find explanation or simplify!
	GXVec3 newAngularVelocityWorld ( rigidBody.GetAngularVelocity () );
	newAngularVelocityWorld.Sum ( rigidBody.GetAngularVelocity (), deltaAngularVelocityWorld );
	newAngularVelocityWorld.Reverse ();
	rigidBody.SetAngularVelocity ( newAngularVelocityWorld );
}

GXVoid GXContactResolver::ResolveDoubleBodyContacts ( GXContact* /*contacts*/ )
{
	GXUInt todo = 0;
	GXLogW ( L"%u\n", todo );
	//TODO
}

GXVoid GXContactResolver::GetRigidBodyKinematics ( GXVec3 &linearVelocityWorld, GXVec3 &angularVelocityWorld, const GXRigidBody &rigidBody, const GXVec3 &impulseWorld, const GXVec3 &centerOfMassToContactPointWorld )
{
	GXVec3 alpha ( centerOfMassToContactPointWorld );
	alpha.Normalize ();

	GXVec3 betta ( impulseWorld );
	betta.Normalize ();

	linearVelocityWorld.Multiply ( impulseWorld, -alpha.DotProduct ( betta ) * rigidBody.GetInverseMass () );

	GXVec3 angularMomentumWorld;
	angularMomentumWorld.CrossProduct ( centerOfMassToContactPointWorld, impulseWorld );

	rigidBody.GetTransposeInverseInertiaTensorWorld ().Multiply ( angularVelocityWorld, angularMomentumWorld );
}
