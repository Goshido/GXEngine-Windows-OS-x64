// version 1.2

#ifndef GX_BOX_SHAPE
#define GX_BOX_SHAPE


#include "GXShape.h"


struct GXBoxShapeVertices final
{
    GXVec3      _vertices[ 8u ];
};

class GXBoxShape final : public GXShape
{
    private:
        GXFloat     _width;
        GXFloat     _height;
        GXFloat     _depth;

    public:
        explicit GXBoxShape ( GXRigidBody* body, GXFloat width, GXFloat height, GXFloat depth );
        ~GXBoxShape () override;

        GXVoid CalculateInertiaTensor ( GXFloat mass ) override;
        GXVoid GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const override;

        GXFloat GetWidth () const;
        GXFloat GetHeight () const;
        GXFloat GetDepth () const;

    protected:
        GXVoid UpdateBoundsWorld () override;

    private:
        GXVoid GetRotatedVecticesWorld ( GXBoxShapeVertices &vertices ) const;

        GXBoxShape ( const GXBoxShape &other ) = delete;
        GXBoxShape& operator = ( const GXBoxShape &other ) = delete;
};


#endif // GX_BOX_SHAPE
