//version 1.2

#ifndef GX_CONTACT
#define GX_CONTACT


#include <GXCommon/GXMath.h>
#include "GXRigidBody.h"
#include "GXShape.h"


class GXContact
{
	private:
		GXRigidBody*	bodies[ 2 ];

		GXFloat			friction;
		GXFloat			restitution;
		GXVec3			point;
		GXVec3			normal;
		GXFloat			penetration;

		GXUInt			gjkIterations;
		GXUInt			epaIterations;

		GXUInt			supportPoints;
		GXUInt			edges;
		GXUInt			faces;

		GXUInt			linkedContacts;

	public:
		GXVoid SetLinkedContacts ( GXUInt contacts );
		GXUInt GetLinkedContacts () const;

		GXVoid SetShapes ( const GXShape &a, const GXShape &b );

		GXRigidBody& GetFirstRigidBody ();
		GXRigidBody& GetSecondRigidBody ();

		GXVoid SetNormal ( const GXVec3 &normal );
		const GXVec3& GetNormal () const;

		GXVoid SetContactPoint ( const GXVec3 &point );
		const GXVec3& GetContactPoint () const;

		GXVoid SetPenetration ( GXFloat penetration );
		GXFloat GetPenetration () const;

		GXFloat GetRestitution () const;

		GXVoid SetGJKIterations ( GXUInt iterations );
		GXUInt GetGTKIterations () const;

		GXVoid SetEPAIterations ( GXUInt iterations );
		GXUInt GetEPAIterations () const;

		GXVoid SetSupportPoints ( GXUInt supportPoints );
		GXUInt GetSupportPoints () const;

		GXVoid SetEdges ( GXUInt edges );
		GXUInt GetEdges () const;

		GXVoid SetFaces ( GXUInt faces );
		GXUInt GetFaces () const;
};


#endif GX_CONTACT
