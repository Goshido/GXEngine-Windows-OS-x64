//version 1.5

#ifndef GX_ANIMATION_SOLVER_PLAYER
#define GX_ANIMATION_SOLVER_PLAYER


#include "GXAnimationSolver.h"


class GXBoneFinder;
class GXAnimationSolverPlayer : public GXAnimationSolver
{
	private:
		GXBoneFinder*					finder;

		GXFloat							speed;
		GXFloat							animPos;
		const GXAnimationInfo*			animationInfo;
		GXFloat							deltaToPartFactor;
		GXFloat							frameInterval;
		GXFloat							frameInterpolationFactor;
		GXBool							isNormalize;

	public:
		explicit GXAnimationSolverPlayer ( GXUShort solverID );
		~GXAnimationSolverPlayer () override;

		GXBool GetBoneJoint ( GXBoneJoint &joint, const GXUTF8* boneName ) override;
		GXVoid Update ( GXFloat delta ) override;

		GXVoid SetAnimationSequence ( const GXAnimationInfo* animData );
		GXVoid SetAnimationMultiplier ( GXFloat multiplier );

		GXVoid EnableNormalization ();
		GXVoid DisableNormalization ();
};


#endif //GX_ANIMATION_SOLVER_PLAYER
