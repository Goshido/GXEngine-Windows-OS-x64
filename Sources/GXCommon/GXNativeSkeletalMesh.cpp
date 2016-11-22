//version 1.0

#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>


GXSkeletalMeshData::GXSkeletalMeshData ()
{
	numVertices = 0;
	vboData = 0;
	numBones = 0;
	boneNames = 0;
	parentIndex = 0;
	refPose = bindTransform = 0;
}

GXVoid GXSkeletalMeshData::Cleanup ()
{
	GXSafeFree ( vboData );
	GXSafeFree ( parentIndex );
	GXSafeFree ( refPose );
	GXSafeFree ( bindTransform );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeletalMesh ( const GXWChar* fileName, GXSkeletalMeshData &info )
{
	GXUByte* data;
	GXUInt size;

	if ( !GXLoadFile ( fileName, (GXVoid**)&data, size, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadNativeSkeletalMesh::Error - Can't load file %s\n", fileName );
		return;
	}

	GXNativeSkeletalMeshHeader* h = (GXNativeSkeletalMeshHeader*)data;

	info.numVertices = h->numVertices;

	size = info.numVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
	info.vboData = (GXFloat*)malloc ( size );
	memcpy ( info.vboData, data + h->vboOffset, size );

	info.numBones = h->numBones;

	size = info.numBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	info.boneNames = (GXUTF8*)malloc ( size );
	memcpy ( info.boneNames, data + h->boneNamesOffset, size );

	size = info.numBones * sizeof ( GXShort );
	info.parentIndex = (GXShort*)malloc ( size );
	memcpy ( info.parentIndex, data + h->boneParentIndicesOffset, size );

	size = info.numBones * sizeof ( GXQuatLocJoint );
	info.refPose = (GXQuatLocJoint*)malloc ( size );
	memcpy ( info.refPose, data + h->refPoseOffset, size );

	info.bindTransform = (GXQuatLocJoint*)malloc ( size );
	memcpy ( info.bindTransform, data + h->bindTransformOffset, size );

	free ( data );
}

GXVoid GXCALL GXLoadNativeAnimation ( const GXWChar* fileName, GXAnimationInfoExt &info )
{
	GXUByte* data;
	GXUInt size;

	if ( !GXLoadFile ( fileName, (GXVoid**)&data, size, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadNativeAnimation::Error - Can't load file %s\n", fileName );
		return;
	}

	GXNativeAnimationHeader* h = (GXNativeAnimationHeader*)data;

	info.fps = h->fps;
	info.numBones = h->numBones;
	info.numFrames = h->numFrames;
	
	size = h->numBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	info.boneNames = (GXUTF8*)malloc ( size );
	memcpy ( info.boneNames, data + h->boneNamesOffset, size );

	size = h->numBones * h->numFrames * sizeof ( GXQuatLocJoint );
	info.keys = (GXQuatLocJoint*)malloc ( size );
	memcpy ( info.keys, data + h->keysOffset, size );

	free ( data );
}
