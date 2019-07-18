// version 1.2

#ifndef GX_SPHERE_SHAPE
#define GX_SPHERE_SHAPE


#include "GXShape.h"


class GXSphereShape final : public GXShape
{
    private:
        GXFloat     radius;

    public:
        explicit GXSphereShape ( GXRigidBody* body, GXFloat radius );
        ~GXSphereShape () override;

        GXFloat GetRadius () const;

        GXVoid CalculateInertiaTensor ( GXFloat mass ) override;
        GXVoid GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const override;

    protected:
        GXVoid UpdateBoundsWorld () override;

    private:
        GXSphereShape ( const GXSphereShape &other ) = delete;
        GXSphereShape& operator = ( const GXSphereShape &other ) = delete;
};


#endif // GX_SPHERE_SHAPE
