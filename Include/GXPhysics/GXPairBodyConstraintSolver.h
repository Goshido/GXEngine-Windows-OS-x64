// version 1.2

#ifndef GX_PAIR_BODY_CONSTRAINT_SOLVER
#define GX_PAIR_BODY_CONSTRAINT_SOLVER


#include "GXConstraint.h"
#include "GXRigidBody.h"
#include <GXCommon/GXMemory.h>


// Implementation is based on paper
// "Iterative Dynamics with Temporal Coherence"
// by Erin Catto

class GXPairBodyConstraintSolver final
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
		GXDynamicArray		b;					// 2 x s GXVec6's

		GXVec6				a[ 2 ];				// 2 x 1 GXVec6's

		GXUShort			maximumIterations;
		GXUInt				constraints;
		GXRigidBody*		firstBody;
		GXRigidBody*		secondBody;

	public:
		explicit GXPairBodyConstraintSolver ( GXUShort maximumIterations );
		~GXPairBodyConstraintSolver ();

		GXVoid Begin ( GXRigidBody &first, GXRigidBody &second );
		GXVoid AddConstraint ( const GXConstraint &constraint );
		GXVoid End ();

	private:
		GXVoid UpdateB ();
		GXVoid UpdateEta ();
		GXVoid UpdateA ();
		GXVoid UpdateBodyVelocities ();

		const GXVec6& GetJacobianElement ( GXUInt constraint, GXUByte bodyIndex ) const;
		const GXVec6& GetBElement ( GXUByte bodyIndex, GXUInt constraint ) const;

		GXPairBodyConstraintSolver () = delete;
		GXPairBodyConstraintSolver ( const GXPairBodyConstraintSolver &other ) = delete;
		GXPairBodyConstraintSolver& operator = ( const GXPairBodyConstraintSolver &other ) = delete;
};


#endif // GX_PAIR_BODY_CONSTRAINT_SOLVER
