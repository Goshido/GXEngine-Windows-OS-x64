//version 1.0

#include <GXPlugin_for_3ds_Max/GXAnimationExporter.h>
#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGameModifier.h>
#include <GXCommon/GXRestoreWarnings.h>


GXAnimationExporter::GXAnimationExporter ( INode& selection, const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame )
{
	game = nullptr;
	node = nullptr;

	if ( Init ( selection ) )
	{
		Save ( fileName, startFrame, lastFrame );
		MessageBoxA ( 0, "Animation is saved", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
		return;
	}

	MessageBoxA ( 0, "Can't save animation", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
}

GXAnimationExporter::~GXAnimationExporter ()
{
	//NOTHING
}

GXBool GXAnimationExporter::Init ( INode& selection )
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

GXVoid GXAnimationExporter::Save ( const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame )
{
	GXUInt totalFrames = ( lastFrame - startFrame ) + 1;
	GXUPointer size = sizeof ( GXNativeAnimationHeader ) + skeleton.GetTotalBones () * ( ( GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) ) + totalFrames * sizeof ( GXBoneJoint ) );

	GXUByte* data = (GXUByte*)malloc ( size );

	GXNativeAnimationHeader* h = (GXNativeAnimationHeader*)data;

	h->fps = (GXFloat)GetFrameRate ();
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

	GXWriteToFile ( fileNameW, data, size );

	free ( fileNameW );
	free ( data );

	node->ReleaseIGameObject ();
	game->ReleaseIGame ();
}
