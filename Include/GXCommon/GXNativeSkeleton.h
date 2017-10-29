//version 1.0

#ifndef GX_NATIVE_SKELETON
#define GX_NATIVE_SKELETON


#include "GXBoneJoint.h"


#pragma pack ( push )
#pragma pack ( 1 )

struct GXNativeSkeletonHeader
{
	GXUShort	totalBones;
	GXUBigInt	boneNamesOffset;				//array of totalBones elements: 64 byte slot with zero terminated UTF8 string.
	GXUBigInt	parentBoneIndicesOffset;		//array of totalBones elements: zero based index (GXUShort).
	GXUBigInt	referensePoseOffset;			//array of totalBones elements: GXBoneJoint. Relative parent.
	GXUBigInt	inverseBindTransformOffset;		//array of totalBones elements: GXBoneJoint.
};

#pragma pack ( pop )

struct GXSkeletonInfo
{
	GXUShort		totalBones;
	GXUTF8*			boneNames;
	GXUShort*		parentBoneIndices;
	GXBoneJoint*	referencePose;
	GXBoneJoint*	inverseBindTransform;

	GXSkeletonInfo ();
	GXVoid Cleanup ();
};

//------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeleton ( GXSkeletonInfo &info, const GXWChar* fileName );

#endif //GX_NATIVE_SKELETON
