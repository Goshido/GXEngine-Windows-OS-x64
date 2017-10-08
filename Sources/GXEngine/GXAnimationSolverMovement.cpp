//version 1.3

#include <GXEngine/GXAnimationSolverMovement.h>


GXAnimationSolverMovement::GXAnimationSolverMovement ( GXUShort solverID ):
GXAnimationSolver ( solverID )
{
	state = eGXAnimationSolverMovementState::Idle;
}

GXAnimationSolverMovement::~GXAnimationSolverMovement ()
{
	//NOTHING
}

GXBool GXAnimationSolverMovement::GetBone ( GXQuat &rotation, GXVec3 &location, const GXUTF8* boneName )
{
	return animationStreams[ (GXUShort)state ]->GetBone ( rotation, location, boneName );
}

GXBool GXAnimationSolverMovement::GetBone2 ( GXMat4 &transform, const GXUTF8* boneName )
{
	return animationStreams[ (GXUShort)state ]->GetBone2 ( transform, boneName );
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