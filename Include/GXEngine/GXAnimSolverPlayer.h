//version 1.4

#ifndef	GX_ANIM_SOLVER_PLAYER
#define	GX_ANIM_SOLVER_PLAYER


#include "GXAnimSolver.h"
#include "GXSkeletalMeshStorage.h"


class GXBoneFinder;
class GXAnimSolverPlayer : public GXAnimSolver
{
	private:
		GXBoneFinder*					finder;

		GXFloat							multiplier;
		GXFloat							animPos;
		const GXAnimationInfoExt*		animData;
		GXFloat							delta2PartFartor;

	public:
		GXAnimSolverPlayer ( GXUShort solver );
		virtual ~GXAnimSolverPlayer ();

		virtual GXVoid GetBone ( const GXUTF8* boneName, const GXQuat** rot, const GXVec3** loc );
		virtual GXVoid Update ( GXFloat delta );

		GXVoid SetAnimationSequence ( const GXAnimationInfoExt* animData );
		GXVoid SetAnimationMultiplier ( GXFloat multiplier );
};


#endif	//GX_ANIM_SOLVER_PLAYER
