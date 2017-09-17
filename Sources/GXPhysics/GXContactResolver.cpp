//version 1.1

#include <GXPhysics/GXContactResolver.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_ANGULAR_MOVE_LIMIT		0.1f
#define ANGULAR_INERTIA_EPSILON			0.0001f


GXContactResolver::GXContactResolver ()
{
	SetAngularMoveLimit ( DEFAULT_ANGULAR_MOVE_LIMIT );
}

GXVoid GXContactResolver::ResolveContacts ( GXContact* contactArray, GXUInt numContacts )
{
	GXUInt i = 0;
	while ( i < numContacts )
	{
		GXContact& contact = contactArray[ i ];
		GXUInt linkedContacts = contact.GetLinkedContacts ();

		GXRigidBody& secondRigidBody = contact.GetSecondRigidBody ();

		if ( secondRigidBody.IsKinematic () )
			ResolveSingleBodyContacts ( contactArray + i );
		else
			ResolveDoubleBodyContacts ( contactArray + i );

		i += linkedContacts;
	}
}

GXVoid GXContactResolver::SetAngularMoveLimit ( GXFloat limit )
{
	angularMoveLimit = limit;
}

GXVoid GXContactResolver::ResolveSingleBodyContacts ( GXContact* contacts )
{
	GXVec3 deltaLocationWorld;
	GXVec3 deltaRotationWorld;

	GXVec3 deltaLinearVelocityWorld;
	GXVec3 deltaAngularVelocityWorld;

	GXUInt i = 0;
	GXUInt linkedContacts = contacts->GetLinkedContacts ();

	GXRigidBody& notKinematicRigidBody = contacts->GetFirstRigidBody ();
	const GXRigidBody& kinematicRigidBody = contacts->GetSecondRigidBody ();

	while ( i < linkedContacts )
	{
		const GXContact& contact = contacts[ i ];

		GXVec3 notKinematicRigidBodyCenterOfMassToContactPointWorld;
		notKinematicRigidBodyCenterOfMassToContactPointWorld.Substract ( contact.GetContactPoint (), notKinematicRigidBody.GetLocation () );

		GXVec3 kinematicRigidBodyCenterOfMassToContactPointWorld;
		kinematicRigidBodyCenterOfMassToContactPointWorld.Substract ( contact.GetContactPoint (), kinematicRigidBody.GetLocation () );

		GXVec3 contactVelocityWorld;
		GetContactVelocityWorld ( contactVelocityWorld, notKinematicRigidBody, notKinematicRigidBodyCenterOfMassToContactPointWorld, kinematicRigidBody, kinematicRigidBodyCenterOfMassToContactPointWorld );

		if ( contactVelocityWorld.DotProduct ( contact.GetNormal () ) > 0.0f )
		{
			i++;
			continue;
		}

		ResolvePenetrationWorld ( deltaLocationWorld, deltaRotationWorld, notKinematicRigidBody, notKinematicRigidBodyCenterOfMassToContactPointWorld, contact.GetNormal (), contact.GetPenetration () );

		GXFloat alpha = -( 1.0f + contact.GetRestitution () ) * contactVelocityWorld.DotProduct ( contact.GetNormal () );

		if ( alpha == 0.0f )
		{
			//Not procceed zero impulses.
			i++;
			continue;
		}

		GXVec3 betta;
		betta.CrossProduct ( notKinematicRigidBodyCenterOfMassToContactPointWorld, contact.GetNormal () );

		GXVec3 gamma;
		notKinematicRigidBody.GetTransposeInverseInertiaTensorWorld ().Multiply ( gamma, betta );

		betta.CrossProduct ( gamma, notKinematicRigidBodyCenterOfMassToContactPointWorld );
		GXFloat yotta = notKinematicRigidBody.GetInverseMass () + contact.GetNormal ().DotProduct ( betta );

		GXVec3 impulseWorld ( contact.GetNormal () );
		impulseWorld.Multiply ( impulseWorld, alpha / yotta );

		GXVec3 linearVelocityWorld;
		GXVec3 angularVelocityWorld;

		GetRigidBodyKinematicsWorld ( linearVelocityWorld, angularVelocityWorld, notKinematicRigidBody, impulseWorld, notKinematicRigidBodyCenterOfMassToContactPointWorld );

		deltaLinearVelocityWorld.Sum ( deltaLinearVelocityWorld, linearVelocityWorld );
		deltaAngularVelocityWorld.Sum ( deltaAngularVelocityWorld, angularVelocityWorld );

		i++;
	}

	GXFloat inverseLinkedContacts = 1.0f / (GXFloat)i;

	GXVec3 newLocation;
	newLocation.Sum ( notKinematicRigidBody.GetLocation (), inverseLinkedContacts, deltaLocationWorld );
	notKinematicRigidBody.SetLocation ( newLocation );

	deltaRotationWorld.Multiply ( deltaRotationWorld, -0.5f * inverseLinkedContacts );
	GXQuat alpha ( deltaRotationWorld.GetX (), deltaRotationWorld.GetY (), deltaRotationWorld.GetZ (), 0.0f );
	GXQuat betta;
	betta.Multiply ( alpha, notKinematicRigidBody.GetRotation () );
	GXQuat newRotation;
	newRotation.Sum ( notKinematicRigidBody.GetRotation (), betta );

	notKinematicRigidBody.SetRotaton ( newRotation );
	
	GXVec3 newLinearVelocityWorld;
	newLinearVelocityWorld.Sum ( notKinematicRigidBody.GetLinearVelocity (), deltaLinearVelocityWorld );

	notKinematicRigidBody.SetLinearVelocity ( newLinearVelocityWorld );

	// Too complex. Need to find explanation or simplify!
	GXVec3 newAngularVelocityWorld;
	newAngularVelocityWorld.Sum ( notKinematicRigidBody.GetAngularVelocity (), deltaAngularVelocityWorld );
	newAngularVelocityWorld.Reverse ();
	notKinematicRigidBody.SetAngularVelocity ( newAngularVelocityWorld );
}

