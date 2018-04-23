// version 1.0

#include <GXPhysics/GXShape.h>
#include <GXPhysics/GXRigidBody.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_STATIC_FRICTION		0.9f
#define DEFAULT_DYNAMIC_FRICTION	0.4f
#define DEFAULT_RESTITUTION			0.5f


GXShape::GXShape ( eGXShapeType type, GXRigidBody* body )
{
	this->type = type;
	this->body = body;

	SetFriction ( DEFAULT_STATIC_FRICTION, DEFAULT_DYNAMIC_FRICTION );
	SetRestitution ( DEFAULT_RESTITUTION );

	transformRigidBody.Identity ();
	transformWorld.Identity ();
}

GXShape::~GXShape ()
{
	// NOTHING
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
	if ( !body ) return;

	const GXMat4& matrix = body->GetTransform ();
	transformWorld.Multiply ( transformRigidBody, matrix );

	UpdateBoundsWorld ();
}

GXRigidBody& GXShape::GetRigidBody () const
{
	return *body;
}

const GXMat3& GXShape::GetInertiaTensor () const
{
	return inertiaTensor;
}

GXVoid GXShape::SetFriction ( GXFloat newStaticFriction, GXFloat newDynamicFriction )
{
	staticFriction = newStaticFriction;
	dynamicFriction = newDynamicFriction;
}

GXFloat GXShape::GetStaticFriction () const
{
	return staticFriction;
}

GXFloat GXShape::GetDynamicFriction () const
{
	return dynamicFriction;
}

GXVoid GXShape::SetRestitution ( GXFloat newRestitution )
{
	restitution = newRestitution;
}

GXFloat GXShape::GetRestitution () const
{
	return restitution;
}

const GXAABB& GXShape::GetBoundsLocal () const
{
	return boundsLocal;
}

const GXAABB& GXShape::GetBoundsWorld () const
{
	return boundsWorld;
}
