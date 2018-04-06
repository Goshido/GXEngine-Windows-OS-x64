//version 1.0

#ifndef GX_CONSTRAINT_SOLVER
#define GX_CONSTRAINT_SOLVER


#include "GXConstraint.h"
#include "GXRigidBody.h"
#include <GXCommon/GXMemory.h>


//Implementation is based on paper
//"Iterative Dynamics with Temporal Coherence"
//by Erin Catto

class GXConstraintSolver
{
	private:
		//s is number of constaints.

		GXDynamicArray				jacobian;			// s x 2 GXSparseMatrixElement's
		GXDynamicArray				mapper;				// s x 2 GXUByte's
		GXDynamicArray				bias;				// s GXFloat's
		GXDynamicArray				lambdaRange;		// s GXVec2's
		GXDynamicArray				initialLambda;		// s GXFloat's

		// working arrays
		GXDynamicArray				lambda;				// s GXFloat's
		GXDynamicArray				eta;				// s GXFloat's
		GXDynamicArray				d;					// s GXFloat's
		GXDynamicArray				b;					// s x 2 GXSparseMatrixElement's

		GXSparseMatrixElement		a[ 2 ];				// 2 x 1 GXSparseMatrixElement's
		GXSparseMatrixElement		inverseM[ 24 ];		// 12 x 2 GXSparseMatrixElement's

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

	private:
		GXVoid UpdateMapper ();
		GXVoid UpdateInverseM ();
		GXVoid UpdateB ();
		GXVoid UpdateEta ();
};


#endif //GX_CONSTRAINT_SOLVER
