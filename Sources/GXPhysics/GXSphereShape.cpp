// version 1.2

#include <GXPhysics/GXSphereShape.h>


GXSphereShape::GXSphereShape ( GXRigidBody* body, GXFloat radius ):
    GXShape ( eGXShapeType::Sphere, body ),
    radius ( radius )
{
    _boundsLocal.AddVertex ( -radius, -radius, -radius );
    _boundsLocal.AddVertex ( radius, radius, radius );

    _boundsWorld = _boundsLocal;
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
    _inertiaTensor._m[ 0u ][ 0u ] = _inertiaTensor._m[ 1u ][ 1u ] = _inertiaTensor._m[ 2u ][ 2u ] = 0.4f * mass * radius * radius;

    _inertiaTensor._m[ 0u ][ 1u ] = _inertiaTensor._m[ 0u ][ 2u ] = 0.0f;
    _inertiaTensor._m[ 1u ][ 0u ] = _inertiaTensor._m[ 1u ][ 2u ] = 0.0f;
    _inertiaTensor._m[ 2u ][ 0u ] = _inertiaTensor._m[ 2u ][ 1u ] = 0.0f;
}

GXVoid GXSphereShape::GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const
{
    GXVec3 d ( direction );
    d.Normalize ();
    GXVec3 tmp;
    _transformWorld.GetW ( tmp );
    point.Sum ( tmp, radius, d );
}

GXVoid GXSphereShape::UpdateBoundsWorld ()
{
    _boundsWorld.Empty ();

    GXVec3 center;
    _transformWorld.GetW ( center );

    GXVec3 alpha;
    alpha.Substract ( center, GXVec3 ( radius, radius, radius ) );
    _boundsWorld.AddVertex ( alpha );

    alpha.Sum ( center, GXVec3 ( radius, radius, radius ) );
    _boundsWorld.AddVertex ( alpha );
}
