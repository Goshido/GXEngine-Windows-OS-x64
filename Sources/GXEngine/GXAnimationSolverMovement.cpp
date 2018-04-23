// version 1.3

#include <GXEngine/GXAnimationSolverMovement.h>


GXAnimationSolverMovement::GXAnimationSolverMovement ( GXUShort solverID ):
GXAnimationSolver ( solverID )
{
	state = eGXAnimationSolverMovementState::Idle;
}

GXAnimationSolverMovement::~GXAnimationSolverMovement ()
{
	// NOTHING
}

GXBool GXAnimationSolverMovement::GetBoneJoint ( GXBoneJoint &joint, const GXUTF8* boneName )
{
	return animationStreams[ (GXUShort)state ]->GetBoneJoint ( joint, boneName );
}

GXVoid GXAnimationSolverMovement::Update ( GXFloat delta )
{
	animationStreams[ (GXUShort)state ]->Update ( delta );
}

GXVoid GXAnimationSolverMovement::SetAnimationStream ( eGXAnimationSolverMovementState animationState, GXAnimationSolver* animationStream )
{
	animationStreams[ (GXUShort)animationState ] = animationStream;
}

GXVoid GXAnimationSolverMovement::SetState ( eGXAnimationSolverMovementState newState )
{
	state = newState;
}