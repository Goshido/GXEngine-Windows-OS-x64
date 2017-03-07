//version 1.0

#include <GXPhysics/GXContact.h>
#include <GXCommon/GXLogger.h>


#define DEFAILT_VELOCITY_LIMIT		0.25f
#define DEFAULT_ANGULAR_LIMIT		0.2f


GXVoid GXContact::SetData ( GXRigidBody &bodyA, GXRigidBody* bodyB, GXFloat friction, GXFloat restitution )
{
	bodies[ 0 ] = &bodyA;
	bodies[ 1 ] = bodyB;
	this->friction = friction;
	this->restitution = restitution;
}

GXVoid GXContact::SetNormal ( const GXVec3 &normal )
{
	this->normal = normal;
}

GXVoid GXContact::SetContactPoint ( const GXVec3 &point )
{
	this->point = point;
}

GXVoid GXContact::SetPenetration ( GXFloat penetration )
{
	this->penetration = penetration;
}

GXFloat GXContact::GetPenetration () const
{
	return penetration;
}

const GXMat3& GXContact::GetContactToWorldTransform () const
{
	return contactToWorld;
}

GXVoid GXContact::SetContactVelocity ( const GXVec3 &velocity )
{
	contactVelocity = velocity;
}

const GXVec3& GXContact::GetContactVelocity () const
{
	return contactVelocity;
}

GXVoid GXContact::SetDesiredDeltaVelocity ( GXFloat deltaVelocity )
{
	desiredDeltaVelocity = deltaVelocity;
}

GXFloat GXContact::GetDesiredDeltaVelocity () const
{
	return desiredDeltaVelocity;
}

GXVoid GXContact::SetRelativeContactPosition ( GXUByte index, const GXVec3 &position )
{
	if ( index > 1 )
	{
		GXLogW ( L"GXContact::SetRelativeContactPosition::Error - Wrong index %i\n", index );
		return;
	}

	relativeContactPositions[ index ] = position;
}

const GXVec3& GXContact::GetRelativeContactPosition ( GXUByte index ) const
{
	if ( index > 1 )
	{
		GXLogW ( L"GXContact::GetRelativeContactPosition::Error - Wrong index %i\n", index );
		return relativeContactPositions[ 0 ];
	}

	return relativeContactPositions[ index ];
}

GXVoid GXContact::CalculateInternals ( GXFloat deltaTime )
{
	if ( !bodies[ 0 ] )
		SwapBodies ();

	if ( !bodies[ 0 ] )
		GXLogW ( L"GXContact::CalculateInternals::Error - Body A is nullptr!" );

	CalculateContactBasis ();

	GXSubVec3Vec3 ( relativeContactPositions[ 0 ], point, bodies[ 0 ]->GetLocation () );
	if ( bodies[ 1 ] )
		GXSubVec3Vec3 ( relativeContactPositions[ 1 ], point, bodies[ 1 ]->GetLocation () );

	CalculateLocalVelocity ( contactVelocity, 0, deltaTime );
	if ( bodies[ 1 ] )
	{
		 GXVec3 vel;
		 CalculateLocalVelocity ( vel, 1, deltaTime );
		 GXSubVec3Vec3 ( contactVelocity, contactVelocity, vel );
	}

	CalculateDesiredDeltaVelocity ( deltaTime );
}

GXVoid GXContact::MatchAwakeState ()
{
	if ( !bodies[ 1 ] ) return;

	GXBool awake0 = bodies[ 0 ]->IsAwake ();
	GXBool awake1 = bodies[ 1 ]->IsAwake ();

	if ( awake0 ^ awake1 )
	{
		if ( awake0 )
			bodies[ 1 ]->SetAwake ();
		else
			bodies[ 0 ]->SetAwake ();
	}
}

