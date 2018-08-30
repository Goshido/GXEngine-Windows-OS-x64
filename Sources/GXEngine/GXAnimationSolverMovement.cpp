// version 1.5

#include <GXEngine/GXAnimationSolverMovement.h>


GXAnimationSolverMovement::GXAnimationSolverMovement ( GXUShort solverID ):
	GXAnimationSolver ( solverID ),
	state ( eGXAnimationSolverMovementState::Idle )
{
	// NOTHING
}

GXAnimationSolverMovement::~GXAnimationSolverMovement ()
{
	// NOTHING
}

GXBool GXAnimationSolverMovement::GetBoneJoint ( GXBoneJoint &joint, const GXUTF8* boneName )
{
	return animationStreams[ static_cast<GXUShort> ( state ) ]->GetBoneJoint ( joint, boneName );
}

GXVoid GXAnimationSolverMovement::Update ( GXFloat delta )
{
	animationStreams[ static_cast<GXUShort> ( state ) ]->Update ( delta );
}

GXVoid GXAnimationSolverMovement::SetAnimationStream ( eGXAnimationSolverMovementState animationState, GXAnimationSolver* animationStream )
{
	animationStreams[ static_cast<GXUShort> ( animationState ) ] = animationStream;
}

GXVoid GXAnimationSolverMovement::SetState ( eGXAnimationSolverMovementState newState )
{
	state = newState;
}
