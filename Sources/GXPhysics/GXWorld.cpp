// version 1.3

#include <GXPhysics/GXWorld.h>
#include <GXPhysics/GXContactGenerator.h>
#include <GXPhysics/GXSphereShape.h>
#include <GXCommon/GXLogger.h>


struct GXRigidBodyRegistration final
{
    GXRigidBodyRegistration*    _next;
    GXRigidBodyRegistration*    _previous;

    GXRigidBody*                _body;
};

//----------------------------------------------------------------

struct GXContactGeneratorsRegistration final
{
    GXContactGeneratorsRegistration*    _next;
    GXContactGeneratorsRegistration*    _previous;

    GXContactGenerator*                 _generator;
};

//----------------------------------------------------------------

struct GXForceGeneratorsRegistration final
{
    GXForceGeneratorsRegistration*      _next;
    GXForceGeneratorsRegistration*      _previous;

    GXForceGenerator*                   _generator;
    GXRigidBody*                        _body;
};

//----------------------------------------------------------------

GXWorld::GXWorld ( GXUInt maxContacts, GXUInt /*iterations*/ ):
    _collisions ( maxContacts ),
    _bodies ( nullptr ),
    _contactGenerators ( nullptr ),
    _forceGenerators ( nullptr ),
    _isCalculateIterations ( GX_FALSE )
{
    GXCollisionDetector::GetInstance ();
}

GXWorld::~GXWorld ()
{
    delete &( GXCollisionDetector::GetInstance () );

    ClearRigidBodyRegistrations ();
    ClearForceGeneratorRegistrations ();
}

GXVoid GXWorld::RegisterRigidBody ( GXRigidBody &body )
{
    GXRigidBodyRegistration* reg = new GXRigidBodyRegistration ();
    reg->_body = &body;
    reg->_next = nullptr;
    reg->_previous = nullptr;

    _smartLock.AcquireExclusive ();

    if ( _bodies )
        _bodies->_previous = reg;

    reg->_next = _bodies;
    _bodies = reg;

    _smartLock.ReleaseExclusive ();
}

GXVoid GXWorld::UnregisterRigidBody ( GXRigidBody &body )
{
    _smartLock.AcquireExclusive ();

    GXRigidBodyRegistration* reg = FindRigidBodyRegistration ( body );

    if ( !reg )
    {
        GXLogA ( "GXWorld::UnregisterRigidBody::Error - Can't find rigid body!" );
        _smartLock.ReleaseExclusive ();
        return;
    }

    if ( reg == _bodies )
    {
        if ( !reg->_next )
        {
            _bodies = nullptr;
        }
        else
        {
            _bodies = _bodies->_next;
            _bodies->_previous = nullptr;
        }

        delete reg;
    }
    else if ( !reg->_next )
    {
        reg->_previous->_next = nullptr;
        delete reg;
    }
    else
    {
        reg->_next->_previous = reg->_previous;
        reg->_previous->_next = reg->_next;

        delete reg;
    }

    _smartLock.ReleaseExclusive ();
}

GXVoid GXWorld::ClearRigidBodyRegistrations ()
{
    _smartLock.AcquireExclusive ();

    GXRigidBodyRegistration* toDelete = _bodies;
    _bodies = nullptr;

    _smartLock.ReleaseExclusive ();

    while ( toDelete )
    {
        GXRigidBodyRegistration* reg = toDelete;
        toDelete = toDelete->_next;
        delete reg;
    }
}

GXVoid GXWorld::RegisterForceGenerator ( GXRigidBody &body, GXForceGenerator &generator )
{
    GXForceGeneratorsRegistration* reg = new GXForceGeneratorsRegistration ();
    reg->_body = &body;
    reg->_generator = &generator;
    reg->_next = nullptr;
    reg->_previous = nullptr;

    _smartLock.AcquireExclusive ();

    if ( _forceGenerators )
        _forceGenerators->_previous = reg;

    reg->_next = _forceGenerators;
    _forceGenerators = reg;

    _smartLock.ReleaseExclusive ();
}

GXVoid GXWorld::UnregisterForceGenerator ( GXRigidBody &body, GXForceGenerator &generator )
{
    GXForceGeneratorsRegistration* reg = FindForceGeneratorRegistration ( body, generator );

    _smartLock.AcquireExclusive ();

    if ( !reg )
    {
        GXLogA ( "GXWorld::UnregisterForceGenerator::Error - Can't find force generator!" );
        _smartLock.ReleaseExclusive ();
        return;
    }

    if ( reg == _forceGenerators )
    {
        if ( !reg->_next )
        {
            _forceGenerators = nullptr;
        }
        else
        {
            _forceGenerators = _forceGenerators->_next;
            _forceGenerators->_previous = nullptr;
        }

        delete reg;
    }
    else if ( !reg->_next )
    {
        reg->_previous->_next = nullptr;
        delete reg;
    }
    else
    {
        reg->_next->_previous = reg->_previous;
        reg->_previous->_next = reg->_next;

        delete reg;
    }

    _smartLock.ReleaseExclusive ();
}

