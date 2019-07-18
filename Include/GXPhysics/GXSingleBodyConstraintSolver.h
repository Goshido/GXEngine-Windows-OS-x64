// version 1.3

#ifndef GX_SINGLE_BODY_CONSTRAINT_SOLVER
#define GX_SINGLE_BODY_CONSTRAINT_SOLVER


#include "GXConstraint.h"
#include "GXRigidBody.h"
#include <GXCommon/GXMemory.h>


// Implementation is based on paper
// "Iterative Dynamics with Temporal Coherence"
// by Erin Catto

class GXSingleBodyConstraintSolver final
{
    private:
        // s is number of constaints.

        GXDynamicArray      _jacobian;              // s x 2 GXVec6's
        GXDynamicArray      _bias;                  // s GXFloat's
        GXDynamicArray      _lambdaRange;           // s GXVec2's
        GXDynamicArray      _initialLambda;         // s GXFloat's

        // working arrays
        GXDynamicArray      _lambda;                // s GXFloat's
        GXDynamicArray      _eta;                   // s GXFloat's
        GXDynamicArray      _d;                     // s GXFloat's
        GXDynamicArray      _b;                     // s GXVec6's

        GXVec6              _a;                     // 1 GXVec6

        GXUShort            _maximumIterations;
        GXUInt              _constraints;
        GXRigidBody*        _firstBody;
        GXRigidBody*        _secondBody;

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
