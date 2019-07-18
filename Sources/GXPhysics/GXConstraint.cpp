// version 1.2

#include <GXPhysics/GXConstraint.h>


const GXVec6* GXConstraint::GetJacobian () const
{
    return _jacobian;
}

GXFloat GXConstraint::GetBias () const
{
    return _bias;
}

const GXVec2& GXConstraint::GetLambdaRange () const
{
    return _lambdaRange;
}

GXConstraint::GXConstraint ()
{
    // NOTHING
}

GXConstraint::~GXConstraint ()
{
    // NOTHING
}
