//version 1.0

#include <GXPhysics/GXConstraintSolver.h>


#define CONSTRAINT_STORAGE_GROW_FACTOR		32


GXConstraintSolver::GXConstraintSolver ( GXUShort maximumIterations ):
jacobianElements (sizeof ( GXSparseMatrixElement ) ), mapper ( sizeof ( GXUByte ) ), etaElements ( sizeof ( GXFloat ) )
{
	this->maximumIterations = maximumIterations;
	constraints = 0;
	firstBody = nullptr;
	secondBody = nullptr;
}

GXConstraintSolver::~GXConstraintSolver ()
{
	// NOTHING
}

GXVoid GXConstraintSolver::Reset ()
{
	constraints = 0;
	firstBody = secondBody = nullptr;
}

GXVoid GXConstraintSolver::SetBodies ( const GXRigidBody &first, const GXRigidBody &second )
{
	firstBody = &first;
	secondBody = &second;
}

GXVoid GXConstraintSolver::AddConstraint ( const GXConstraint& /*constraint*/ )
{
	if ( constraints >= jacobianElements.GetLength () / 2 )
	{
		GXUInt elements = constraints + CONSTRAINT_STORAGE_GROW_FACTOR * 2;
		jacobianElements.Resize ( elements );
		mapper.Resize ( elements );
		etaElements.Resize ( constraints + CONSTRAINT_STORAGE_GROW_FACTOR );
	}

	// TODO
}

GXVoid GXConstraintSolver::Solve ()
{
	// TODO
}
