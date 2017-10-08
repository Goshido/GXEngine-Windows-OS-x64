//version 1.1

#ifndef GX_SKELETON
#define GX_SKELETON


#include "GXBone.h"


class GXSkeleton
{
	private:
		GXBool		isValid;

		GXUShort	totalBones;
		GXBone*		bones;
		GXBone*		rootBone;

	public:
		GXSkeleton ();
		explicit GXSkeleton ( IGameObject &skinMesh );
		~GXSkeleton ();

		GXVoid From ( IGameObject &skinMesh );
		GXVoid Reset ();

		GXBool IsValid ();

		const GXUTF8* GetBoneName ( GXUShort boneIndex ) const;

		GXUShort GetBoneIndex ( IGameNode* bone ) const;
		GXUShort GetParentBoneIndex ( GXUShort boneIndex ) const;

		GXVoid CalculateReferenceTransform ();
		GXVoid CalculateInverseBindTransform ();
		GXVoid CalculatePoseTransform ( GXUInt frame );

		const GXQuatLocJoint& GetBoneReferenceTransform ( GXUShort boneIndex ) const;
		const GXMat4& GetBoneReferenceTransform2 ( GXUShort boneIndex ) const;
		const GXQuatLocJoint& GetBoneInverseBindTransform ( GXUShort boneIndex ) const;
		const GXMat4& GetBoneInverseBindTransform2 ( GXUShort boneIndex ) const;
		const GXQuatLocJoint& GetBonePoseTransform ( GXUShort boneIndex ) const;
		const GXMat4& GetBonePoseTransform2 ( GXUShort boneIndex ) const;

		GXUShort GetTotalBones () const;

	private:
		GXBool ExtractBones ( IGameObject &skinMesh );
		GXVoid LinkBones ();

		GXVoid RebuildBoneArray ();
		GXVoid RegroupBones ( GXBone* bone, GXUShort &currentBoneIndex, GXUShort parentBoneIndex );

		GXBool IsRootBone ( IGameNode* boneFather ) const;
		GXUShort FindBoneIndex ( IGameNode* bone ) const;
};


#endif //GX_SKELETON
