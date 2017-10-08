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
	referencePose2 = inverseBindTransform2 = nullptr;
}

GXVoid GXSkeletalMeshData::Cleanup ()
{
	GXSafeFree ( vboData );
	GXSafeFree ( boneNames );
	GXSafeFree ( parentBoneIndices );
	GXSafeFree ( referencePose );
	GXSafeFree ( referencePose2 );
	GXSafeFree ( inverseBindTransform );
	GXSafeFree ( inverseBindTransform2 );
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

	size = info.totalBones * sizeof ( GXQuatLocJoint );
	info.referencePose = (GXQuatLocJoint*)malloc ( size );
	memcpy ( info.referencePose, data + h->referensePoseOffset, size );

	size = info.totalBones * sizeof ( GXMat4 );
	info.referencePose2 = (GXMat4*)malloc ( size );
	memcpy ( info.referencePose2, data + h->referensePoseOffset2, size );

	size = info.totalBones * sizeof ( GXQuatLocJoint );
	info.inverseBindTransform = (GXQuatLocJoint*)malloc ( size );
	memcpy ( info.inverseBindTransform, data + h->inverseBindTransformOffset, size );

	size = info.totalBones * sizeof ( GXMat4 );
	info.inverseBindTransform2 = (GXMat4*)malloc ( size );
	memcpy ( info.inverseBindTransform2, data + h->inverseBindTransformOffset2, size );

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

	size = h->totalBones * h->totalFrames * sizeof ( GXQuatLocJoint );
	info.keys = (GXQuatLocJoint*)malloc ( size );
	memcpy ( info.keys, data + h->keysOffset, size );

	size = h->totalBones * h->totalFrames * sizeof ( GXMat4 );
	info.keys2 = (GXMat4*)malloc ( size );
	memcpy ( info.keys2, data + h->keysOffset2, size );

	free ( data );
}
