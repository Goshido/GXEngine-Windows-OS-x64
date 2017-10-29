//version 1.0

#include <GXCommon/GXNativeSkeleton.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


GXSkeletonInfo::GXSkeletonInfo ()
{
	totalBones = 0;
	boneNames = nullptr;
	parentBoneIndices = nullptr;
	referencePose = inverseBindTransform = nullptr;
}

GXVoid GXSkeletonInfo::Cleanup ()
{
	GXSafeFree ( boneNames );
	GXSafeFree ( parentBoneIndices );
	GXSafeFree ( referencePose );
	GXSafeFree ( inverseBindTransform );
}

//------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeleton ( GXSkeletonInfo &info, const GXWChar* fileName )
{
	GXUByte* data;
	GXUBigInt fileSize;

	if ( !GXLoadFile ( fileName, (GXVoid**)&data, fileSize, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadNativeSkeleton::Error - Can't load file %s\n", fileName );
		return;
	}

	GXNativeSkeletonHeader* h = (GXNativeSkeletonHeader*)data;

	info.totalBones = h->totalBones;

	GXUPointer size = info.totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	info.boneNames = (GXUTF8*)malloc ( size );
	memcpy ( info.boneNames, data + h->boneNamesOffset, size );

	size = info.totalBones * sizeof ( GXShort );
	info.parentBoneIndices = (GXUShort*)malloc ( size );
	memcpy ( info.parentBoneIndices, data + h->parentBoneIndicesOffset, size );

	size = info.totalBones * sizeof ( GXBoneJoint );
	info.referencePose = (GXBoneJoint*)malloc ( size );
	memcpy ( info.referencePose, data + h->referensePoseOffset, size );

	info.inverseBindTransform = (GXBoneJoint*)malloc ( size );
	memcpy ( info.inverseBindTransform, data + h->inverseBindTransformOffset, size );

	free ( data );
}
