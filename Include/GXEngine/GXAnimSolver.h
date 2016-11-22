//version 1.3

#ifndef	GX_ANIM_SOLVER
#define	GX_ANIM_SOLVER


#include <GXCommon/GXMath.h>
#include <GXCommon/GXNativeSkeletalMesh.h>

/*
struct GXQuatLocPoseLocal
{
	GXChar				boneName[ MAX_BONES ][ 64 ];
	GXQuatLocJoint		local[ MAX_BONES ];
};
*/
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

		virtual GXVoid GetBone ( const GXUTF8* boneName, const GXQuat** rot, const GXVec3** loc ) = 0;
		virtual GXVoid Update ( GXFloat delta ) = 0;
};


#endif	//GX_ANIM_SOLVER