GXVoid GXContactResolver::ResolveDoubleBodyContacts ( GXContact* contacts )
{
	GXVec3 deltaLocationWorld[ 2 ];
	GXVec3 deltaRotationWorld[ 2 ];

	GXVec3 deltaLinearVelocityWorld[ 2 ];
	GXVec3 deltaAngularVelocityWorld[ 2 ];

	GXUInt i = 0;
	GXUInt linkedContacts = contacts->GetLinkedContacts ();

	GXRigidBody* rigidBodies[ 2 ] = { &contacts->GetFirstRigidBody (), &contacts->GetSecondRigidBody () };

	GXVec3 reversedContactNormal ( contacts->GetContactPoint () );
	reversedContactNormal.Reverse ();

	while ( i < linkedContacts )
	{
		const GXContact& contact = contacts[ i ];

		GXVec3 centerOfMassToContactPointWorld[ 2 ];
		centerOfMassToContactPointWorld[ 0 ].Substract ( contact.GetContactPoint (), rigidBodies[ 0 ]->GetLocation () );
		centerOfMassToContactPointWorld[ 1 ].Substract ( contact.GetContactPoint (), rigidBodies[ 1 ]->GetLocation () );

		GXVec3 contactVelocityWorld;
		GetContactVelocityWorld ( contactVelocityWorld, *( rigidBodies[ 0 ] ), centerOfMassToContactPointWorld[ 0 ], *( rigidBodies[ 1 ] ), centerOfMassToContactPointWorld[ 1 ] );

		if ( contactVelocityWorld.DotProduct ( contact.GetNormal () ) > 0.0f )
		{
			i++;
			continue;
		}

		ResolvePenetrationWorld ( deltaLocationWorld[ 0 ], deltaRotationWorld[ 0 ], *( rigidBodies[ 0 ] ), centerOfMassToContactPointWorld[ 0 ], contact.GetNormal (), contact.GetPenetration () );
		ResolvePenetrationWorld ( deltaLocationWorld[ 1 ], deltaRotationWorld[ 1 ], *( rigidBodies[ 1 ] ), centerOfMassToContactPointWorld[ 1 ], reversedContactNormal, contact.GetPenetration () );

		GXFloat alpha = -( 1.0f + contact.GetRestitution () ) * contactVelocityWorld.DotProduct ( contact.GetNormal () );

		if ( alpha == 0.0f )
		{
			//Not procceed zero impulses.
			i++;
			continue;
		}

		GXVec3 betta;
		betta.CrossProduct ( centerOfMassToContactPointWorld[ 0 ], contact.GetNormal () );

		GXVec3 gamma;
		rigidBodies[ 0 ]->GetTransposeInverseInertiaTensorWorld ().Multiply ( gamma, betta );

		GXVec3 phi;
		phi.CrossProduct ( gamma, centerOfMassToContactPointWorld[ 0 ] );

		betta.CrossProduct ( centerOfMassToContactPointWorld[ 1 ], contact.GetNormal () );
		rigidBodies[ 1 ]->GetTransposeInverseInertiaTensorWorld ().Multiply ( gamma, betta );

		GXVec3 eta;
		eta.CrossProduct ( gamma, centerOfMassToContactPointWorld[ 1 ] );

		betta.Sum ( phi, eta );

		GXFloat yotta = rigidBodies[ 0 ]->GetInverseMass () + rigidBodies[ 1 ]->GetInverseMass () + contact.GetNormal ().DotProduct ( betta );

		GXVec3 impulseWorld ( contact.GetNormal () );
		impulseWorld.Multiply ( impulseWorld, alpha / yotta );

		GXVec3 linearVelocityWorld;
		GXVec3 angularVelocityWorld;

		GetRigidBodyKinematicsWorld ( linearVelocityWorld, angularVelocityWorld, *( rigidBodies[ 0 ] ), impulseWorld, centerOfMassToContactPointWorld[ 0 ] );
		deltaLinearVelocityWorld[ 0 ].Sum ( deltaLinearVelocityWorld[ 0 ], linearVelocityWorld );
		deltaAngularVelocityWorld[ 0 ].Sum ( deltaAngularVelocityWorld[ 0 ], angularVelocityWorld );

		impulseWorld.Reverse ();

		GetRigidBodyKinematicsWorld ( linearVelocityWorld, angularVelocityWorld, *( rigidBodies[ 1 ] ), impulseWorld, centerOfMassToContactPointWorld[ 1 ] );
		deltaLinearVelocityWorld[ 1 ].Sum ( deltaLinearVelocityWorld[ 1 ], linearVelocityWorld );
		deltaAngularVelocityWorld[ 1 ].Sum ( deltaAngularVelocityWorld[ 1 ], angularVelocityWorld );

		i++;
	}

	GXFloat inverseLinkedContacts = 1.0f / (GXFloat)i;

	for ( GXUByte j = 0; j < 2; j++ )
	{
		GXVec3 newLocation;
		newLocation.Sum ( rigidBodies[ j ]->GetLocation (), inverseLinkedContacts, deltaLocationWorld[ j ] );
		rigidBodies[ j ]->SetLocation ( newLocation );

		deltaRotationWorld[ j ].Multiply ( deltaRotationWorld[ j ], -0.5f * inverseLinkedContacts );
		GXQuat alpha ( deltaRotationWorld[ j ].GetX (), deltaRotationWorld[ j ].GetY (), deltaRotationWorld[ j ].GetZ (), 0.0f );
		GXQuat betta;
		betta.Multiply ( alpha, rigidBodies[ j ]->GetRotation () );
		GXQuat newRotation;
		newRotation.Sum ( rigidBodies[ j ]->GetRotation (), betta );
		rigidBodies[ j ]->SetRotaton ( newRotation );

		GXVec3 newLinearVelocityWorld;
		newLinearVelocityWorld.Sum ( rigidBodies[ j ]->GetLinearVelocity (), deltaLinearVelocityWorld[ j ] );

		rigidBodies[ j ]->SetLinearVelocity ( newLinearVelocityWorld );

		// Too complex. Need to find explanation or simplify!
		GXVec3 newAngularVelocityWorld;
		newAngularVelocityWorld.Sum ( rigidBodies[ j ]->GetAngularVelocity (), deltaAngularVelocityWorld[ j ] );
		newAngularVelocityWorld.Reverse ();
		rigidBodies[ j ]->SetAngularVelocity ( newAngularVelocityWorld );
	}
}

