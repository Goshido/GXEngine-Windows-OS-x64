//version 1.4

#ifndef	GX_ANIM_SOLVER_PLAYER
#define	GX_ANIM_SOLVER_PLAYER


#include "GXAnimSolver.h"
//#include "GXSkeletalMeshStorage.h"


class GXBoneFinder;
class GXAnimSolverPlayer : public GXAnimSolver
{
	private:
		GXBoneFinder*					finder;

		GXFloat							multiplier;
		GXFloat							animPos;
		const GXAnimationInfo*			animData;
		GXFloat							delta2PartFartor;

	public:
		GXAnimSolverPlayer ( GXUShort solverID );
		~GXAnimSolverPlayer () override;

		GXVoid GetBone ( const GXUTF8* boneName, const GXQuat** rot, const GXVec3** loc ) override;
		GXVoid Update ( GXFloat delta ) override;

		GXVoid SetAnimationSequence ( const GXAnimationInfo* animData );
		GXVoid SetAnimationMultiplier ( GXFloat multiplier );
};


#endif	//GX_ANIM_SOLVER_PLAYER
