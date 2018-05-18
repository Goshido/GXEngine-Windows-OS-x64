// version 1.3

#include <GXPhysics/GXContact.h>
#include <GXCommon/GXLogger.h>


GXVoid GXContact::SetLinkedContacts ( GXUInt contacts )
{
	linkedContacts = contacts;
}

GXUInt GXContact::GetLinkedContacts () const
{
	return linkedContacts;
}

GXVoid GXContact::SetShapes ( const GXShape &a, const GXShape &b )
{
	bodies[ 0 ] = &a.GetRigidBody ();
	bodies[ 1 ] = &b.GetRigidBody ();

	// TODO frictions|restitution imperical lookup table

	staticFriction = GXMinf ( a.GetStaticFriction (), b.GetStaticFriction () );
	dynamicFriction = GXMinf ( a.GetDynamicFriction (), b.GetDynamicFriction () );

	if ( staticFriction < dynamicFriction )
		dynamicFriction = staticFriction;

	restitution = GXMinf ( a.GetRestitution (), b.GetRestitution () );
}

GXRigidBody& GXContact::GetFirstRigidBody ()
{
	return *bodies[ 0 ];
}

GXRigidBody& GXContact::GetSecondRigidBody ()
{
	return *bodies[ 1 ];
}

const GXVec3& GXContact::GetTangent () const
{
	return tangent;
}

const GXVec3& GXContact::GetBitangent () const
{
	return bitangent;
}

GXVoid GXContact::SetNormal ( const GXVec3 &contactNormal )
{
	GXMat3 tmp;
	tmp.From ( contactNormal );

	tmp.GetX ( tangent );
	tmp.GetY ( bitangent );
	normal = contactNormal;
}

const GXVec3& GXContact::GetNormal () const
{
	return normal;
}

GXVoid GXContact::SetContactPoint ( const GXVec3 &contactPoint )
{
	point = contactPoint;
}

const GXVec3& GXContact::GetContactPoint () const
{
	return point;
}

GXVoid GXContact::SetPenetration ( GXFloat contactPenetration )
{
	penetration = contactPenetration;
}

GXFloat GXContact::GetPenetration () const
{
	return penetration;
}

GXFloat GXContact::GetStaticFriction () const
{
	return staticFriction;
}

GXFloat GXContact::GetDynamicFriction () const
{
	return dynamicFriction;
}

GXFloat GXContact::GetRestitution () const
{
	return restitution;
}

GXVoid GXContact::SetGJKIterations ( GXUInt iterations )
{
	gjkIterations = iterations;
}

GXUInt GXContact::GetGJKIterations () const
{
	return gjkIterations;
}

GXVoid GXContact::SetEPAIterations ( GXUInt iterations )
{
	epaIterations = iterations;
}

GXUInt GXContact::GetEPAIterations () const
{
	return epaIterations;
}

GXVoid GXContact::SetSupportPoints ( GXUInt totalSupportPoints )
{
	supportPoints = totalSupportPoints;
}

GXUInt GXContact::GetSupportPoints () const
{
	return supportPoints;
}

GXVoid GXContact::SetEdges ( GXUInt totalEdges )
{
	edges = totalEdges;
}

GXUInt GXContact::GetEdges () const
{
	return edges;
}

GXVoid GXContact::SetFaces ( GXUInt totalFaces )
{
	faces = totalFaces;
}

GXUInt GXContact::GetFaces () const
{
	return faces;
}
