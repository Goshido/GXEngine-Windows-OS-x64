//version 1.0

#include <GXPhysics/GXShape.h>
#include <GXPhysics/GXRigidBody.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_FRICTION		0.5f
#define DEFAULT_RESTITUTION		0.4f


GXShape::GXShape ( eGXShapeType type, GXRigidBody* body )
{
	this->type = type;
	this->body = body;

	SetFriction ( DEFAULT_FRICTION );
	SetRestitution ( DEFAULT_RESTITUTION );

	GXSetMat4Identity ( transformRigidBody );
	GXSetMat4Identity ( transformWorld );
}

GXShape::~GXShape ()
{
	GXUInt r = 0;
	GXLogW ( L"%u\n", r );
	//NOTHING
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
	GXMulMat4Mat4 ( transformWorld, transformRigidBody, matrix );
}

GXRigidBody* GXShape::GetRigidBody () const
{
	return body;
}

const GXMat3& GXShape::GetInertialTensor () const
{
	return inertialTensor;
}

GXVoid GXShape::SetFriction ( GXFloat newFriction )
{
	friction = newFriction;
}

GXFloat GXShape::GetFriction () const
{
	return friction;
}

GXVoid GXShape::SetRestitution ( GXFloat newRestitution )
{
	restitution = newRestitution;
}

GXFloat GXShape::GetRestitution () const
{
	return restitution;
}