GXVoid GXWorld::ClearForceGeneratorRegistrations ()
{
    _smartLock.AcquireExclusive ();

    GXForceGeneratorsRegistration* toDelete = _forceGenerators;
    _forceGenerators = nullptr;

    _smartLock.ReleaseExclusive ();

    while ( toDelete )
    {
        GXForceGeneratorsRegistration* reg = toDelete;
        toDelete = toDelete->_next;
        delete reg;
    }
}

GXVoid GXWorld::RunPhysics ( GXFloat deltaTime )
{
    _collisions.Reset ();

    _smartLock.AcquireShared ();

    // We do not do anything unless 2 rigid bodies were registered
    if ( !_bodies || !_bodies->_next )
    {
        _smartLock.ReleaseShared ();
        return;
    }

    for ( GXRigidBodyRegistration* p = _bodies; p; p = p->_next )
        p->_body->ClearAccumulators ();

    for ( GXForceGeneratorsRegistration* p = _forceGenerators; p; p = p->_next )
        p->_generator->UpdateForce ( *p->_body, deltaTime );

    for ( GXRigidBodyRegistration* p = _bodies; p; p = p->_next )
        p->_body->Integrate ( deltaTime );

    for ( GXContactGeneratorsRegistration* reg = _contactGenerators; reg; reg = reg->_next )
    {
        reg->_generator->AddContact ( _collisions );

        if ( !_collisions.HasMoreContacts () ) break;
    }

    GXCollisionDetector& collisionDetector = GXCollisionDetector::GetInstance ();

    for ( GXRigidBodyRegistration* p = _bodies; p->_next; p = p->_next )
    {
        for ( GXRigidBodyRegistration* q = p->_next; q; q = q->_next )
        {
            if ( p->_body->IsKinematic () && q->_body->IsKinematic () ) continue;

            collisionDetector.Check ( p->_body->GetShape (), q->_body->GetShape (), _collisions );
        }
    }

    _contactResolver.ResolveContacts ( _collisions.GetAllContacts (), _collisions.GetTotalContacts () );
    _smartLock.ReleaseShared ();
}

const GXCollisionData& GXWorld::GetCollisionData () const
{
    return _collisions;
}

