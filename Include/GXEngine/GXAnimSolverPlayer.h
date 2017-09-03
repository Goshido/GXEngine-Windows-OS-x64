//version 1.4

#ifndef GX_ANIM_SOLVER_PLAYER
#define GX_ANIM_SOLVER_PLAYER


#include "GXAnimSolver.h"


class GXBoneFinder;
class GXAnimSolverPlayer : public GXAnimSolver
{
	private:
		GXBoneFinder*					finder;

		GXFloat							speed;
		GXFloat							animPos;
		const GXAnimationInfo*			animationInfo;
		GXFloat							delta2PartFactor;
		GXFloat							frameInterval;
		GXFloat							frameInterpolationFactor;
		GXBool							isNormalize;

	public:
		GXAnimSolverPlayer ( GXUShort solverID );
		~GXAnimSolverPlayer () override;

		GXBool GetBone ( const GXUTF8* boneName, GXQuat &rotation, GXVec3 &location ) override;
		GXVoid Update ( GXFloat delta ) override;

		GXVoid SetAnimationSequence ( const GXAnimationInfo* animData );
		GXVoid SetAnimationMultiplier ( GXFloat multiplier );

		GXVoid EnableNormalization ();
		GXVoid DisableNormalization ();
};


#endif //GX_ANIM_SOLVER_PLAYER
