// version 1.2

#ifndef GX_FORCE_GENERATOR
#define GX_FORCE_GENERATOR


#include "GXRigidBody.h"


class GXForceGenerator
{
    public:
        GXForceGenerator ();
        virtual ~GXForceGenerator ();

        virtual GXVoid UpdateForce ( GXRigidBody &body, GXFloat deltaTime ) = 0;

    private:
        GXForceGenerator ( const GXForceGenerator &other ) = delete;
        GXForceGenerator& operator = ( const GXForceGenerator &other ) = delete;
};


#endif // GX_FORCE_GENERATOR
