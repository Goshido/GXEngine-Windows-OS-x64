// version 1.3

#ifndef GX_WORLD
#define GX_WORLD


#include "GXCollisionData.h"
#include "GXCollisionDetector.h"
#include "GXContactResolver.h"
#include "GXForceGenerator.h"
#include "GXRigidBody.h"
#include <GXCommon/GXSmartLock.h>


struct GXContactGeneratorsRegistration;
struct GXForceGeneratorsRegistration;
struct GXRigidBodyRegistration;

class GXWorld final
{
    private:
        GXCollisionData                     _collisions;
        GXRigidBodyRegistration*            _bodies;
        GXContactGeneratorsRegistration*    _contactGenerators;
        GXForceGeneratorsRegistration*      _forceGenerators;
        GXBool                              _isCalculateIterations;

        GXContactResolver                   _contactResolver;
        GXSmartLock                         _smartLock;

    public:
        explicit GXWorld ( GXUInt maxContacts, GXUInt iterations );
        ~GXWorld ();

        // Note methods are thread safe.
        GXVoid RegisterRigidBody ( GXRigidBody &body );
        GXVoid UnregisterRigidBody ( GXRigidBody &body );
        GXVoid ClearRigidBodyRegistrations ();

        // Note methods are thread safe.
        GXVoid RegisterForceGenerator ( GXRigidBody &body, GXForceGenerator &generator );
        GXVoid UnregisterForceGenerator ( GXRigidBody &body, GXForceGenerator &generator );
        GXVoid ClearForceGeneratorRegistrations ();

        GXVoid RunPhysics ( GXFloat deltaTime );

        const GXCollisionData& GetCollisionData () const;

        GXBool Raycast ( const GXVec3 &origin, const GXVec3 &direction, GXFloat length, GXVec3 &contactLocation, GXVec3 &contactNormal, const GXShape** shape );

    private:
        GXRigidBodyRegistration* FindRigidBodyRegistration ( GXRigidBody &body );
        GXForceGeneratorsRegistration* FindForceGeneratorRegistration ( GXRigidBody &body, GXForceGenerator &generator );

        GXWorld ( const GXWorld &other ) = delete;
        GXWorld& operator = ( const GXWorld &other ) = delete;
};


#endif // GX_WORLD
