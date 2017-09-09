//version 1.3

#ifndef	GX_ANIMATION_SOLVER_MOVEMENT
#define	GX_ANIMATION_SOLVER_MOVEMENT


#include "GXAnimationSolver.h"


enum class eGXAnimationSolverMovementState : GXUShort
{
	Walk = 0,
	Run = 1,
	Idle = 2
};

class GXAnimationSolverMovement : public GXAnimationSolver
{
	private:
		eGXAnimationSolverMovementState		state;
		GXAnimationSolver*					animationStreams[ 3 ];

	public:
		GXAnimationSolverMovement ( GXUShort solverID );
		~GXAnimationSolverMovement () override;

		GXBool GetBone ( const GXUTF8* boneName, GXQuat &rotation, GXVec3 &location ) override;
		GXVoid Update ( GXFloat delta ) override;

		GXVoid SetAnimationStream ( eGXAnimationSolverMovementState animationState, GXAnimationSolver* animationStream );
		GXVoid SetState ( eGXAnimationSolverMovementState newState );
};


#endif	//GX_ANIMATION_SOLVER_MOVEMENT
