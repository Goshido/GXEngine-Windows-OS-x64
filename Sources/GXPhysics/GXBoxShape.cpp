//version 1.0

#include <GXPhysics/GXBoxShape.h>

GXBoxShape::GXBoxShape ( GXRigidBody* body, GXFloat width, GXFloat height, GXFloat depth )
: GXShape ( eGXShapeType::Box, body )
{
	this->width = width;
	this->height = height;
	this->depth = depth;
}

GXVoid GXBoxShape::CalculateInertiaTensor ( GXFloat mass )
{
	GXFloat factor = mass * 0.08333f;
	GXFloat ww = width * width;
	GXFloat hh = height * height;
	GXFloat dd = depth * depth;

	inertialTensor.m11 = factor * ( hh + dd );
	inertialTensor.m22 = factor * ( ww + dd );
	inertialTensor.m33 = factor * ( ww + hh );

	inertialTensor.m12 = inertialTensor.m13 = 0.0f;
	inertialTensor.m21 = inertialTensor.m23 = 0.0f;
	inertialTensor.m31 = inertialTensor.m32 = 0.0f;
}

GXVoid GXBoxShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
	GXBoxShapeVertices v;
	GetVecticesWorld ( v );

	GXUByte index = 0;
	GXFloat projection = -FLT_MIN;

	for ( GXUByte i = 0; i < 8; i++ )
	{
		GXFloat p = GXDotVec3Fast ( direction, v.vertices[ i ] );
		if ( p > projection )
		{
			projection = p;
			index = i;
		}
	}

	point = v.vertices[ index ];
}

GXFloat GXBoxShape::GetWidth () const
{
	return width;
}

GXFloat GXBoxShape::GetHeight () const
{
	return height;
}

GXFloat GXBoxShape::GetDepth () const
{
	return depth;
}

GXVoid GXBoxShape::GetVecticesWorld ( GXBoxShapeVertices &vertices ) const
{
	GXFloat w = width * 0.5f;
	GXFloat h = height * 0.5f;
	GXFloat d = depth * 0.5f;

	GXVec3 v[ 8 ];
	v[ 0 ] = GXCreateVec3 ( -w, -h, -d );
	v[ 1 ] = GXCreateVec3 ( w, -h, -d );
	v[ 2 ] = GXCreateVec3 ( w, h, -d );
	v[ 3 ] = GXCreateVec3 ( -w, h, -d );

	v[ 4 ] = GXCreateVec3 ( -w, -h, d );
	v[ 5 ] = GXCreateVec3 ( w, -h, d );
	v[ 6 ] = GXCreateVec3 ( w, h, d );
	v[ 7 ] = GXCreateVec3 ( -w, h, d );

	for ( GXUByte i = 0; i < 8; i++ )
		GXMulVec3Mat4AsPoint ( vertices.vertices[ i ], v[ i ], transformWorld );
}
