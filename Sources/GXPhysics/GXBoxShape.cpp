//version 1.0

#include <GXPhysics/GXBoxShape.h>


GXBoxShape::GXBoxShape ( GXRigidBody* body, GXFloat width, GXFloat height, GXFloat depth )
: GXShape ( eGXShapeType::Box, body )
{
	this->width = width;
	this->height = height;
	this->depth = depth;
}

GXBoxShape::~GXBoxShape ()
{
	//NOTHING
}

GXVoid GXBoxShape::CalculateInertiaTensor ( GXFloat mass )
{
	GXFloat factor = mass * 0.08333f;
	GXFloat ww = width * width;
	GXFloat hh = height * height;
	GXFloat dd = depth * depth;

	inertialTensor.m[ 0 ][ 0 ] = factor * ( hh + dd );
	inertialTensor.m[ 1 ][ 1 ] = factor * ( ww + dd );
	inertialTensor.m[ 2 ][ 2 ] = factor * ( ww + hh );

	inertialTensor.m[ 0 ][ 1 ] = inertialTensor.m[ 0 ][ 2 ] = 0.0f;
	inertialTensor.m[ 1 ][ 0 ] = inertialTensor.m[ 1 ][ 2 ] = 0.0f;
	inertialTensor.m[ 2 ][ 0 ] = inertialTensor.m[ 2 ][ 1 ] = 0.0f;
}

GXVoid GXBoxShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
	GXBoxShapeVertices v;
	GetRotatedVecticesWorld ( v );

	GXUByte index = 0;
	GXFloat projection = -FLT_MAX;

	for ( GXUByte i = 0; i < 8; i++ )
	{
		GXFloat p = direction.DotProduct ( v.vertices[ i ] );
		if ( p > projection )
		{
			projection = p;
			index = i;
		}
	}

	GXVec3 originWorld;
	transformWorld.GetW ( originWorld );
	point.Sum ( v.vertices[ index ], originWorld );
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

GXVoid GXBoxShape::GetRotatedVecticesWorld ( GXBoxShapeVertices &vertices ) const
{
	GXFloat w = width * 0.5f;
	GXFloat h = height * 0.5f;
	GXFloat d = depth * 0.5f;

	GXVec3 v[ 8 ];
	v[ 0 ].Init ( -w, -h, -d );
	v[ 1 ].Init ( w, -h, -d );
	v[ 2 ].Init ( w, h, -d );
	v[ 3 ].Init ( -w, h, -d );

	v[ 4 ].Init ( -w, -h, d );
	v[ 5 ].Init ( w, -h, d );
	v[ 6 ].Init ( w, h, d );
	v[ 7 ].Init ( -w, h, d );

	GXMat4 rotatationWorld = transformWorld;
	static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );
	rotatationWorld.SetW ( origin );

	for ( GXUByte i = 0; i < 8; i++ )
		rotatationWorld.MultiplyAsPoint ( vertices.vertices[ i ], v[ i ] );
}
