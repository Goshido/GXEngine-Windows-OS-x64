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

GXVoid GXAnimSolverMovement::GetBone ( const GXChar* boneName, const GXQuat** rot, const GXVec3** loc )
{
	animStream[ state ]->GetBone ( boneName, rot, loc );
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