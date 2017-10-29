//version 1.0

#include <GXCommon/GXNativeAnimation.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


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
