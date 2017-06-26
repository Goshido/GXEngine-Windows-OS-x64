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
	return animStream[ state ]->GetBone ( boneName, rotation, location );
}

GXVoid GXAnimSolverMovement::Update ( GXFloat delta )
{
	animStream[ state ]->Update ( delta );
}

GXVoid GXAnimSolverMovement::SetAnimStream ( GXUChar state, GXAnimSolver* animStream )
{
	this->animStream[ state ] = animStream;
}

GXVoid GXAnimSolverMovement::SetState ( GXUChar state )
{
	this->state = state;
}