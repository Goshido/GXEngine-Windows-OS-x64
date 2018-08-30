// version 1.9

#ifndef GX_ANIMATION_SOLVER_PLAYER
#define GX_ANIMATION_SOLVER_PLAYER


#include "GXAnimationSolver.h"


class GXBoneFinder;
class GXAnimationSolverPlayer final : public GXAnimationSolver
{
	private:
		GXBoneFinder*					finder;

		GXFloat							animPos;
		const GXAnimationInfo*			animationInfo;
		GXFloat							frameInterval;
		GXFloat							frameInterpolationFactor;

		GXFloat							speed;
		GXFloat							deltaToPartFactor;
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

	private:
		GXAnimationSolverPlayer ( const GXAnimationSolverPlayer &other ) = delete;
		GXAnimationSolverPlayer& operator = ( const GXAnimationSolverPlayer &other ) = delete;
};


#endif // GX_ANIMATION_SOLVER_PLAYER
