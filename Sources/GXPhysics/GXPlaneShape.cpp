//version 1.0

#include <GXPhysics/GXPlaneShape.h>


GXPlaneShape::GXPlaneShape ( GXRigidBody* body, const GXPlane& plane )
: GXShape ( eGXShapeType::Plane, body )
{
	this->plane = plane;
}

const GXPlane& GXPlaneShape::GetPlane () const
{
	return plane;
}

GXVoid GXPlaneShape::CalculateInertiaTensor ( GXFloat /*mass*/ )
{
	inertialTensor.m11 = inertialTensor.m22 = inertialTensor.m33 = 300.0f;

	inertialTensor.m12 = inertialTensor.m13 = 0.0f;
	inertialTensor.m21 = inertialTensor.m23 = 0.0f;
	inertialTensor.m31 = inertialTensor.m32 = 0.0f;
}
