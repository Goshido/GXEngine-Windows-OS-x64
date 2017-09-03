//version 1.2

#include <GXEngine/GXAnimSolverMovement.h>


GXAnimSolverMovement::GXAnimSolverMovement ( GXUShort solverID ):
GXAnimSolver ( solverID )
{
	state = GX_ANIM_SOLVER_MOVEMENT_IDLE;
}

GXAnimSolverMovement::~GXAnimSolverMovement ()
{
	//NOTHING
}

GXBool GXAnimSolverMovement::GetBone ( const GXUTF8* boneName, GXQuat &rotation, GXVec3 &location )
{
	return animationStreams[ state ]->GetBone ( boneName, rotation, location );
}

GXVoid GXAnimSolverMovement::Update ( GXFloat delta )
{
	animationStreams[ state ]->Update ( delta );
}

GXVoid GXAnimSolverMovement::SetAnimStream ( GXUChar animationState, GXAnimSolver* animStream )
{
	animationStreams[ animationState ] = animStream;
}

GXVoid GXAnimSolverMovement::SetState ( GXUChar animationState )
{
	state = animationState;
}