GXVoid GXContact::ApplyPositionChange ( GXVec3 linearChange[ 2 ], GXVec3 angularChange[ 2 ], GXFloat penetration )
{
	GXFloat angularMove[ 2 ];
	GXFloat linearMove[ 2 ];

	GXFloat totalInertia = 0.0f;
	GXFloat linearInirtia[ 2 ];
	GXFloat angularInertia[ 2 ];

	for ( GXUInt i = 0; i < 2; i++ )
	{
		if ( bodies[ i ] )
		{
			GXVec3 alpha;
			GXCrossVec3Vec3 ( alpha, relativeContactPositions[ i ], normal );
			GXVec3 betta;
			GXMulVec3Mat3 ( betta, alpha, bodies[ i ]->GetInverseInertiaTensorWorld () );

			GXVec3 angularInertiaWorld;
			GXCrossVec3Vec3 ( angularInertiaWorld, betta, relativeContactPositions[ i ] );

			angularInertia[ i ] = GXDotVec3Fast ( angularInertiaWorld, normal );
			linearInirtia[ i ] = bodies[ i ]->GetInverseMass ();

			totalInertia = linearInirtia[ i ] + angularInertia[ i ];
		}
	}

	for ( GXUInt i = 0; i < 2; i++ )
	{
		if ( bodies[ i ] )
		{
			GXFloat sign = ( i == 0 ) ? 1.0f : -1.0f;
			angularMove[ i ] = sign * penetration * ( angularInertia[ i ] / totalInertia );
			linearMove[ i ] = sign * penetration * ( linearInirtia[ i ] / totalInertia );

			GXVec3 projection;
			GXSumVec3ScaledVec3 ( projection, relativeContactPositions[ i ], -GXDotVec3Fast ( relativeContactPositions[ i ], normal ), normal );

			GXFloat maxMagnitude = DEFAULT_ANGULAR_LIMIT * GXLengthVec3 ( projection );

			if ( angularMove[ i ] < -maxMagnitude )
			{
				GXFloat totalMove = angularMove[ i ] + linearMove[ i ];
				angularMove[ i ] = -maxMagnitude;
				linearMove[ i ] = totalMove - angularMove[ i ];
			}
			else if ( angularMove[ i ] > maxMagnitude )
			{
				GXFloat totalMove = angularMove[ i ] + linearMove[ i ];
				angularMove[ i ] = maxMagnitude;
				linearMove[ i ] = totalMove - angularMove[ i ];
			}

			if ( angularMove[ i ] == 0.0f )
			{
				angularChange[ i ] = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
			}
			else
			{
				GXVec3 targetAngularDirection;
				GXCrossVec3Vec3 ( targetAngularDirection, relativeContactPositions[ i ], normal );

				GXVec3 alpha;
				GXMulVec3Mat3 ( alpha, targetAngularDirection, bodies[ i ]->GetInverseInertiaTensorWorld () );
				GXMulVec3Scalar ( angularChange[ i ], alpha, angularMove[ i ] / angularInertia[ i ] );
			}

			GXMulVec3Scalar ( linearChange[ i ], normal, linearMove[ i ] );
			GXVec3 loc;
			GXSumVec3ScaledVec3 ( loc, bodies[ i ]->GetLocation (), linearMove[ i ], normal );
			bodies[ i ]->SetLocation ( loc );

			GXQuat rotation;
			GXSumQuatScaledVec3 ( rotation, bodies[ i ]->GetRotation (), 1.0f, angularChange[ i ] );
			bodies[ i ]->SetRotaton ( rotation );

			if ( !bodies[ i ]->IsAwake () )
				bodies[ i ]->CalculateCachedData ();
		}
	}
}

GXVoid GXContact::SwapBodies ()
{
	GXReverseVec3 ( normal );

	GXRigidBody* temp = bodies[ 0 ];
	bodies[ 0 ] = bodies[ 1 ];
	bodies[ 1 ] = temp;
}