GXVoid GXContactResolver::GetRigidBodyKinematicsWorld ( GXVec3 &linearVelocityWorld, GXVec3 &angularVelocityWorld, const GXRigidBody &rigidBody, const GXVec3 &impulseWorld, const GXVec3 &centerOfMassToContactPointWorld )
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

GXVoid GXContactResolver::GetContactVelocityWorld ( GXVec3 &contactVelocityWorld, const GXRigidBody &rigidBodyA, const GXVec3 &rigidBodyACenterOfMassToContactPointWorld, const GXRigidBody &rigidBodyB, const GXVec3 &rigidBodyBCenterOfMassToContactPointWorld )
{
	GXVec3 rigidBodyAContactPointVelocityWorld;
	rigidBodyAContactPointVelocityWorld.CrossProduct ( rigidBodyA.GetAngularVelocity (), rigidBodyACenterOfMassToContactPointWorld );
	rigidBodyAContactPointVelocityWorld.Sum ( rigidBodyAContactPointVelocityWorld, rigidBodyA.GetLinearVelocity () );

	GXVec3 rigidBodyBContactPointVelocityWorld;
	rigidBodyBContactPointVelocityWorld.CrossProduct ( rigidBodyB.GetAngularVelocity (), rigidBodyBCenterOfMassToContactPointWorld );
	rigidBodyBContactPointVelocityWorld.Sum ( rigidBodyBContactPointVelocityWorld, rigidBodyB.GetLinearVelocity () );

	contactVelocityWorld.Substract ( rigidBodyAContactPointVelocityWorld, rigidBodyBContactPointVelocityWorld );
}

