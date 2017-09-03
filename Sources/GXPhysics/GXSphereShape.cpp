//version 1.0

#include <GXPhysics/GXSphereShape.h>


GXSphereShape::GXSphereShape ( GXRigidBody* body, GXFloat radius )
: GXShape ( eGXShapeType::Sphere, body )
{
	this->radius = radius;
}

GXSphereShape::~GXSphereShape ()
{
	//NOTHING
}

GXFloat GXSphereShape::GetRadius () const
{
	return radius;
}

GXVoid GXSphereShape::CalculateInertiaTensor ( GXFloat mass )
{
	inertialTensor.m11 = inertialTensor.m22 = inertialTensor.m33 = 0.4f * mass * radius * radius;

	inertialTensor.m12 = inertialTensor.m13 = 0.0f;
	inertialTensor.m21 = inertialTensor.m23 = 0.0f;
	inertialTensor.m31 = inertialTensor.m32 = 0.0f;
}

GXVoid GXSphereShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
	GXVec3 d = direction;
	GXNormalizeVec3 ( d );
	GXVec3 tmp;
	transformWorld.GetW ( tmp );
	GXSumVec3ScaledVec3 ( point, tmp, radius, d );
}