GXVoid GXContact::CalculateContactBasis ()
{
	GXVec3 contactTangent[ 2 ];

	if ( fabsf ( normal.x ) > fabsf ( normal.y ) )
	{
		GXFloat s = 1.0f / sqrtf ( normal.z * normal.z + normal.x * normal.x );

		contactTangent[ 0 ].x = normal.z * s;
		contactTangent[ 0 ].y = 0.0f;
		contactTangent[ 0 ].z = -normal.x * s;

		contactTangent[ 1 ].x = normal.y * contactTangent[ 0 ].x;
		contactTangent[ 1 ].y = normal.z * contactTangent[ 0 ].x - normal.x * contactTangent[ 0 ].z;
		contactTangent[ 1 ].z = -normal.y * contactTangent[ 0 ].x;
	}
	else
	{
		GXFloat s = 1.0f / sqrtf ( normal.z * normal.z + normal.y * normal.y );

		contactTangent[ 0 ].x = 0.0f;
		contactTangent[ 0 ].y = -normal.z * s;
		contactTangent[ 0 ].z = normal.y * s;

		contactTangent[ 1 ].x = normal.y * contactTangent[ 0 ].z - normal.z * contactTangent[ 0 ].y;
		contactTangent[ 1 ].y = -normal.x * contactTangent[ 0 ].z;
		contactTangent[ 1 ].z = normal.x * contactTangent[ 0 ].y;
	}

	contactToWorld.xv = normal;
	contactToWorld.yv = contactTangent[ 0 ];
	contactToWorld.zv = contactTangent[ 1 ];
}

GXVoid GXContact::CalculateLocalVelocity ( GXVec3 &out, GXUInt bodyIndex, GXFloat deltaTime )
{
	if ( bodyIndex > 1 )
	{
		GXLogW ( L"GXContact::CalculateLocalVelocity::Error - Wrong body index %i\n", bodyIndex );
		return;
	}

	GXRigidBody* thisBody = bodies[ bodyIndex ];

	GXVec3 velocity;
	GXCrossVec3Vec3 ( velocity, thisBody->GetAngularVelocity (), relativeContactPositions[ bodyIndex ] );
	GXSumVec3Vec3 ( velocity, velocity, thisBody->GetLinearVelocity () );

	GXVec3 contactVelocity;
	GXMat3 invContactToWorld;
	GXSetMat3Transponse ( invContactToWorld, contactToWorld );
	GXMulVec3Mat3 ( contactVelocity, velocity, invContactToWorld );

	GXVec3 alpha;
	GXVec3 accVelocity;
	GXMulVec3Scalar ( alpha, thisBody->GetLastFrameAcceleration (), deltaTime );
	GXMulVec3Mat3 ( accVelocity, alpha, invContactToWorld );

	accVelocity.x = 0.0f;

	GXSumVec3Vec3 ( out, contactVelocity, accVelocity );
}

GXVoid GXContact::CalculateDesiredDeltaVelocity ( GXFloat deltaTime )
{
	GXFloat velocityFromAcc = 0.0f;

	if ( bodies[ 0 ]->IsAwake () )
	{
		GXVec3 alpha;
		GXMulVec3Scalar ( alpha, bodies[ 0 ]->GetLastFrameAcceleration (), deltaTime );
		velocityFromAcc += GXDotVec3Fast ( alpha, normal );
	}

	if ( bodies[ 1 ] && bodies[ 1 ]->IsAwake () )
	{
		GXVec3 alpha;
		GXMulVec3Scalar ( alpha, bodies[ 1 ]->GetLastFrameAcceleration (), deltaTime );
		velocityFromAcc -= GXDotVec3Fast ( alpha, normal );
	}

	GXFloat thisRestitution = restitution;

	if ( fabsf ( contactVelocity.x ) < DEFAILT_VELOCITY_LIMIT )
		thisRestitution = 0.0f;

	desiredDeltaVelocity = -contactVelocity.x - thisRestitution * ( contactVelocity.x - velocityFromAcc );
}
