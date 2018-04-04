//version 1.0

#include <GXPhysics/GXConstraint.h>


const GXSparseMatrixElement* GXConstraint::GetJacobian () const
{
	return jacobian;
}

GXFloat GXConstraint::GetBias () const
{
	return bias;
}

GXVoid GXConstraint::GetLambdaRange ( GXFloat &minimum, GXFloat &maximum ) const
{
	minimum = minimumLambda;
	maximum = maximumLambda;
}

GXConstraint::GXConstraint ()
{
	// NOTHING
}

GXConstraint::~GXConstraint ()
{
	// NOTHING
}
