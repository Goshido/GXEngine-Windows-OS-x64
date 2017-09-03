//version 1.2

#ifndef	GX_ANIM_SOLVER_MOVEMENT
#define	GX_ANIM_SOLVER_MOVEMENT


#include "GXAnimSolver.h"


#define GX_ANIM_SOLVER_MOVEMENT_STATES	3
#define GX_ANIM_SOLVER_MOVEMENT_WALK	0
#define GX_ANIM_SOLVER_MOVEMENT_RUN		1
#define GX_ANIM_SOLVER_MOVEMENT_IDLE	2


class GXAnimSolverMovement : public GXAnimSolver
{
	private:
		GXUChar					state;
		GXAnimSolver*			animationStreams[ GX_ANIM_SOLVER_MOVEMENT_STATES ];

	public:
		GXAnimSolverMovement ( GXUShort solver );
		~GXAnimSolverMovement () override;

		GXBool GetBone ( const GXUTF8* boneName, GXQuat &rotation, GXVec3 &location ) override;
		GXVoid Update ( GXFloat delta ) override;

		GXVoid SetAnimStream ( GXUChar animationState, GXAnimSolver* animStream );
		GXVoid SetState ( GXUChar animationState );
};


#endif	//GX_ANIM_SOLVER_MOVEMENT
