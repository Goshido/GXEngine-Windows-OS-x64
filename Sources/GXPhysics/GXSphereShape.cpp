// version 1.0

#include <GXPhysics/GXSphereShape.h>


GXSphereShape::GXSphereShape ( GXRigidBody* body, GXFloat radius ):
	GXShape ( eGXShapeType::Sphere, body ),
	radius ( radius )
{
	boundsLocal.AddVertex ( -radius, -radius, -radius );
	boundsLocal.AddVertex ( radius, radius, radius );

	boundsWorld = boundsLocal;
}

GXSphereShape::~GXSphereShape ()
{
	// NOTHING
}

GXFloat GXSphereShape::GetRadius () const
{
	return radius;
}

GXVoid GXSphereShape::CalculateInertiaTensor ( GXFloat mass )
{
	inertiaTensor.m[ 0 ][ 0 ] = inertiaTensor.m[ 1 ][ 1 ] = inertiaTensor.m[ 2 ][ 2 ] = 0.4f * mass * radius * radius;

	inertiaTensor.m[ 0 ][ 1 ] = inertiaTensor.m[ 0 ][ 2 ] = 0.0f;
	inertiaTensor.m[ 1 ][ 0 ] = inertiaTensor.m[ 1 ][ 2 ] = 0.0f;
	inertiaTensor.m[ 2 ][ 0 ] = inertiaTensor.m[ 2 ][ 1 ] = 0.0f;
}

GXVoid GXSphereShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
	GXVec3 d ( direction );
	d.Normalize ();
	GXVec3 tmp;
	transformWorld.GetW ( tmp );
	point.Sum ( tmp, radius, d );
}

GXVoid GXSphereShape::UpdateBoundsWorld ()
{
	boundsWorld.Empty ();

	GXVec3 center;
	transformWorld.GetW ( center );

	GXVec3 alpha;
	alpha.Substract ( center, GXVec3 ( radius, radius, radius ) );
	boundsWorld.AddVertex ( alpha );

	alpha.Sum ( center, GXVec3 ( radius, radius, radius ) );
	boundsWorld.AddVertex ( alpha );
}
