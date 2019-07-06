// version 1.1

#ifndef GX_SHAPE
#define GX_SHAPE


#include <GXCommon/GXMath.h>


enum class eGXShapeType : GXUByte
{
    Sphere = 0u,
    Box = 1u,
    Rectangle = 2u
};

class GXRigidBody;
class GXShape
{
    protected:
        eGXShapeType    _type;
        GXRigidBody*    _body;

        GXMat4          _transformRigidBody;
        GXMat4          _transformWorld;
        GXMat3          _inertiaTensor;

        GXFloat         _staticFriction;
        GXFloat         _dynamicFriction;
        GXFloat         _restitution;

        GXAABB          _boundsLocal;
        GXAABB          _boundsWorld;

    public:
        explicit GXShape ( eGXShapeType type, GXRigidBody* body );
        virtual ~GXShape ();

        eGXShapeType GetType () const;

        const GXMat4& GetTransformWorld () const;
        GXVoid CalculateCacheData ();
        GXRigidBody& GetRigidBody () const;

        virtual GXVoid CalculateInertiaTensor ( GXFloat mass ) = 0;
        const GXMat3& GetInertiaTensor () const;

        virtual GXVoid GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const = 0;

        GXVoid SetFriction ( GXFloat staticFriction, GXFloat dynamicFriction );
        GXFloat GetStaticFriction () const;
        GXFloat GetDynamicFriction () const;

        GXVoid SetRestitution ( GXFloat restitution );
        GXFloat GetRestitution () const;

        const GXAABB& GetBoundsLocal () const;
        const GXAABB& GetBoundsWorld () const;

    protected:
        virtual GXVoid UpdateBoundsWorld () = 0;

    private:
        GXShape ( const GXShape &other ) = delete;
        GXShape& operator = ( const GXShape &other ) = delete;
};


#endif // GX_SHAPE
