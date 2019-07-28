// version 1.12

#ifndef GX_ANIMATION_SOLVER_PLAYER
#define GX_ANIMATION_SOLVER_PLAYER


#include "GXAnimationSolver.h"


class GXBoneFinder;
class GXAnimationSolverPlayer final : public GXAnimationSolver
{
    private:
        GXBoneFinder*               _finder;

        GXFloat                     _animPos;
        const GXAnimationInfo*      _animationInfo;
        GXFloat                     _frameInterval;
        GXFloat                     _frameInterpolationFactor;

        GXFloat                     _speed;
        GXFloat                     _deltaToPartFactor;
        GXBool                      _isNormalize;

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
        GXAnimationSolverPlayer () = delete;
        GXAnimationSolverPlayer ( const GXAnimationSolverPlayer &other ) = delete;
        GXAnimationSolverPlayer& operator = ( const GXAnimationSolverPlayer &other ) = delete;
};


#endif // GX_ANIMATION_SOLVER_PLAYER
