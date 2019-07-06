// version 1.1

#include <GXPhysics/GXShape.h>
#include <GXPhysics/GXRigidBody.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_STATIC_FRICTION     0.9f
#define DEFAULT_DYNAMIC_FRICTION    0.4f
#define DEFAULT_RESTITUTION         0.5f


GXShape::GXShape ( eGXShapeType type, GXRigidBody* body ):
    _type ( type ),
    _body ( body )
{

    SetFriction ( DEFAULT_STATIC_FRICTION, DEFAULT_DYNAMIC_FRICTION );
    SetRestitution ( DEFAULT_RESTITUTION );

    _transformRigidBody.Identity ();
    _transformWorld.Identity ();
}

GXShape::~GXShape ()
{
    // NOTHING
}

eGXShapeType GXShape::GetType () const
{
    return _type;
}

const GXMat4& GXShape::GetTransformWorld () const
{
    return _transformWorld;
}

GXVoid GXShape::CalculateCacheData ()
{
    if ( !_body ) return;

    const GXMat4& matrix = _body->GetTransform ();
    _transformWorld.Multiply ( _transformRigidBody, matrix );

    UpdateBoundsWorld ();
}

GXRigidBody& GXShape::GetRigidBody () const
{
    return *_body;
}

const GXMat3& GXShape::GetInertiaTensor () const
{
    return _inertiaTensor;
}

GXVoid GXShape::SetFriction ( GXFloat staticFriction, GXFloat dynamicFriction )
{
    _staticFriction = staticFriction;
    _dynamicFriction = dynamicFriction;
}

GXFloat GXShape::GetStaticFriction () const
{
    return _staticFriction;
}

GXFloat GXShape::GetDynamicFriction () const
{
    return _dynamicFriction;
}

GXVoid GXShape::SetRestitution ( GXFloat restitution )
{
    _restitution = restitution;
}

GXFloat GXShape::GetRestitution () const
{
    return _restitution;
}

const GXAABB& GXShape::GetBoundsLocal () const
{
    return _boundsLocal;
}

const GXAABB& GXShape::GetBoundsWorld () const
{
    return _boundsWorld;
}
