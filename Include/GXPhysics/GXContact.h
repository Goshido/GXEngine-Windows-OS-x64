// version 1.5

#ifndef GX_CONTACT
#define GX_CONTACT


#include "GXRigidBody.h"
#include "GXShape.h"
#include <GXCommon/GXMath.h>


class GXContact final
{
    private:
        GXRigidBody*    _bodies[ 2u ];

        GXFloat         _staticFriction;
        GXFloat         _dynamicFriction;
        GXFloat         _restitution;
        GXVec3          _point;

        GXVec3          _tangent;
        GXVec3          _bitangent;
        GXVec3          _normal;

        GXFloat         _penetration;

        GXUInt          _gjkIterations;
        GXUInt          _epaIterations;

        GXUInt          _supportPoints;
        GXUInt          _edges;
        GXUInt          _faces;

        GXUInt          _linkedContacts;

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
        GXVoid SetNormal ( const GXVec3 &normal );
        const GXVec3& GetNormal () const;

        // Contact point in world space.
        GXVoid SetContactPoint ( const GXVec3 &point );
        const GXVec3& GetContactPoint () const;

        GXVoid SetPenetration ( GXFloat penetration );
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
