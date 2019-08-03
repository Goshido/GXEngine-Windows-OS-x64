// version 1.4

#ifndef GX_SPRING
#define GX_SPRING


#include "GXForceGenerator.h"


class GXSpring final : public GXForceGenerator
{
    private:
        GXVec3          _connectionPointLocal;
        GXVec3          _otherConnectionPointLocal;

        GXRigidBody*    _otherBody;

        GXFloat         _hardness;
        GXFloat         _restLength;

    public:
        explicit GXSpring ( const GXVec3 &connectionPointLocal, GXRigidBody &otherBody, const GXVec3 &otherConnectionPointLocal, GXFloat hardness, GXFloat restLength );
        ~GXSpring () override;

        GXVoid UpdateForce ( GXRigidBody &body, GXFloat deltaTime ) override;

    private:
        GXSpring () = delete;
        GXSpring ( const GXSpring &other ) = delete;
        GXSpring& operator = ( const GXSpring &other ) = delete;
};


#endif // GX_SPRING