GXVoid GXContactResolver::ResolvePenetrationWorld ( GXVec3 &deltaLocationWorld, GXVec3 &deltaRotationWorld, const GXRigidBody &rigidBody, const GXVec3 &centerOfMassToContactPointWorld, const GXVec3 &contactNormalWorld, GXFloat contactPenetration )
{
	GXVec3 unitNormalAngularMomentumWorld;
	unitNormalAngularMomentumWorld.CrossProduct ( centerOfMassToContactPointWorld, contactNormalWorld );

	GXVec3 deltaAngularVelocityFromUnitNormalAngularMomentumWorld;
	rigidBody.GetTransposeInverseInertiaTensorWorld ().Multiply ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, unitNormalAngularMomentumWorld );

	GXVec3 deltaLinearVelocityFromUnitNormalAngularMomentumWorld;
	deltaLinearVelocityFromUnitNormalAngularMomentumWorld.CrossProduct ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, centerOfMassToContactPointWorld );

	GXFloat angularInertia = deltaLinearVelocityFromUnitNormalAngularMomentumWorld.DotProduct ( contactNormalWorld ) + ANGULAR_INERTIA_EPSILON;
	GXFloat linearInertia = rigidBody.GetInverseMass ();
	GXFloat inverseTotalInertia = 1.0f / ( angularInertia + linearInertia );

	GXFloat linearMove = contactPenetration * linearInertia * inverseTotalInertia;
	GXFloat angularMove = contactPenetration * angularInertia * inverseTotalInertia;

	if ( fabsf ( angularMove ) > angularMoveLimit )
	{
		GXFloat totalMove = linearMove + angularMove;

		if ( angularMove < 0.0f )
			angularMove = -angularMoveLimit;
		else
			angularMove = angularMoveLimit;

		linearMove = totalMove - angularMove;
	}

	deltaLocationWorld.Sum ( deltaLocationWorld, linearMove, contactNormalWorld );

	GXVec3 currentDeltaRotationWorld;
	currentDeltaRotationWorld.Multiply ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, angularMove / angularInertia );
	deltaRotationWorld.Sum ( deltaRotationWorld, currentDeltaRotationWorld );
}
