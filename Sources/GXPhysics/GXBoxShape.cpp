// version 1.2

#include <GXPhysics/GXBoxShape.h>
#include <GXCommon/GXLogger.h>


GXBoxShape::GXBoxShape ( GXRigidBody* body, GXFloat width, GXFloat height, GXFloat depth ):
    GXShape ( eGXShapeType::Box, body ),
    _width ( width ),
    _height ( height ),
    _depth ( depth )
{
    const GXFloat halfWidth = 0.5f * width;
    const GXFloat halfHeight = 0.5f * height;
    const GXFloat halfDepth = 0.5f * depth;

    _boundsLocal.AddVertex ( -halfWidth, -halfHeight, -halfDepth );
    _boundsLocal.AddVertex ( halfWidth, halfHeight, halfDepth );

    _boundsWorld = _boundsLocal;
}

GXBoxShape::~GXBoxShape ()
{
    // NOTHING
}

GXVoid GXBoxShape::CalculateInertiaTensor ( GXFloat mass )
{
    const GXFloat factor = mass * 0.08333f;
    const GXFloat ww = _width * _width;
    const GXFloat hh = _height * _height;
    const GXFloat dd = _depth * _depth;

    _inertiaTensor._m[ 0u ][ 0u ] = factor * ( hh + dd );
    _inertiaTensor._m[ 1u ][ 1u ] = factor * ( ww + dd );
    _inertiaTensor._m[ 2u ][ 2u ] = factor * ( ww + hh );

    _inertiaTensor._m[ 0u ][ 1u ] = _inertiaTensor._m[ 0u ][ 2u ] = 0.0f;
    _inertiaTensor._m[ 1u ][ 0u ] = _inertiaTensor._m[ 1u ][ 2u ] = 0.0f;
    _inertiaTensor._m[ 2u ][ 0u ] = _inertiaTensor._m[ 2u ][ 1u ] = 0.0f;
}

GXVoid GXBoxShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
    GXBoxShapeVertices v;
    GetRotatedVecticesWorld ( v );

    GXUByte index = 0u;
    GXFloat projection = -FLT_MAX;

    for ( GXUByte i = 0u; i < 8u; ++i )
    {
        GXFloat p = direction.DotProduct ( v._vertices[ i ] );

        if ( p <= projection ) continue;

        projection = p;
        index = i;
    }

    GXVec3 originWorld;
    _transformWorld.GetW ( originWorld );
    point.Sum ( v._vertices[ index ], originWorld );
}

GXFloat GXBoxShape::GetWidth () const
{
    return _width;
}

GXFloat GXBoxShape::GetHeight () const
{
    return _height;
}

GXFloat GXBoxShape::GetDepth () const
{
    return _depth;
}

GXVoid GXBoxShape::UpdateBoundsWorld ()
{
    _boundsLocal.Transform ( _boundsWorld, _transformWorld );
}

GXVoid GXBoxShape::GetRotatedVecticesWorld ( GXBoxShapeVertices &vertices ) const
{
    const GXFloat w = 0.5f * _width;
    const GXFloat h = 0.5f * _height;
    const GXFloat d = 0.5f * _depth;

    GXVec3 v[ 8u ];
    v[ 0u ].Init ( -w, -h, -d );
    v[ 1u ].Init ( w, -h, -d );
    v[ 2u ].Init ( w, h, -d );
    v[ 3u ].Init ( -w, h, -d );

    v[ 4u ].Init ( -w, -h, d );
    v[ 5u ].Init ( w, -h, d );
    v[ 6u ].Init ( w, h, d );
    v[ 7u ].Init ( -w, h, d );

    GXMat4 rotatationWorld ( _transformWorld );
    constexpr GXVec3 origin ( 0.0f, 0.0f, 0.0f );
    rotatationWorld.SetW ( origin );

    for ( GXUByte i = 0u; i < 8u; ++i )
    {
        rotatationWorld.MultiplyAsPoint ( vertices._vertices[ i ], v[ i ] );
    }
}
