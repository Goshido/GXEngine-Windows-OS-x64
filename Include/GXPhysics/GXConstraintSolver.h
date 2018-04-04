//version 1.0

#ifndef GX_CONSTRAINT_SOLVER
#define GX_CONSTRAINT_SOLVER


#include "GXConstraint.h"
#include "GXRigidBody.h"
#include <GXCommon/GXMemory.h>


//Implementation from paper
//"Iterative Dynamics with Temporal Coherence"
//by Erin Catto

class GXConstraintSolver
{
	private:
		GXDynamicArray				jacobianElements;
		GXDynamicArray				mapper;

		// TODO working arrays
		GXDynamicArray				etaElements;

		GXUShort					maximumIterations;
		GXUInt						constraints;
		const GXRigidBody*			firstBody;
		const GXRigidBody*			secondBody;

	public:
		explicit GXConstraintSolver ( GXUShort maximumIterations );
		~GXConstraintSolver ();

		GXVoid Reset ();

		GXVoid SetBodies ( const GXRigidBody &first, const GXRigidBody &second );
		GXVoid AddConstraint ( const GXConstraint &constraint );

		GXVoid Solve ();
};


#endif //GX_CONSTRAINT_SOLVER
