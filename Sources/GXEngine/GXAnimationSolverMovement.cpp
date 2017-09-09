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

GXBool GXAnimationSolverMovement::GetBone ( const GXUTF8* boneName, GXQuat &rotation, GXVec3 &location )
{
	return animationStreams[ (GXUShort)state ]->GetBone ( boneName, rotation, location );
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