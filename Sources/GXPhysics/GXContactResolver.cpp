//version 1.0

#include <GXPhysics/GXContactResolver.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_POSITION_EPSILON	0.01f
#define DEFAULT_VELOCITY_EPSILON	0.01f


GXContactResolver::GXContactResolver ( GXUInt iterations )
{
	SetVelocityIterations ( iterations );
	SetPositionIterations ( iterations );

	SetPositionEpsilon ( DEFAULT_POSITION_EPSILON );
	SetVelocityEpsilon ( DEFAULT_VELOCITY_EPSILON );
}

GXVoid GXContactResolver::ResolveContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime )
{
	if ( numContacts == 0 ) return;
	if ( !IsValid () ) return;
	
	PrepareContacts ( contactArray, numContacts, deltaTime );
	AdjustPositions ( contactArray, numContacts, deltaTime );
	AdjustVelocities ( contactArray, numContacts, deltaTime );
}

GXVoid GXContactResolver::SetPositionEpsilon ( GXFloat epsilon )
{
	positionEpsilon = epsilon;
}

GXVoid GXContactResolver::SetVelocityEpsilon ( GXFloat epsilon )
{
	velocityEpsilon = epsilon;
}

GXVoid GXContactResolver::SetVelocityIterations ( GXUInt iterations )
{
	velocityIterations = iterations;
}

GXVoid GXContactResolver::SetPositionIterations ( GXUInt iterations )
{
	positionIterations = iterations;
}

GXVoid GXContactResolver::PrepareContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime )
{
	for ( GXUInt i = 0; i < numContacts; i++ )
		contactArray[ i ].CalculateInternals ( deltaTime );
}

GXVoid GXContactResolver::AdjustPositions ( GXContact* contacts, GXUInt numContacts, GXFloat deltaTime )
{
	GXUInt i;
	GXUInt index;
	GXVec3 linearChange[ 2 ];
	GXVec3 angularChange[ 2 ];

	GXFloat max;
	GXVec3 deltaPosition;

	positionIterationsUsed = 0;
	while ( positionIterationsUsed < positionIterations )
	{
		max = positionEpsilon;
		index = numContacts;

		for ( i = 0; i < numContacts; i++ )
		{
			GXFloat penetration = contacts[ i ].GetPenetration ();
			if ( contacts[ i ].GetPenetration () > max )
			{
				max = penetration;
				index = i;
			}
		}

		if ( index == numContacts ) break;

		contacts[ index ].MatchAwakeState ();

		contacts[ index ].ApplyPositionChange ( linearChange, angularChange, max );

		for ( i = 0; i < numContacts; i++ )
		{
			for ( GXUByte b = 0; b < 2; b++ )
			{
				if ( contacts[ i ].GetRigidBody ( b ) )
				{
					for ( GXUByte d = 0; d < 2; d++ )
					{
						if ( contacts[ i ].GetRigidBody ( b ) == contacts[ index ].GetRigidBody ( d ) )
						{
							GXVec3 alpha;
							GXCrossVec3Vec3 ( alpha, angularChange[ d ], contacts[ i ].GetRelativeContactPosition ( b ) );
							GXSumVec3Vec3 ( deltaPosition, linearChange[ d ], alpha );

							GXFloat penetration = contacts[ i ].GetPenetration ();
							penetration += GXDotVec3Fast ( deltaPosition, contacts[ i ].GetNormal () ) * ( b ? 1.0f : -1.0f );
							contacts[ i ].SetPenetration ( penetration );
						}
					}
				}
			}
		}

		positionIterationsUsed++;
	}

	//GXLogW ( L"GXContactResolver::AdjustPositions::Info - Iterations %i/%i\n", positionIterationsUsed, positionIterations );
}

GXVoid GXContactResolver::AdjustVelocities ( GXContact* contacts, GXUInt numContacts, GXFloat deltaTime )
{
	GXVec3 velocityChange[ 2 ];
	GXVec3 rotationChange[ 2 ];
	GXVec3 deltaVel;

	velocityIterationsUsed = 0;
	while ( velocityIterationsUsed < velocityIterations )
	{
		GXFloat max = velocityEpsilon;

		GXUInt index = numContacts;
		for ( GXUInt i = 0; i < numContacts; i++ )
		{
			if ( contacts[ i ].GetDesiredDeltaVelocity () > max )
			{
				max = contacts[ i ].GetDesiredDeltaVelocity ();
				index = i;
			}
		}

		if ( index == numContacts ) break;

		contacts[ index ].MatchAwakeState ();
		contacts[ index ].ApplyVelocityChange ( velocityChange, rotationChange );

		for ( GXUInt i = 0; i < numContacts; i++ )
		{
			for ( GXUInt b = 0; b < 2; b++ )
			{
				if ( contacts[ i ].GetRigidBody ( b ) )
				{
					for ( GXUInt d = 0; d < 2; d++ )
					{
						if ( contacts[ i ].GetRigidBody ( b ) == contacts[ index ].GetRigidBody ( d ) )
						{
							GXVec3 alpha;
							GXCrossVec3Vec3 ( alpha, rotationChange[ d ], contacts[ i ].GetRelativeContactPosition ( b ) );
							GXSumVec3Vec3 ( deltaVel, velocityChange[ d ], alpha );

							GXMat3 betta;
							GXSetMat3Transponse ( betta, contacts[ i ].GetContactToWorldTransform () );

							GXVec3 gamma;
							GXMulVec3Mat3 ( gamma, deltaVel, betta );

							GXVec3 contactVelocity;
							GXSumVec3ScaledVec3 ( contactVelocity, contacts[ i ].GetContactVelocity (), ( b ? -1.0f : 1.0f ), gamma );

							contacts[ i ].SetContactVelocity ( contactVelocity );
							contacts[ i ].CalculateDesiredDeltaVelocity ( deltaTime );
						}
					}
				}
			}
		}

		velocityIterationsUsed++;
	}

	//GXLogW ( L"GXContactResolver::AdjustVelocities::Info - Iterations %i/%i\n", velocityIterationsUsed, velocityIterations );
}

GXBool GXContactResolver::IsValid () const
{
	return velocityIterations > 0 && positionIterations > 0 && positionEpsilon >= 0.0f && velocityEpsilon >= 0.0f;
}
