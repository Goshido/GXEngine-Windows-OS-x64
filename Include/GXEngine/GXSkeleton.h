//version 1.2

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

		GXBoneJoint*		tempPoseLocal;
		GXBoneJoint*		tempPoseGlobal;

		GXBoneJoint*		referencePose;
		GXBoneJoint*		inverseBindTransform;
		GXBoneJoint*		skinTransform;

	public:
		GXSkeleton ();
		~GXSkeleton ();

		GXVoid LoadFromSkm ( const GXWChar* fileName );
		GXVoid UpdatePose ( GXAnimationSolver &solver, GXFloat deltaTime );
		const GXBoneJoint* GetSkinTransform () const;

		const GXBoneJoint& GetBoneTransformWorld ( GXUShort boneIndex ) const;
		const GXBoneJoint& GetParentBoneTransformWorld ( GXUShort boneIndex ) const;

		GXUShort GetTotalBones () const;

	private:
		GXVoid CalculatePose ();
};


#endif //GX_SKELETON
