//version 1.0

#include <GXPhysics/GXPlaneShape.h>


#define ANY_VALUE 777.777f


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
	inertialTensor.m11 = inertialTensor.m22 = inertialTensor.m33 = ANY_VALUE;

	inertialTensor.m12 = inertialTensor.m13 = 0.0f;
	inertialTensor.m21 = inertialTensor.m23 = 0.0f;
	inertialTensor.m31 = inertialTensor.m32 = 0.0f;
}

GXVoid GXPlaneShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
	// NOTHING
}
