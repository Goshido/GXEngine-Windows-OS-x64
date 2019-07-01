//version 1.1

#ifndef GX_NATIVE_SKELETON
#define GX_NATIVE_SKELETON


#include "GXBoneJoint.h"


#pragma pack ( push )
#pragma pack ( 1 )

struct GXNativeSkeletonHeader
{
    GXUShort        _totalBones;
    GXUBigInt       _boneNamesOffset;               //array of totalBones elements: 64 byte slot with zero terminated UTF8 string.
    GXUBigInt       _parentBoneIndicesOffset;       //array of totalBones elements: zero based index (GXUShort).
    GXUBigInt       _referensePoseOffset;           //array of totalBones elements: GXBoneJoint. Relative parent.
    GXUBigInt       _inverseBindTransformOffset;    //array of totalBones elements: GXBoneJoint.
};

#pragma pack ( pop )

struct GXSkeletonInfo
{
    GXUShort        _totalBones;
    GXUTF8*         _boneNames;
    GXUShort*       _parentBoneIndices;
    GXBoneJoint*    _referencePose;
    GXBoneJoint*    _inverseBindTransform;

    GXSkeletonInfo ();
    GXVoid Cleanup ();
};

//------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeleton ( GXSkeletonInfo &info, const GXWChar* fileName );

#endif //GX_NATIVE_SKELETON
