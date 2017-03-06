//version 1.0

#include <GXPhysics/GXBoxShape.h>

GXBoxShape::GXBoxShape ( GXFloat width, GXFloat height, GXFloat depth )
{
	this->width = width;
	this->height = height;
	this->depth = depth;
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
