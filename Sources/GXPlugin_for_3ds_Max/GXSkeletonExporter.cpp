//version 1.0

#include <GXPlugin_for_3ds_Max/GXSkeletonExporter.h>
#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXNativeSkeleton.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGameModifier.h>
#include <GXCommon/GXRestoreWarnings.h>


#define REPORT_BUFFER_SIZE 0x400000u	// 4Mb


GXSkeletonExporter::GXSkeletonExporter ( INode &selection, const GXUTF8* fileName ):
GXExporter ( selection, fileName )
{
	if ( !Init () )
	{
		MessageBoxA ( 0, "Can't save skeleton", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
		return;
	}

	Export ();
	MessageBoxA ( 0, "Skeleton saved", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
}

GXSkeletonExporter::~GXSkeletonExporter ()
{
	//NOTHING
}

GXUTF8* GXSkeletonExporter::GetWarnings () const
{
	//TODO
	return nullptr;
}

GXUTF8* GXSkeletonExporter::GetReport () const
{
	static GXUTF8 buffer[ REPORT_BUFFER_SIZE ];

	GXUPointer freeSpace = (GXUPointer)REPORT_BUFFER_SIZE;
	GXUPointer offset = (GXUPointer)0u;

	GXUShort totalBones = skeleton.GetTotalBones ();

	sprintf_s ( buffer, freeSpace, "Bones: %hu\n\n", totalBones );

	GXUPointer stringSize = strlen ( buffer );
	freeSpace -= stringSize;
	offset += stringSize;

	sprintf_s ( buffer + offset, freeSpace, "Hierarchy:\n" );

	stringSize = strlen ( buffer + offset );
	freeSpace -= stringSize;
	offset += stringSize;

	sprintf_s ( buffer + offset, freeSpace, "Bone #0: %s -> Parent bone: NONE\n", skeleton.GetBoneName ( 0 ) );

	stringSize = strlen ( buffer + offset );
	freeSpace -= stringSize;
	offset += stringSize;

	for ( GXUShort i = 1; i < totalBones; i++ )
	{
		sprintf_s ( buffer + offset, freeSpace, "Bone #%hu: %s -> Parent bone: %s\n", i, skeleton.GetBoneName ( i ), skeleton.GetBoneName ( skeleton.GetParentBoneIndex ( i ) ) );

		stringSize = strlen ( buffer + offset );
		freeSpace -= stringSize;
		offset += stringSize;
	}

	GXUTF8* report = nullptr;
	GXUTF8clone ( &report, buffer );

	return report;
}

GXBool GXSkeletonExporter::Export ()
{
	GXUShort totalBones = skeleton.GetTotalBones ();

	GXUPointer size = sizeof ( GXNativeSkeletonHeader ) + totalBones * ( GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) + sizeof ( GXShort ) + sizeof ( GXBoneJoint ) + sizeof ( GXBoneJoint ) );
	GXUByte* data = (GXUByte*)malloc ( size );

	GXNativeSkeletonHeader* h = (GXNativeSkeletonHeader*)data;

	GXUPointer offset = sizeof ( GXNativeSkeletonHeader );

	h->totalBones = totalBones;
	h->boneNamesOffset = (GXUBigInt)offset;

	for ( GXUShort i = 0; i < totalBones; i++ )
	{
		memcpy ( data + offset, skeleton.GetBoneName ( i ), GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) );
		offset += GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	}

	h->parentBoneIndicesOffset = (GXUBigInt)offset;

	GXUShort* parentIndices = (GXUShort*)( data + offset );

	for ( GXUShort i = 0; i < totalBones; i++ )
		parentIndices[ i ] = skeleton.GetParentBoneIndex ( i );

	offset += totalBones * sizeof ( GXShort );

	h->referensePoseOffset = (GXUBigInt)offset;

	for ( GXUShort i = 0; i < totalBones; i++ )
	{
		memcpy ( data + offset, &( skeleton.GetBoneReferenceTransform ( i ) ), sizeof ( GXBoneJoint ) );
		offset += sizeof ( GXBoneJoint );
	}

	h->inverseBindTransformOffset = (GXUBigInt)offset;

	for ( GXUShort i = 0; i < totalBones; i++ )
	{
		memcpy ( data + offset, &( skeleton.GetBoneInverseBindTransform ( i ) ), sizeof ( GXBoneJoint ) );
		offset += sizeof ( GXBoneJoint );
	}

	GXWChar* fileNameW;
	GXToWcs ( &fileNameW, fileName );

	GXBool result = GXWriteToFile ( fileNameW, data, size );

	free ( fileNameW );
	free ( data );

	return result;
}

GXBool GXSkeletonExporter::Init ()
{
	IGameScene* game = GetIGameInterface ();
	IGameConversionManager* cm = GetConversionManager ();

	UserCoord coordSystem;
	coordSystem.rotation = 0;
	coordSystem.xAxis = 1;
	coordSystem.yAxis = 2;
	coordSystem.zAxis = 4;
	coordSystem.uAxis = 1;
	coordSystem.vAxis = 0;
	cm->SetUserCoordSystem ( coordSystem );

	cm->SetCoordSystem ( IGameConversionManager::IGAME_USER );

	game->InitialiseIGame ();
	game->SetStaticFrame ( 0 );

	IGameNode* node = game->GetIGameNode ( &selection );

	if ( !node )
	{
		game->ReleaseIGame ();
		return GX_FALSE;
	}

	IGameObject* obj = node->GetIGameObject ();
	obj->InitializeData ();

	skeleton.From ( *obj );

	if ( !skeleton.IsValid () )
	{
		node->ReleaseIGameObject ();
		game->ReleaseIGame ();
		return GX_FALSE;
	}

	skeleton.CalculateReferenceTransform ();
	skeleton.CalculateInverseBindTransform ();

	node->ReleaseIGameObject ();
	game->ReleaseIGame ();

	return GX_TRUE;
}
