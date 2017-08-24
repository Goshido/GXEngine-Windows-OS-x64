//version 1.0

#include <GXPhysics/GXPolygonShape.h>

GXPolygonShape::GXPolygonShape ( GXRigidBody* body, GXFloat widht, GXFloat depth )
: GXShape ( eGXShapeType::Polygon, body )
{
	this->width = widht;
	this->depth = depth;
}

GXPolygonShape::~GXPolygonShape ()
{
	//NOTHING
}

GXFloat GXPolygonShape::GetWidth () const
{
	return width;
}

GXFloat GXPolygonShape::GetHeight () const
{
	return depth;
}

GXVoid GXPolygonShape::CalculateInertiaTensor ( GXFloat mass )
{
	GXSetMat3Identity ( inertialTensor );
}

GXVoid GXPolygonShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
	GXFloat w = width * 0.5f;
	GXFloat d = depth * 0.5f;

	GXVec3 vLocal[ 4 ];

	vLocal[ 0 ] = GXCreateVec3 ( -w, 0.0f, -d );
	vLocal[ 1 ] = GXCreateVec3 ( w, 0.0f, -d );
	vLocal[ 2 ] = GXCreateVec3 ( w, 0.0f, d );
	vLocal[ 3 ] = GXCreateVec3 ( -w, 0.0f, d );

	GXVec3 vWorld[ 4 ];
	for ( GXUByte i = 0; i < 4; i++ )
		GXMulVec3Mat4AsPoint ( vWorld[ i ], vLocal[ 1 ], transformWorld );

	GXUByte index = 0;
	GXFloat projection = -FLT_MAX;

		for ( GXUByte i = 0; i < 8; i++ )
	{
		GXFloat p = GXDotVec3 ( direction, vWorld[ i ] );
		if ( p > projection )
		{
			projection = p;
			index = i;
		}
	}

	point = vWorld[ index ];
}
