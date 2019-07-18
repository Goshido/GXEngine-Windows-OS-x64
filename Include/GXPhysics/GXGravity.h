// version 1.3

#ifndef GX_GRAVITY
#define GX_GRAVITY


#include "GXForceGenerator.h"


class GXGravity final : public GXForceGenerator
{
    private:
        GXVec3      _gravity;

    public:
        explicit GXGravity ( const GXVec3 &gravity );
        ~GXGravity () override;

        GXVoid UpdateForce ( GXRigidBody &body, GXFloat deltaTime ) override;

    private:
        GXGravity ( const GXGravity &other ) = delete;
        GXGravity& operator = ( const GXGravity &other ) = delete;
};


#endif // GX_GRAVITY
