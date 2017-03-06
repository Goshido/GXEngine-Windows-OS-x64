//version 1.0

#include <GXPhysics/GXContact.h>


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


