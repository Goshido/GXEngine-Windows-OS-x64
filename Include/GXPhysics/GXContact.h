//version 1.0

#ifndef GX_CONTACT
#define GX_CONTACT


#include <GXCommon/GXMath.h>
#include "GXRigidBody.h"
#include "GXShape.h"


class GXContact
{
	private:
		GXRigidBody*	bodies[ 2 ]; //second can be nullptr

		GXFloat			friction;
		GXFloat			restitution;
		GXVec3			point;
		GXVec3			normal;
		GXFloat			penetration;

		GXMat3			contactToWorld;
		GXVec3			contactVelocity;
		GXFloat			desiredDeltaVelocity;
		GXVec3			relativeContactPositions[ 2 ];

	public:
		GXVoid SetData ( const GXShape &bodyA, const GXShape* bodyB );

		GXVoid SetNormal ( const GXVec3 &normal );
		const GXVec3& GetNormal () const;
		GXVoid SetContactPoint ( const GXVec3 &point );
		GXVoid SetPenetration ( GXFloat penetration );
		GXFloat GetPenetration () const;
		
		const GXMat3& GetContactToWorldTransform () const;
		GXVoid SetContactVelocity ( const GXVec3 &velocity );
		const GXVec3& GetContactVelocity () const;
		GXVoid SetDesiredDeltaVelocity ( GXFloat deltaVelocity );
		GXFloat GetDesiredDeltaVelocity () const;
		GXVoid SetRelativeContactPosition ( GXUByte index, const GXVec3 &position );
		const GXVec3& GetRelativeContactPosition ( GXUByte index ) const;
		GXRigidBody* GetRigidBody ( GXUByte index );

		GXVoid CalculateInternals ( GXFloat deltaTime );
		GXVoid CalculateDesiredDeltaVelocity ( GXFloat deltaTime );
		GXVoid MatchAwakeState ();
		GXVoid ApplyPositionChange ( GXVec3 linearChange[ 2 ], GXVec3 angularChange[ 2 ], GXFloat penetration );
		GXVoid ApplyVelocityChange ( GXVec3 velocityChange[ 2 ], GXVec3 rotationChange[ 2 ] );

	private:
		GXVoid SwapBodies ();
		GXVoid CalculateContactBasis ();
		GXVoid CalculateLocalVelocity ( GXVec3 &out, GXUInt bodyIndex, GXFloat deltaTime );
		GXVec3 CalculateFrictionlessImpulse ();
		GXVec3 CalculateFrictionImpulse ();
};


#endif GX_CONTACT
