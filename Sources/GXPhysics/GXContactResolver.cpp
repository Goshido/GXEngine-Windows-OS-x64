//version 1.1

#include <GXPhysics/GXContactResolver.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_ANGULAR_MOVE_LIMIT				0.1f
#define ANGULAR_INERTIA_EPSILON					1.0e-4f
#define VELOCITY_PROJECTION_EPSILON				1.0e-4f
#define TOTAL_FORCE_PROJECTION_EPSILON			1.0e-4f
#define TANGENT_VECTOR_SQUARED_LENGTH_EPSILON	1.0e-4f


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

		ResolvePenetrationWorld ( deltaLocationWorld, deltaRotationWorld, notKinematicRigidBody, notKinematicRigidBodyCenterOfMassToContactPointWorld, contact.GetNormal (), contact.GetPenetration () );

		GXFloat contactVelocityProjection = contactVelocityWorld.DotProduct ( contact.GetNormal () );

		if ( contactVelocityProjection >= 0.0f )
		{
			i++;
			continue;
		}

		GXFloat alpha = -( 1.0f + contact.GetRestitution () ) * contactVelocityProjection;

		if ( alpha == 0.0f )
		{
			//Not handle zero impulses.
			i++;
			continue;
		}

		GXVec3 betta;
		betta.CrossProduct ( notKinematicRigidBodyCenterOfMassToContactPointWorld, contact.GetNormal () );

		GXVec3 gamma;
		notKinematicRigidBody.GetInverseInertiaTensorWorld ().MultiplyMatrixVector ( gamma, betta );

		betta.CrossProduct ( gamma, notKinematicRigidBodyCenterOfMassToContactPointWorld );
		GXFloat yotta = notKinematicRigidBody.GetInverseMass () + contact.GetNormal ().DotProduct ( betta );

		GXFloat reactionImpulseMagnitudeWorld = alpha / yotta;

		GXFloat staticFrictionImpulseMagnitudeWorld = contact.GetStaticFriction () * reactionImpulseMagnitudeWorld;
		GXFloat dynamicFrictionImpulseMagnitudeWorld = contact.GetDynamicFriction () * reactionImpulseMagnitudeWorld;

		GXVec3 tangentVector;

		if ( fabsf ( contactVelocityProjection ) > VELOCITY_PROJECTION_EPSILON )
		{
			GXVec3 omega;
			omega.Multiply ( contact.GetNormal (), contactVelocityProjection );
			tangentVector.Substract ( contactVelocityWorld, omega );

			if ( tangentVector.SquaredLength () > TANGENT_VECTOR_SQUARED_LENGTH_EPSILON )
				tangentVector.Normalize ();
		}

		GXFloat tangentProjection = tangentVector.DotProduct ( contactVelocityWorld );
		GXFloat frictionImpulseMagnitudeWorld;

		if ( fabsf ( tangentProjection ) < VELOCITY_PROJECTION_EPSILON )
		{
			GXVec3 contactImpulseWorld;
			contactImpulseWorld.Multiply ( contactVelocityWorld, notKinematicRigidBody.GetMass () );

			GXFloat contactImpulseToTangentVectorProjectionWorld = tangentVector.DotProduct ( contactImpulseWorld );

			if ( contactImpulseToTangentVectorProjectionWorld <= staticFrictionImpulseMagnitudeWorld )
			{
				frictionImpulseMagnitudeWorld = -contactImpulseToTangentVectorProjectionWorld;
			}
			else
			{
				frictionImpulseMagnitudeWorld = -dynamicFrictionImpulseMagnitudeWorld;
			}
		}
		else
		{
			frictionImpulseMagnitudeWorld = -dynamicFrictionImpulseMagnitudeWorld;
		}

		GXVec3 reactionImpulseWorld;
		reactionImpulseWorld.Multiply ( contact.GetNormal (), reactionImpulseMagnitudeWorld );

		GXVec3 frictionImpulseWorld;
		frictionImpulseWorld.Multiply ( tangentVector, frictionImpulseMagnitudeWorld );

		GXVec3 totalImpulseWorld;
		totalImpulseWorld.Sum ( reactionImpulseWorld, frictionImpulseWorld );

		GXVec3 linearVelocityWorld;
		GXVec3 angularVelocityWorld;

		GetRigidBodyKinematicsWorld ( linearVelocityWorld, angularVelocityWorld, notKinematicRigidBody, totalImpulseWorld, notKinematicRigidBodyCenterOfMassToContactPointWorld );

		deltaLinearVelocityWorld.Sum ( deltaLinearVelocityWorld, linearVelocityWorld );
		deltaAngularVelocityWorld.Sum ( deltaAngularVelocityWorld, angularVelocityWorld );

		i++;
	}

	GXFloat inverseLinkedContacts = 1.0f / (GXFloat)i;

	GXVec3 newLocation;
	newLocation.Sum ( notKinematicRigidBody.GetLocation (), inverseLinkedContacts, deltaLocationWorld );
	notKinematicRigidBody.SetLocation ( newLocation );

	deltaRotationWorld.Multiply ( deltaRotationWorld, 0.5f * inverseLinkedContacts );
	GXQuat alpha ( 0.0f, deltaRotationWorld.GetX (), deltaRotationWorld.GetY (), deltaRotationWorld.GetZ () );
	GXQuat betta;
	betta.Multiply ( alpha, notKinematicRigidBody.GetRotation () );
	GXQuat newRotation;
	newRotation.Sum ( notKinematicRigidBody.GetRotation (), betta );
	notKinematicRigidBody.SetRotaton ( newRotation );

	GXVec3 newLinearVelocityWorld;
	newLinearVelocityWorld.Sum ( notKinematicRigidBody.GetLinearVelocity (), inverseLinkedContacts, deltaLinearVelocityWorld );
	notKinematicRigidBody.SetLinearVelocity ( newLinearVelocityWorld );

	GXVec3 newAngularVelocityWorld;
	newAngularVelocityWorld.Sum ( notKinematicRigidBody.GetAngularVelocity (), inverseLinkedContacts, deltaAngularVelocityWorld );
	notKinematicRigidBody.SetAngularVelocity ( newAngularVelocityWorld );
}

