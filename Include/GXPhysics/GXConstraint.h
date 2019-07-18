// version 1.2

#ifndef GX_CONSTRAINT
#define GX_CONSTRAINT


#include <GXCommon/GXMath.h>


class GXConstraint
{
    protected:
        GXVec6      _jacobian[ 2u ];
        GXFloat     _bias;
        GXVec2      _lambdaRange;

    public:
        const GXVec6* GetJacobian () const;
        GXFloat GetBias () const;
        const GXVec2& GetLambdaRange () const;

    protected:
        GXConstraint ();
        virtual ~GXConstraint ();

    private:
        GXConstraint ( const GXConstraint &other ) = delete;
        GXConstraint& operator = ( const GXConstraint &other ) = delete;
};


#endif // GX_CONSTRAINT
