//version 1.2

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
	bodies[ 0 ] = a.GetRigidBody ();
	bodies[ 1 ] = b.GetRigidBody ();

	friction = GXMinf ( a.GetFriction (), b.GetFriction () );
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

GXVoid GXContact::SetNormal ( const GXVec3 &normal )
{
	this->normal = normal;
}

const GXVec3& GXContact::GetNormal () const
{
	return normal;
}

GXVoid GXContact::SetContactPoint ( const GXVec3 &point )
{
	this->point = point;
}

const GXVec3& GXContact::GetContactPoint () const
{
	return point;
}

GXVoid GXContact::SetPenetration ( GXFloat penetration )
{
	this->penetration = penetration;
}

GXFloat GXContact::GetPenetration () const
{
	return penetration;
}

GXFloat GXContact::GetRestitution () const
{
	return restitution;
}

GXVoid GXContact::SetGJKIterations ( GXUInt iterations )
{
	gjkIterations = iterations;
}

GXUInt GXContact::GetGTKIterations () const
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

GXVoid GXContact::SetSupportPoints ( GXUInt supportPoints )
{
	this->supportPoints = supportPoints;
}

GXUInt GXContact::GetSupportPoints () const
{
	return supportPoints;
}

GXVoid GXContact::SetEdges ( GXUInt edges )
{
	this->edges = edges;
}

GXUInt GXContact::GetEdges () const
{
	return edges;
}

GXVoid GXContact::SetFaces ( GXUInt faces )
{
	this->faces = faces;
}

GXUInt GXContact::GetFaces () const
{
	return faces;
}