GXVoid GXContactResolver::ResolveDoubleBodyContacts ( GXContact* contacts )
{
	GXVec3 deltaLinearVelocity[ 2 ];
	GXVec3 deltaAngularVelocity[ 2 ];

	GXUPointer size = 2 * sizeof ( GXVec3 );
	memset ( deltaLinearVelocity, 0, size );
	memset ( deltaAngularVelocity, 0, size );

	GXUInt i = 0;
	GXUInt linkedContacts = contacts->GetLinkedContacts ();

	while ( i < linkedContacts )
	{
		GXContact& contact = contacts[ i ];

		GXVec3 v[ 2 ];

		CalculateContactVelocity ( v[ 0 ], contact.GetFirstRigidBody (), contact.GetContactPoint () );
		CalculateContactVelocity ( v[ 1 ], contact.GetSecondRigidBody (), contact.GetContactPoint () );

		GXVec3 deltaV;
		deltaV.Substract ( v[ 0 ], v[ 1 ] );

		if ( deltaV.DotProduct ( contact.GetNormal () ) >= 0.0f )
		{
			i++;
			continue;
		}


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

	rigidBody.GetInverseInertiaTensorWorld ().MultiplyMatrixVector ( angularVelocityWorld, angularMomentumWorld );
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
	rigidBody.GetInverseInertiaTensorWorld ().MultiplyMatrixVector ( deltaAngularVelocityFromUnitNormalAngularMomentumWorld, unitNormalAngularMomentumWorld );

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

GXVoid GXContactResolver::CalculateContactVelocity ( GXVec3 &velocity, const GXRigidBody &body, const GXVec3 &point ) const
{
	GXVec3 toPoint;
	toPoint.Substract ( point, body.GetLocation () );

	GXVec3 tangentVelocity;
	tangentVelocity.CrossProduct ( body.GetAngularVelocity (), toPoint );

	velocity.Sum ( body.GetLinearVelocity (), tangentVelocity );
}

GXFloat GXContactResolver::CalculateIdealImpulseMagnitude ( GXContact &contact ) const
{
	GXRigidBody& firstBody = contact.GetFirstRigidBody ();
	GXRigidBody& secondBody = contact.GetSecondRigidBody ();

	GXVec3 normal ( contact.GetNormal () );
	normal.Reverse ();

	GXVec3 toPoint[ 2 ];
	toPoint[ 0 ].Substract ( contact.GetContactPoint (), firstBody.GetLocation () );
	toPoint[ 1 ].Substract ( contact.GetContactPoint (), secondBody.GetLocation () );

	GXVec3 tmp[ 2 ];
	CalculateContactVelocity ( tmp[ 0 ], firstBody, contact.GetContactPoint () );
	CalculateContactVelocity ( tmp[ 1 ], secondBody, contact.GetContactPoint () );

	GXVec3 relativeVelocity;
	relativeVelocity.Substract ( tmp[ 1 ], tmp[ 0 ] );

	if ( secondBody.IsKinematic () )
	{
		tmp[ 0 ].CrossProduct ( toPoint[ 0 ], normal );
		firstBody.GetInverseInertiaTensorWorld ().MultiplyMatrixVector ( tmp[ 1 ], tmp[ 0 ] );
		tmp[ 0 ].CrossProduct ( tmp[ 1 ], toPoint[ 0 ] );

		return ( ( -1.0f - contact.GetRestitution () ) * relativeVelocity.DotProduct ( normal ) ) / ( firstBody.GetInverseMass () + tmp[ 0 ].DotProduct ( normal ) );
	}

	tmp[ 0 ].CrossProduct ( toPoint[ 0 ], normal );
	firstBody.GetInverseInertiaTensorWorld ().MultiplyMatrixVector ( tmp[ 1 ], tmp[ 0 ] );

	GXVec3 alpha;
	alpha.CrossProduct ( tmp[ 1 ], toPoint[ 0 ] );

	tmp[ 0 ].CrossProduct ( toPoint[ 1 ], normal );
	secondBody.GetInverseInertiaTensorWorld ().MultiplyMatrixVector ( tmp[ 1 ], tmp[ 0 ] );

	GXVec3 betta;
	betta.CrossProduct ( tmp[ 1 ], toPoint[ 1 ] );

	tmp[ 0 ].Sum ( alpha, betta );
	
	return ( ( -1.0f - contact.GetRestitution () ) * relativeVelocity.DotProduct ( normal ) ) / ( firstBody.GetInverseMass () + secondBody.GetInverseMass () + tmp[ 0 ].DotProduct ( normal ) );
}

GXFloat GXContactResolver::CalculateFrictionImpulseMagnitude ( GXContact& /*contact*/ ) const
{
	//GXFloat idealImpulse = CalculateIdealImpulseMagnitude ( contact );
	// TODO
	return 0.0f;
}
