//version 1.0

#include <GXPlugin_for_3ds_Max/GXAnimationExporter.h>
#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXCommon/GXNativeAnimation.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGameModifier.h>
#include <GXCommon/GXRestoreWarnings.h>


#define REPORT_BUFFER_SIZE 0x400000u	// 4Mb


GXAnimationExporter::GXAnimationExporter ( INode& selection, const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame ):
GXExporter ( selection, fileName )
{
	game = nullptr;
	node = nullptr;
	this->startFrame = startFrame;
	this->lastFrame = lastFrame;
	fps = 0.0f;

	if ( Init () )
	{
		Export ();
		MessageBoxA ( 0, "Animation is saved", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
		return;
	}

	MessageBoxA ( 0, "Can't save animation", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
}

GXAnimationExporter::~GXAnimationExporter ()
{
	//NOTHING
}

GXUTF8* GXAnimationExporter::GetWarnings () const
{
	//TODO
	return nullptr;
}

GXUTF8* GXAnimationExporter::GetReport () const
{
	static GXUTF8 buffer[ REPORT_BUFFER_SIZE ];

	GXUPointer freeSpace = (GXUPointer)REPORT_BUFFER_SIZE;
	GXUPointer offset = (GXUPointer)0u;

	sprintf_s ( buffer, (size_t)REPORT_BUFFER_SIZE, "Frames per second: %f\n\n", fps );

	GXUPointer stringSize = strlen ( buffer );
	freeSpace -= stringSize;
	offset += stringSize;

	sprintf_s ( buffer, freeSpace, "Frames: %u\n\n", lastFrame - startFrame + 1 );

	stringSize = strlen ( buffer + offset );
	freeSpace -= stringSize;
	offset += stringSize;

	GXUShort totalBones = skeleton.GetTotalBones ();

	sprintf_s ( buffer, freeSpace, "Keys: %u\n\n", (GXUInt)( ( lastFrame - startFrame + 1 ) * totalBones ) );

	stringSize = strlen ( buffer + offset );
	freeSpace -= stringSize;
	offset += stringSize;

	sprintf_s ( buffer, freeSpace, "Active bones: %hu\n\n", totalBones );

	stringSize = strlen ( buffer + offset );
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

GXBool GXAnimationExporter::Export ()
{
	GXUInt totalFrames = ( lastFrame - startFrame ) + 1;
	GXUPointer size = sizeof ( GXNativeAnimationHeader ) + skeleton.GetTotalBones () * ( ( GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) ) + totalFrames * sizeof ( GXBoneJoint ) );

	GXUByte* data = (GXUByte*)malloc ( size );

	GXNativeAnimationHeader* h = (GXNativeAnimationHeader*)data;

	fps = h->fps = (GXFloat)GetFrameRate ();
	h->totalFrames = totalFrames;
	h->totalBones = skeleton.GetTotalBones ();

	GXUPointer offset = sizeof ( GXNativeAnimationHeader );
	h->boneNamesOffset = (GXUBigInt)offset;

	for ( GXUShort i = 0; i < h->totalBones; i++ )
	{
		memcpy ( data + offset, skeleton.GetBoneName ( i ), GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) );
		offset += GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	}

	h->keysOffset = (GXUBigInt)offset;

	for ( GXUInt frame = startFrame; frame <= lastFrame; frame++ )
	{
		skeleton.CalculatePoseTransform ( frame );

		for ( GXUShort i = 0; i < h->totalBones; i++ )
		{
			memcpy ( data + offset, &skeleton.GetBonePoseTransform ( i ), sizeof ( GXBoneJoint ) );
			offset += sizeof ( GXBoneJoint );
		}
	}

	GXWChar* fileNameW;
	GXToWcs ( &fileNameW, fileName );

	GXBool result = GXWriteToFile ( fileNameW, data, size );

	free ( fileNameW );
	free ( data );

	node->ReleaseIGameObject ();
	game->ReleaseIGame ();

	return result;
}

GXBool GXAnimationExporter::Init ()
{
	game = GetIGameInterface ();

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

	node = game->GetIGameNode ( &selection );

	if ( !node )
	{
		game->ReleaseIGame ();
		return GX_FALSE;
	}

	IGameObject* gameObject = node->GetIGameObject ();
	gameObject->InitializeData ();

	skeleton.From ( *gameObject );

	if ( !skeleton.IsValid () )
	{
		node->ReleaseIGameObject ();
		game->ReleaseIGame ();

		return GX_FALSE;
	}

	return GX_TRUE;
}
