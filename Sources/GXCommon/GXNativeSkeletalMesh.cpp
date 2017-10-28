//version 1.2

#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


#define UNKNOWN_BONE_INDEX	0xFFFE


GXSkeletalMeshData::GXSkeletalMeshData ()
{
	totalVertices = 0;
	vboData = nullptr;

	totalBones = 0;
	boneNames = nullptr;
	parentBoneIndices = nullptr;
	referencePose = inverseBindTransform = nullptr;
}

GXVoid GXSkeletalMeshData::Cleanup ()
{
	GXSafeFree ( vboData );
	GXSafeFree ( boneNames );
	GXSafeFree ( parentBoneIndices );
	GXSafeFree ( referencePose );
	GXSafeFree ( inverseBindTransform );
}

//-------------------------------------------------------------------------------------------------------------

GXAnimationInfo::GXAnimationInfo ()
{
	totalBones = 0;
	boneNames = nullptr;

	fps = 60.0f;
	totalFrames = 0;
	keys = nullptr;
}

GXVoid GXAnimationInfo::Cleanup ()
{
	totalBones = 0;
	GXSafeFree ( boneNames );

	fps = 60.0f;
	totalFrames = 0;
	GXSafeFree ( keys );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeletalMesh ( GXSkeletalMeshData &info, const GXWChar* fileName )
{
	GXUByte* data;
	GXUBigInt fileSize;

	if ( !GXLoadFile ( fileName, (GXVoid**)&data, fileSize, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadNativeSkeletalMesh::Error - Can't load file %s\n", fileName );
		return;
	}

	GXUPointer size = 0;

	GXNativeSkeletalMeshHeader* h = (GXNativeSkeletalMeshHeader*)data;

	info.totalVertices = h->totalVertices;

	size = info.totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
	info.vboData = (GXFloat*)malloc ( size );
	memcpy ( info.vboData, data + h->vboOffset, size );

	info.totalBones = h->totalBones;

	size = info.totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
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

GXVoid GXCALL GXLoadNativeAnimation ( GXAnimationInfo &info, const GXWChar* fileName )
{
	GXUByte* data;
	GXUPointer size;

	if ( !GXLoadFile ( fileName, (GXVoid**)&data, size, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadNativeAnimation::Error - Can't load file %s\n", fileName );
		return;
	}

	GXNativeAnimationHeader* h = (GXNativeAnimationHeader*)data;

	info.fps = h->fps;
	info.totalBones = h->totalBones;
	info.totalFrames = h->totalFrames;
	
	size = h->totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	info.boneNames = (GXUTF8*)malloc ( size );
	memcpy ( info.boneNames, data + h->boneNamesOffset, size );

	size = h->totalBones * h->totalFrames * sizeof ( GXBoneJoint );
	info.keys = (GXBoneJoint*)malloc ( size );
	memcpy ( info.keys, data + h->keysOffset, size );

	free ( data );
}
