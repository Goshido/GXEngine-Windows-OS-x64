// version 1.7

#ifndef	GX_ANIMATION_SOLVER
#define	GX_ANIMATION_SOLVER


#include <GXCommon/GXMath.h>
#include <GXCommon/GXNativeSkeletalMesh.h>


class GXAnimationSolver
{
    protected:
        GXUShort    _solverID;

    public:
        explicit GXAnimationSolver ( GXUShort solverID );
        virtual ~GXAnimationSolver ();

        virtual GXBool GetBoneJoint ( GXBoneJoint &joint, const GXUTF8* boneName ) = 0;
        virtual GXVoid Update ( GXFloat delta ) = 0;

    private:
        GXAnimationSolver () = delete;
        GXAnimationSolver ( const GXAnimationSolver &other ) = delete;
        GXAnimationSolver& operator = ( const GXAnimationSolver &other ) = delete;
};


#endif // GX_ANIMATION_SOLVER
