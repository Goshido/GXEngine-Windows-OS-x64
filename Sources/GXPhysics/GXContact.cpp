// version 1.5

#include <GXPhysics/GXContact.h>
#include <GXCommon/GXLogger.h>


GXVoid GXContact::SetLinkedContacts ( GXUInt contacts )
{
    _linkedContacts = contacts;
}

GXUInt GXContact::GetLinkedContacts () const
{
    return _linkedContacts;
}

GXVoid GXContact::SetShapes ( const GXShape &a, const GXShape &b )
{
    _bodies[ 0u ] = &a.GetRigidBody ();
    _bodies[ 1u ] = &b.GetRigidBody ();

    // TODO frictions|restitution imperical lookup table

    _staticFriction = GXMinf ( a.GetStaticFriction (), b.GetStaticFriction () );
    _dynamicFriction = GXMinf ( a.GetDynamicFriction (), b.GetDynamicFriction () );

    if ( _staticFriction < _dynamicFriction )
        _dynamicFriction = _staticFriction;

    _restitution = GXMinf ( a.GetRestitution (), b.GetRestitution () );
}

GXRigidBody& GXContact::GetFirstRigidBody ()
{
    return *_bodies[ 0u ];
}

GXRigidBody& GXContact::GetSecondRigidBody ()
{
    return *_bodies[ 1u ];
}

const GXVec3& GXContact::GetTangent () const
{
    return _tangent;
}

const GXVec3& GXContact::GetBitangent () const
{
    return _bitangent;
}

GXVoid GXContact::SetNormal ( const GXVec3 &normal )
{
    GXMat3 tmp;
    tmp.From ( normal );

    tmp.GetX ( _tangent );
    tmp.GetY ( _bitangent );
    _normal = normal;
}

const GXVec3& GXContact::GetNormal () const
{
    return _normal;
}

GXVoid GXContact::SetContactPoint ( const GXVec3 &point )
{
    _point = point;
}

const GXVec3& GXContact::GetContactPoint () const
{
    return _point;
}

GXVoid GXContact::SetPenetration ( GXFloat penetration )
{
    _penetration = penetration;
}

GXFloat GXContact::GetPenetration () const
{
    return _penetration;
}

GXFloat GXContact::GetStaticFriction () const
{
    return _staticFriction;
}

GXFloat GXContact::GetDynamicFriction () const
{
    return _dynamicFriction;
}

GXFloat GXContact::GetRestitution () const
{
    return _restitution;
}

GXVoid GXContact::SetGJKIterations ( GXUInt iterations )
{
    _gjkIterations = iterations;
}

GXUInt GXContact::GetGJKIterations () const
{
    return _gjkIterations;
}

GXVoid GXContact::SetEPAIterations ( GXUInt iterations )
{
    _epaIterations = iterations;
}

GXUInt GXContact::GetEPAIterations () const
{
    return _epaIterations;
}

GXVoid GXContact::SetSupportPoints ( GXUInt totalSupportPoints )
{
    _supportPoints = totalSupportPoints;
}

GXUInt GXContact::GetSupportPoints () const
{
    return _supportPoints;
}

GXVoid GXContact::SetEdges ( GXUInt totalEdges )
{
    _edges = totalEdges;
}

GXUInt GXContact::GetEdges () const
{
    return _edges;
}

GXVoid GXContact::SetFaces ( GXUInt totalFaces )
{
    _faces = totalFaces;
}

GXUInt GXContact::GetFaces () const
{
    return _faces;
}
