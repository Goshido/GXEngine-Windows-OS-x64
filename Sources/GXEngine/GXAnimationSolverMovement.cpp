// version 1.6

#include <GXEngine/GXAnimationSolverMovement.h>


GXAnimationSolverMovement::GXAnimationSolverMovement ( GXUShort solverID ):
    GXAnimationSolver ( solverID ),
    _state ( eGXAnimationSolverMovementState::Idle )
{
    // NOTHING
}

GXAnimationSolverMovement::~GXAnimationSolverMovement ()
{
    // NOTHING
}

GXBool GXAnimationSolverMovement::GetBoneJoint ( GXBoneJoint &joint, const GXUTF8* boneName )
{
    return _animationStreams[ static_cast<GXUShort> ( _state ) ]->GetBoneJoint ( joint, boneName );
}

GXVoid GXAnimationSolverMovement::Update ( GXFloat delta )
{
    _animationStreams[ static_cast<GXUShort> ( _state ) ]->Update ( delta );
}

GXVoid GXAnimationSolverMovement::SetAnimationStream ( eGXAnimationSolverMovementState animationState, GXAnimationSolver* animationStream )
{
    _animationStreams[ static_cast<GXUShort> ( animationState ) ] = animationStream;
}

GXVoid GXAnimationSolverMovement::SetState ( eGXAnimationSolverMovementState newState )
{
    _state = newState;
}
