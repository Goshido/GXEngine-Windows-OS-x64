//version 1.0

#include <GXPhysics/GXPlaneShape.h>


#define ANY_VALUE 777.777f


GXPlaneShape::GXPlaneShape ( GXRigidBody* body, const GXPlane& plane )
: GXShape ( eGXShapeType::Plane, body )
{
	this->plane = plane;
}

GXPlaneShape::~GXPlaneShape ()
{
	//NOTHING
}

const GXPlane& GXPlaneShape::GetPlane () const
{
	return plane;
}

GXVoid GXPlaneShape::CalculateInertiaTensor ( GXFloat /*mass*/ )
{
	inertialTensor.m[ 0 ][ 0 ] = inertialTensor.m[ 1 ][ 1 ] = inertialTensor.m[ 2 ][ 2 ] = ANY_VALUE;

	inertialTensor.m[ 0 ][ 1 ] = inertialTensor.m[ 0 ][ 2 ] = 0.0f;
	inertialTensor.m[ 1 ][ 0 ] = inertialTensor.m[ 1 ][ 2 ] = 0.0f;
	inertialTensor.m[ 2 ][ 0 ] = inertialTensor.m[ 2 ][ 1 ] = 0.0f;
}

GXVoid GXPlaneShape::GetExtremePoint ( GXVec3& /*point*/, const GXVec3& /*direction*/ ) const
{
	// NOTHING
}
