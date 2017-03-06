//version 1.0

#ifndef GX_CONTACT
#define GX_CONTACT


#include <GXCommon/GXMath.h>
#include "GXRigidBody.h"


class GXContact
{
	private:
		GXRigidBody*	bodies[ 2 ]; //second can be nullptr

		GXFloat			friction;
		GXFloat			restitution;
		GXVec3			point;
		GXVec3			normal;
		GXFloat			penetration;

	public:
		GXVoid SetData ( GXRigidBody &bodyA, GXRigidBody* bodyB, GXFloat friction, GXFloat restitution );

		GXVoid SetNormal ( const GXVec3 &normal );
		GXVoid SetContactPoint ( const GXVec3 &point );
		GXVoid SetPenetration ( GXFloat penetration );

};


#endif GX_CONTACT
