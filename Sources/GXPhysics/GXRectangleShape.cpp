// version 1.2

#include <GXPhysics/GXRectangleShape.h>


GXRectangleShape::GXRectangleShape ( GXRigidBody* body, GXFloat width, GXFloat depth ):
    GXShape ( eGXShapeType::Rectangle, body ),
    _width ( width ),
    _depth ( depth )
{
    const GXFloat halfWidth = 0.5f * width;
    const GXFloat halfDepth = 0.5f * depth;

    _boundsLocal.AddVertex ( -halfWidth, 0.0f, -halfDepth );
    _boundsLocal.AddVertex ( halfWidth, 0.0f, halfDepth );

    _boundsWorld = _boundsLocal;
}

GXRectangleShape::~GXRectangleShape ()
{
    // NOTHING
}

GXFloat GXRectangleShape::GetWidth () const
{
    return _width;
}

GXFloat GXRectangleShape::GetHeight () const
{
    return _depth;
}

GXVoid GXRectangleShape::CalculateInertiaTensor ( GXFloat /*mass*/ )
{
    _inertiaTensor.Identity ();
}

GXVoid GXRectangleShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
    const GXFloat w = 0.5f * _width;
    const GXFloat d = 0.5f * _depth;

    GXVec3 vLocal[ 4u ];

    vLocal[ 0u ].Init ( -w, 0.0f, -d );
    vLocal[ 1u ].Init ( w, 0.0f, -d );
    vLocal[ 2u ].Init ( w, 0.0f, d );
    vLocal[ 3u ].Init ( -w, 0.0f, d );

    GXVec3 vWorld[ 4u ];

    for ( GXUByte i = 0u; i < 4u; ++i )
        _transformWorld.MultiplyAsPoint ( vWorld[ i ], vLocal[ i ] );

    GXUByte index = 0u;
    GXFloat projection = -FLT_MAX;

    for ( GXUByte i = 0u; i < 8u; ++i )
    {
        GXFloat p = direction.DotProduct ( vWorld[ i ] );

        if ( p <= projection ) continue;

        projection = p;
        index = i;
    }

    point = vWorld[ index ];
}

GXVoid GXRectangleShape::UpdateBoundsWorld ()
{
    _boundsLocal.Transform ( _boundsWorld, _transformWorld );
}
