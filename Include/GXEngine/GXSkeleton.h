//version 1.1

#ifndef GX_SKELETON
#define GX_SKELETON


#include <GXEngine/GXAnimationSolver.h>
#include <GXCommon/GXNativeSkeletalMesh.h>


class GXSkeleton
{
	private:
		GXUShort			totalBones;
		GXUTF8*				boneNames;
		GXShort*			parentBoneIndices;

		GXQuatLocJoint*		tempPoseLocal;
		GXMat4*				tempPoseLocal2;
		GXQuatLocJoint*		tempPoseGlobal;
		GXMat4*				tempPoseGlobal2;

		GXQuatLocJoint*		referencePose;
		GXMat4*				referencePose2;
		GXQuatLocJoint*		inverseBindTransform;
		GXMat4*				inverseBindTransform2;
		GXQuatLocJoint*		pose;
		GXMat4*				pose2;

	public:
		GXSkeleton ();
		~GXSkeleton ();

		GXVoid LoadFromSkm ( const GXWChar* fileName );
		GXVoid UpdatePose ( GXAnimationSolver &solver, GXFloat deltaTime );
		const GXQuatLocJoint* GetPose () const;
		const GXMat4* GetPose2 () const;

		const GXMat4& GetBoneTransformWorld ( GXUShort boneIndex ) const;
		const GXMat4& GetParentBoneTransformWorld ( GXUShort boneIndex ) const;

		GXUShort GetTotalBones () const;

	private:
		GXVoid CalculatePose ();
};


#endif //GX_SKELETON
