// version 1.4

#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


#define UNKNOWN_BONE_INDEX	0xFFFE
#define DEFAULT_FPS			60.0f


GXSkeletalMeshData::GXSkeletalMeshData ():
	totalVertices ( 0u ),
	vboData ( nullptr ),
	totalBones ( 0u ),
	boneNames ( nullptr ),
	parentBoneIndices ( nullptr ),
	referencePose ( nullptr ),
	inverseBindTransform ( nullptr )
{
	// NOTHING
}

GXVoid GXSkeletalMeshData::Cleanup ()
{
	totalVertices = 0u;
	GXSafeFree ( vboData );

	totalBones = 0u;
	GXSafeFree ( boneNames );
	GXSafeFree ( parentBoneIndices );
	GXSafeFree ( referencePose );
	GXSafeFree ( inverseBindTransform );
}

//-------------------------------------------------------------------------------------------------------------

GXAnimationInfo::GXAnimationInfo ():
	totalBones ( 0u ),
	boneNames ( nullptr ),
	fps ( DEFAULT_FPS ),
	totalFrames ( 0u ),
	keys ( nullptr )
{
	// NOTHING
}

GXVoid GXAnimationInfo::Cleanup ()
{
	totalBones = 0u;
	GXSafeFree ( boneNames );

	fps = DEFAULT_FPS;
	totalFrames = 0u;
	GXSafeFree ( keys );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeletalMesh ( GXSkeletalMeshData &info, const GXWChar* fileName )
{
	GXUByte* data;
	GXUBigInt fileSize;

	if ( !GXLoadFile ( fileName, reinterpret_cast<GXVoid**> ( &data ), fileSize, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadNativeSkeletalMesh::Error - Can't load file %s\n", fileName );
		return;
	}

	GXUPointer size = 0u;
	const GXNativeSkeletalMeshHeader* h = reinterpret_cast<const GXNativeSkeletalMeshHeader*> ( data );

	info.totalVertices = h->totalVertices;

	size = info.totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
	info.vboData = static_cast<GXFloat*> ( malloc ( size ) );
	memcpy ( info.vboData, data + h->vboOffset, size );

	info.totalBones = h->totalBones;

	size = info.totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	info.boneNames = static_cast<GXUTF8*> ( malloc ( size ) );
	memcpy ( info.boneNames, data + h->boneNamesOffset, size );

	size = info.totalBones * sizeof ( GXShort );
	info.parentBoneIndices = static_cast<GXUShort*> ( malloc ( size ) );
	memcpy ( info.parentBoneIndices, data + h->parentBoneIndicesOffset, size );

	size = info.totalBones * sizeof ( GXBoneJoint );
	info.referencePose = static_cast<GXBoneJoint*> ( malloc ( size ) );
	memcpy ( info.referencePose, data + h->referensePoseOffset, size );

	info.inverseBindTransform = static_cast<GXBoneJoint*> ( malloc ( size ) );
	memcpy ( info.inverseBindTransform, data + h->inverseBindTransformOffset, size );

	free ( data );
}

GXVoid GXCALL GXLoadNativeAnimation ( GXAnimationInfo &info, const GXWChar* fileName )
{
	GXUByte* data;
	GXUPointer size;

	if ( !GXLoadFile ( fileName, reinterpret_cast<GXVoid**> ( &data ), size, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadNativeAnimation::Error - Can't load file %s\n", fileName );
		return;
	}

	const GXNativeAnimationHeader* h = reinterpret_cast<const GXNativeAnimationHeader*> ( data );

	info.fps = h->fps;
	info.totalBones = h->totalBones;
	info.totalFrames = h->totalFrames;
	
	size = h->totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	info.boneNames = static_cast<GXUTF8*> ( malloc ( size ) );
	memcpy ( info.boneNames, data + h->boneNamesOffset, size );

	size = h->totalBones * h->totalFrames * sizeof ( GXBoneJoint );
	info.keys = static_cast<GXBoneJoint*> ( malloc ( size ) );
	memcpy ( info.keys, data + h->keysOffset, size );

	free ( data );
}
