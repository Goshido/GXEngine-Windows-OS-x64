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
	inertialTensor.m[ 0 ][ 0 ] = inertialTensor.m[ 1 ][ 1 ] = inertialTensor.m[ 2 ][ 2 ] = 0.4f * mass * radius * radius;

	inertialTensor.m[ 0 ][ 1 ] = inertialTensor.m[ 0 ][ 2 ] = 0.0f;
	inertialTensor.m[ 1 ][ 0 ] = inertialTensor.m[ 1 ][ 2 ] = 0.0f;
	inertialTensor.m[ 2 ][ 0 ] = inertialTensor.m[ 2 ][ 1 ] = 0.0f;
}

GXVoid GXSphereShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
	GXVec3 d = direction;
	d.Normalize ();
	GXVec3 tmp;
	transformWorld.GetW ( tmp );
	point.Sum ( tmp, radius, d );
}
