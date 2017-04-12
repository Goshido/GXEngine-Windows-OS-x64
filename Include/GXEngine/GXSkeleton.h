//version 1.0

#ifndef GX_SKELETON
#define GX_SKELETON


#include <GXEngine/GXAnimSolver.h>
#include <GXCommon/GXNativeSkeletalMesh.h>


class GXSkeleton
{
	private:
		GXUShort			numBones;
		GXUTF8*				boneNames;
		GXShort*			parentBoneIndices;
		GXQuatLocJoint*		pose;
		GXQuatLocJoint*		tempPoseLocal;
		GXQuatLocJoint*		tempPoseGlobal;
		GXQuatLocJoint*		referencePose;
		GXQuatLocJoint*		bindTransform;

	public:
		GXSkeleton ();
		~GXSkeleton ();

		GXVoid LoadFromSkm ( const GXWChar* fileName );
		GXVoid UpdatePose ( GXAnimSolver &solver );
		const GXQuatLocJoint* GetPose () const;
		GXUShort GetTotalBones () const;

	private:
		GXVoid CalculatePose ();
};


#endif //GX_SKELETON
