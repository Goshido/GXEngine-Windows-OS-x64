//version 1.0

#include <GXPhysics/GXRectangleShape.h>


GXRectangleShape::GXRectangleShape ( GXRigidBody* body, GXFloat width, GXFloat depth )
: GXShape ( eGXShapeType::Rectangle, body )
{
	this->width = width;
	this->depth = depth;

	GXFloat halfWidth = 0.5f * width;
	GXFloat halfDepth = 0.5f * depth;

	boundsLocal.AddVertex ( -halfWidth, 0.0f, -halfDepth );
	boundsLocal.AddVertex ( halfWidth, 0.0f, halfDepth );

	boundsWorld = boundsLocal;
}

GXRectangleShape::~GXRectangleShape ()
{
	//NOTHING
}

GXFloat GXRectangleShape::GetWidth () const
{
	return width;
}

GXFloat GXRectangleShape::GetHeight () const
{
	return depth;
}

GXVoid GXRectangleShape::CalculateInertiaTensor ( GXFloat /*mass*/ )
{
	inertiaTensor.Identity ();
}

GXVoid GXRectangleShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
	GXFloat w = width * 0.5f;
	GXFloat d = depth * 0.5f;

	GXVec3 vLocal[ 4 ];

	vLocal[ 0 ].Init ( -w, 0.0f, -d );
	vLocal[ 1 ].Init ( w, 0.0f, -d );
	vLocal[ 2 ].Init ( w, 0.0f, d );
	vLocal[ 3 ].Init ( -w, 0.0f, d );

	GXVec3 vWorld[ 4 ];
	for ( GXUByte i = 0; i < 4; i++ )
		transformWorld.MultiplyAsPoint ( vWorld[ i ], vLocal[ i ] );

	GXUByte index = 0;
	GXFloat projection = -FLT_MAX;

	for ( GXUByte i = 0; i < 8; i++ )
	{
		GXFloat p = direction.DotProduct ( vWorld[ i ] );

		if ( p > projection )
		{
			projection = p;
			index = i;
		}
	}

	point = vWorld[ index ];
}

GXVoid GXRectangleShape::UpdateBoundsWorld ()
{
	boundsLocal.Transform ( boundsWorld, transformWorld );
}
