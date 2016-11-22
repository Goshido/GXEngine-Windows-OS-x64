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
		GXAnimSolver*			animStream[ GX_ANIM_SOLVER_MOVEMENT_STATES ];

	public:
		GXAnimSolverMovement ( GXUShort solver );
		virtual ~GXAnimSolverMovement ();

		virtual GXVoid GetBone ( const GXChar* boneName, const GXQuat** rot, const GXVec3** loc );
		virtual GXVoid Update ( GXFloat delta );

		GXVoid SetAnimStream ( GXUChar state, GXAnimSolver* animStream );
		GXVoid SetState ( GXUChar state );
};


#endif	//GX_ANIM_SOLVER_MOVEMENT