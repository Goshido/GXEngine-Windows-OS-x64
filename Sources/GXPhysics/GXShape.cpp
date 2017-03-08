//version 1.0

#include <GXPhysics/GXShape.h>
#include <GXPhysics/GXRigidBody.h>


#define DEFAULT_FRICTION		0.5f
#define DEFAULT_RESTITUTION		0.85f


GXShape::GXShape ( eGXShapeType type, GXRigidBody* body )
{
	this->type = type;
	this->body = body;

	SetFriction ( DEFAULT_FRICTION );
	SetRestitution ( DEFAULT_RESTITUTION );
}

eGXShapeType GXShape::GetType () const
{
	return type;
}

const GXMat4& GXShape::GetTransformWorld () const
{
	return transformWorld;
}

GXVoid GXShape::CalculateCacheData ()
{
	GXMulMat4Mat4 ( transformWorld, transformRigidBody, body->GetTransform () );
}

GXRigidBody* GXShape::GetRigidBody () const
{
	return body;
}

const GXMat3& GXShape::GetInertialTensor () const
{
	return inertialTensor;
}

GXVoid GXShape::SetFriction ( GXFloat friction )
{
	this->friction = friction;
}

GXFloat GXShape::GetFriction () const
{
	return friction;
}

GXVoid GXShape::SetRestitution ( GXFloat restitution )
{
	this->restitution = restitution;
}

GXFloat GXShape::GetRestitution () const
{
	return restitution;
}
