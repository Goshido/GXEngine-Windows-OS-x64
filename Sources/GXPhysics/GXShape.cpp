//version 1.0

#include <GXPhysics/GXShape.h>


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