GXBool GXWorld::Raycast ( const GXVec3 &origin, const GXVec3 &direction, GXFloat length, GXVec3 &contactLocation, GXVec3 &contactNormal, const GXShape** contactShape )
{
    const GXFloat inverseX = 1.0f / direction.GetX ();
    const GXFloat inverseY = 1.0f / direction.GetY ();
    const GXFloat inverseZ = 1.0f / direction.GetZ ();

    GXBool foundIntersection = GX_FALSE;
    GXFloat intersectionFactor = 0.0f;

    _smartLock.AcquireShared ();

    for ( GXRigidBodyRegistration* p = _bodies; p != nullptr; p = p->_next )
    {
        const GXShape& shape = p->_body->GetShape ();
        const GXAABB& shapeBounds = shape.GetBoundsWorld ();

        const GXFloat t1 = ( shapeBounds._min.GetX () - origin.GetX () ) * inverseX;
        const GXFloat t2 = ( shapeBounds._max.GetX () - origin.GetX () ) * inverseX;
        const GXFloat t3 = ( shapeBounds._min.GetY () - origin.GetY () ) * inverseY;
        const GXFloat t4 = ( shapeBounds._max.GetY () - origin.GetY () ) * inverseY;
        const GXFloat t5 = ( shapeBounds._min.GetZ () - origin.GetZ () ) * inverseZ;
        const GXFloat t6 = ( shapeBounds._max.GetZ () - origin.GetZ () ) * inverseZ;

        const GXFloat tMin = GXMaxf ( GXMaxf ( GXMinf ( t1, t2 ), GXMinf ( t3, t4 ) ), GXMinf ( t5, t6 ) );
        const GXFloat tMax = GXMinf ( GXMinf ( GXMaxf ( t1, t2 ), GXMaxf ( t3, t4 ) ), GXMaxf ( t5, t6 ) );

        if ( tMax < 0.0f ) continue;

        if ( tMin > tMax || tMin > length ) continue;

        const GXMat4& shapeTransform = shape.GetTransformWorld ();

        switch ( shape.GetType () )
        {
            case eGXShapeType::Sphere:
            {
                const GXSphereShape& sphereShape = static_cast<const GXSphereShape&> ( shape );

                GXVec3 sphereCenter;
                shapeTransform.GetW ( sphereCenter );

                GXVec3 alpha;
                alpha.Substract ( sphereCenter, origin );

                const GXFloat betta = alpha.DotProduct ( direction );

                if ( betta < 0.0f ) break;

                const GXFloat gamma = alpha.DotProduct ( alpha ) - betta * betta;
                GXFloat radiusSquare = sphereShape.GetRadius ();
                radiusSquare *= radiusSquare;

                if ( gamma > radiusSquare ) break;

                const GXFloat yotta = sqrtf ( radiusSquare - gamma );
                GXFloat intersection0 = betta - yotta;
                GXFloat intersection1 = betta + yotta;

                if ( intersection0 > intersection1 )
                {
                    const GXFloat tmp = intersection0;
                    intersection0 = intersection1;
                    intersection1 = tmp;
                }

                if ( intersection0 < 0.0f )
                {
                    intersection0 = intersection1;

                    if ( intersection0 < 0.0f ) break;
                }

                if ( !foundIntersection )
                {
                    foundIntersection = GX_TRUE;
                    intersectionFactor = intersection0;

                    contactLocation.Sum ( origin, intersection0, direction );
                    contactNormal.Substract ( contactLocation, sphereCenter );
                    contactNormal.Normalize ();
                    *contactShape = &shape;

                    break;
                }

                if ( intersectionFactor < intersection0 ) break;

                intersectionFactor = intersection0;

                contactLocation.Sum ( origin, intersection0, direction );
                contactNormal.Substract ( contactLocation, sphereCenter );
                contactNormal.Normalize ();
                *contactShape = &shape;
            }
            break;

            case eGXShapeType::Box:
            {
                const GXAABB boundsLocal ( shape.GetBoundsLocal () );

                GXVec3 geometryLocal[ 8u ];
                geometryLocal[ 0u ] = boundsLocal._min;
                geometryLocal[ 1u ].Init ( boundsLocal._min.GetX (), boundsLocal._min.GetY (), boundsLocal._max.GetZ () );
                geometryLocal[ 2u ].Init ( boundsLocal._max.GetX (), boundsLocal._min.GetY (), boundsLocal._max.GetZ () );
                geometryLocal[ 3u ].Init ( boundsLocal._max.GetX (), boundsLocal._min.GetY (), boundsLocal._min.GetZ () );
                geometryLocal[ 4u ].Init ( boundsLocal._min.GetX (), boundsLocal._max.GetY (), boundsLocal._min.GetZ () );
                geometryLocal[ 5u ].Init ( boundsLocal._min.GetX (), boundsLocal._max.GetY (), boundsLocal._max.GetZ () );
                geometryLocal[ 6u ] = boundsLocal._max;
                geometryLocal[ 7u ].Init ( boundsLocal._max.GetX (), boundsLocal._max.GetY (), boundsLocal._min.GetZ () );

                constexpr GXUByte indices[ 36u ] =
                {
                    0u, 3u, 1u,
                    1u, 3u, 2u,
                    1u, 5u, 0u,
                    0u, 5u, 4u,
                    2u, 6u, 1u,
                    1u, 6u, 5u,
                    3u, 7u, 2u,
                    2u, 7u, 6u,
                    0u, 4u, 3u,
                    3u, 4u, 7u,
                    4u, 5u, 7u,
                    7u, 5u, 6u
                };

                GXVec3 geometryWorld[ 8u ];

                for ( GXUByte i = 0u; i < 8u; ++i )
                    shapeTransform.MultiplyAsPoint ( geometryWorld[ i ], geometryLocal[ i ] );

                constexpr GXUByte totalTriangles = 12u;
                GXUByte offset = 0u;

                for ( GXUByte i = 0u; i < totalTriangles; ++i )
                {
                    GXFloat t;
                    const GXVec3& a = geometryWorld[ indices[ offset ] ];
                    const GXVec3& b = geometryWorld[ indices[ offset + 1 ] ];
                    const GXVec3& c = geometryWorld[ indices[ offset + 2 ] ];

                    offset += 3u;

                    if ( !GXRayTriangleIntersection3D ( t, origin, direction, length, a, b, c ) ) continue;

                    if ( !foundIntersection )
                    {
                        foundIntersection = GX_TRUE;
                        intersectionFactor = t;

                        contactLocation.Sum ( origin, t, direction );

                        GXPlane plane;
                        plane.From ( a, b, c );

                        contactNormal.Init ( plane._a, plane._b, plane._c );
                        *contactShape = &shape;

                        continue;
                    }

                    if ( t > intersectionFactor ) continue;

                    intersectionFactor = t;
                    contactLocation.Sum ( origin, t, direction );

                    GXPlane plane;
                    plane.From ( a, b, c );

                    contactNormal.Init ( plane._a, plane._b, plane._c );
                    *contactShape = &shape;
                }
            }
            break;

            case eGXShapeType::Rectangle:
                // TODO
            break;
        }
    }

    _smartLock.ReleaseShared ();

    return foundIntersection;
}

GXRigidBodyRegistration* GXWorld::FindRigidBodyRegistration ( GXRigidBody &body )
{
    for ( GXRigidBodyRegistration* p = _bodies; p; p = p->_next )
    {
        if ( p->_body != &body ) continue;

        return p;
    }

    return nullptr;
}

GXForceGeneratorsRegistration* GXWorld::FindForceGeneratorRegistration ( GXRigidBody &body, GXForceGenerator &generator )
{
    for ( GXForceGeneratorsRegistration* p = _forceGenerators; p; p = p->_next )
    {
        if ( p->_body == &body && p->_generator == &generator )
        {
            return p;
        }
    }

    return nullptr;
}
