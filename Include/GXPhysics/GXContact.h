// version 1.4

#ifndef GX_CONTACT
#define GX_CONTACT


#include "GXRigidBody.h"
#include "GXShape.h"
#include <GXCommon/GXMath.h>


class GXContact final
{
	private:
		GXRigidBody*	bodies[ 2 ];

		GXFloat			staticFriction;
		GXFloat			dynamicFriction;
		GXFloat			restitution;
		GXVec3			point;

		GXVec3			tangent;
		GXVec3			bitangent;
		GXVec3			normal;

		GXFloat			penetration;

		GXUInt			gjkIterations;
		GXUInt			epaIterations;

		GXUInt			supportPoints;
		GXUInt			edges;
		GXUInt			faces;

		GXUInt			linkedContacts;

	public:
		// Value makes sence when contact geometry is edge-face or face-face.
		// Used by contact resolver.
		GXVoid SetLinkedContacts ( GXUInt contacts );
		GXUInt GetLinkedContacts () const;

		GXVoid SetShapes ( const GXShape &a, const GXShape &b );

		// Always not kinematic rigid body.
		GXRigidBody& GetFirstRigidBody ();

		// Can be kinematic or not kinemartic rigid body.
		GXRigidBody& GetSecondRigidBody ();

		const GXVec3& GetTangent () const;
		const GXVec3& GetBitangent () const;

		// Direction to move first rigid body in world space.
		GXVoid SetNormal ( const GXVec3 &contactNormal );
		const GXVec3& GetNormal () const;

		// Contact point in world space.
		GXVoid SetContactPoint ( const GXVec3 &contactPoint );
		const GXVec3& GetContactPoint () const;

		GXVoid SetPenetration ( GXFloat contactPenetration );
		GXFloat GetPenetration () const;

		GXFloat GetStaticFriction () const;
		GXFloat GetDynamicFriction () const;
		GXFloat GetRestitution () const;

		GXVoid SetGJKIterations ( GXUInt iterations );
		GXUInt GetGJKIterations () const;

		GXVoid SetEPAIterations ( GXUInt iterations );
		GXUInt GetEPAIterations () const;

		GXVoid SetSupportPoints ( GXUInt totalSupportPoints );
		GXUInt GetSupportPoints () const;

		GXVoid SetEdges ( GXUInt totalEdges );
		GXUInt GetEdges () const;

		GXVoid SetFaces ( GXUInt totalFaces );
		GXUInt GetFaces () const;

	private:
		GXContact ( const GXContact &other ) = delete;
		GXContact& operator = ( const GXContact &other ) = delete;
};


#endif // GX_CONTACT
