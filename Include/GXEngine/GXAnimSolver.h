//version 1.3

#ifndef	GX_ANIM_SOLVER
#define	GX_ANIM_SOLVER


#include <GXCommon/GXMath.h>
#include <GXCommon/GXNativeSkeletalMesh.h>


class GXAnimSolver
{
	protected:
		GXUShort			solverID;

	public:
		GXAnimSolver ( GXUShort solverID )
		{
			this->solverID = solverID;
		}

		virtual ~GXAnimSolver () 
		{ 
			/*PURE VIRTUAL*/ 
		}

		virtual GXBool GetBone ( const GXUTF8* boneName, GXQuat &rotation, GXVec3 &location ) = 0;
		virtual GXVoid Update ( GXFloat delta ) = 0;
};


#endif //GX_ANIM_SOLVER
