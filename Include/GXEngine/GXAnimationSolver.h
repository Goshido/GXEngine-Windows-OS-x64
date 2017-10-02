//version 1.4

#ifndef	GX_ANIMATION_SOLVER
#define	GX_ANIMATION_SOLVER


#include <GXCommon/GXMath.h>
#include <GXCommon/GXNativeSkeletalMesh.h>


class GXAnimationSolver
{
	protected:
		GXUShort			solverID;

	public:
		explicit GXAnimationSolver ( GXUShort solverID );
		virtual ~GXAnimationSolver ();

		virtual GXBool GetBone ( GXQuat &rotation, GXVec3 &location, const GXUTF8* boneName ) = 0;
		virtual GXVoid Update ( GXFloat delta ) = 0;
};


#endif //GX_ANIM_SOLVER
