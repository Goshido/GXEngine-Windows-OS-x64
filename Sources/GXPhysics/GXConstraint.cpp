// version 1.0

#include <GXPhysics/GXConstraint.h>


const GXVec6* GXConstraint::GetJacobian () const
{
	return jacobian;
}

GXFloat GXConstraint::GetBias () const
{
	return bias;
}

const GXVec2& GXConstraint::GetLambdaRange () const
{
	return lambdaRange;
}

GXConstraint::GXConstraint ()
{
	// NOTHING
}

GXConstraint::~GXConstraint ()
{
	// NOTHING
}
