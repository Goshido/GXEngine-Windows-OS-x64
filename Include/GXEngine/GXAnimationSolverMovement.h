// version 1.6

#ifndef	GX_ANIMATION_SOLVER_MOVEMENT
#define	GX_ANIMATION_SOLVER_MOVEMENT


#include "GXAnimationSolver.h"


enum class eGXAnimationSolverMovementState : GXUShort
{
    Walk = 0,
    Run = 1,
    Idle = 2
};

class GXAnimationSolverMovement final : public GXAnimationSolver
{
    private:
        eGXAnimationSolverMovementState     _state;
        GXAnimationSolver*                  _animationStreams[ 3u ];

    public:
        explicit GXAnimationSolverMovement ( GXUShort solverID );
        ~GXAnimationSolverMovement () override;

        GXBool GetBoneJoint ( GXBoneJoint &joint, const GXUTF8* boneName ) override;
        GXVoid Update ( GXFloat delta ) override;

        GXVoid SetAnimationStream ( eGXAnimationSolverMovementState animationState, GXAnimationSolver* animationStream );
        GXVoid SetState ( eGXAnimationSolverMovementState newState );

    private:
        GXAnimationSolverMovement ( const GXAnimationSolverMovement &other ) = delete;
        GXAnimationSolverMovement& operator = ( const GXAnimationSolverMovement &other ) = delete;
};


#endif // GX_ANIMATION_SOLVER_MOVEMENT
