// version 1.1

#ifndef GX_SINGLE_BODY_CONSTRAINT_SOLVER
#define GX_SINGLE_BODY_CONSTRAINT_SOLVER


#include "GXConstraint.h"
#include "GXRigidBody.h"
#include <GXCommon/GXMemory.h>


// Implementation is based on paper
// "Iterative Dynamics with Temporal Coherence"
// by Erin Catto

class GXSingleBodyConstraintSolver
{
	private:
		// s is number of constaints.

		GXDynamicArray		jacobian;			// s x 2 GXVec6's
		GXDynamicArray		bias;				// s GXFloat's
		GXDynamicArray		lambdaRange;		// s GXVec2's
		GXDynamicArray		initialLambda;		// s GXFloat's

		// working arrays
		GXDynamicArray		lambda;				// s GXFloat's
		GXDynamicArray		eta;				// s GXFloat's
		GXDynamicArray		d;					// s GXFloat's
		GXDynamicArray		b;					// s GXVec6's

		GXVec6				a;					// 1 GXVec6

		GXUShort			maximumIterations;
		GXUInt				constraints;
		GXRigidBody*		firstBody;
		GXRigidBody*		secondBody;

	public:
		explicit GXSingleBodyConstraintSolver ( GXUShort maximumIterations );
		~GXSingleBodyConstraintSolver ();

		GXVoid Begin ( GXRigidBody &first, GXRigidBody &second );
		GXVoid AddConstraint ( const GXConstraint &constraint );
		GXVoid End ();

	private:
		GXVoid UpdateB ();
		GXVoid UpdateEta ();
		GXVoid UpdateA ();
		GXVoid UpdateBodyVelocity ();

		const GXVec6& GetJacobianElement ( GXUInt constraint, GXUByte bodyIndex ) const;

		GXSingleBodyConstraintSolver () = delete;
		GXSingleBodyConstraintSolver ( const GXSingleBodyConstraintSolver &other ) = delete;
		GXSingleBodyConstraintSolver& operator = ( const GXSingleBodyConstraintSolver &other ) = delete;
};


#endif // GX_SINGLE_BODY_CONSTRAINT_SOLVER
