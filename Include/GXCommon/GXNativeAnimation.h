//version 1.0

#ifndef GX_NATIVE_ANIMATION
#define GX_NATIVE_ANIMATION


#include "GXBoneJoint.h"


#pragma pack ( push )
#pragma pack ( 1 )

struct GXNativeAnimationHeader
{
	GXFloat		fps;
	GXUInt		totalFrames;

	GXUShort	totalBones;
	GXUBigInt	boneNamesOffset;		//array of totalBones elements: 64 byte slot with zero terminated UTF-8 string
	GXUBigInt	keysOffset;				//array of [totalBones * totalFrames] elements: GXBoneJoint. Relative parent.
};

#pragma pack ( pop )

struct GXAnimationInfo
{
	GXUShort				totalBones;
	GXUTF8*					boneNames;

	GXFloat					fps;
	GXUInt					totalFrames;
	GXBoneJoint*			keys;

	GXAnimationInfo ();
	GXVoid Cleanup ();
};


GXVoid GXCALL GXLoadNativeAnimation ( GXAnimationInfo &info, const GXWChar* fileName );


#endif //GX_NATIVE_ANIMATION
