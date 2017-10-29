//version 1.0

#include <GXPlugin_for_3ds_Max/GXMeshExporter.h>
#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXNativeMesh.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGameModifier.h>
#include <GXCommon/GXRestoreWarnings.h>


GXMeshExporter::GXMeshExporter ( INode &selection, const GXUTF8* fileName, GXBool isExportInCurrentPose ):
GXExporter ( selection, fileName )
{
	totalVertices = 0;
	vboData = nullptr;

	if ( !Init ( isExportInCurrentPose ) )
	{
		MessageBoxA ( 0, "Can't save mesh", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
		return;
	}

	Export ();
	MessageBoxA ( 0, "Mesh saved", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
}

GXMeshExporter::~GXMeshExporter ()
{
	GXSafeFree ( vboData );
}

GXUTF8* GXMeshExporter::GetWarnings () const
{
	//TODO
	return nullptr;
}

GXUTF8* GXMeshExporter::GetReport () const
{
	static GXUTF8 buffer[ 777 ];
	sprintf_s ( buffer, 777, "Vertices: %u\n", totalVertices );

	GXUTF8* report = nullptr;
	GXUTF8clone ( &report, buffer );

	return report;
}

GXBool GXMeshExporter::Export ()
{
	GXUPointer stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	GXUPointer vboSize = totalVertices * stride;
	GXUPointer size = sizeof ( GXNativeMeshHeader ) + vboSize;
	GXUByte* data = (GXUByte*)malloc ( size );

	GXNativeMeshHeader* h = (GXNativeMeshHeader*)data;
	h->totalVertices = totalVertices;

	GXUPointer offset = ( GXUBigInt )sizeof ( GXNativeMeshHeader );

	h->vboOffset = (GXUBigInt)offset;
	memcpy ( data + offset, vboData, vboSize );

	GXWChar* fileNameW;
	GXToWcs ( &fileNameW, fileName );

	GXBool result = GXWriteToFile ( fileNameW, data, size );

	free ( fileNameW );
	free ( data );

	return result;
}

GXBool GXMeshExporter::Init ( GXBool isExportInCurrentPose )
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

	if ( !ExtractGeometryData ( *obj, isExportInCurrentPose ) )
	{
		node->ReleaseIGameObject ();
		game->ReleaseIGame ();
		return GX_FALSE;
	}

	node->ReleaseIGameObject ();
	game->ReleaseIGame ();

	return GX_TRUE;
}

GXBool GXMeshExporter::ExtractGeometryData ( IGameObject &meshToExport, GXBool isExportInCurrentPose )
{
	if ( meshToExport.GetIGameType () != IGameMesh::IGAME_MESH ) return GX_FALSE;

	IGameMesh* mesh = nullptr;
	IGameSkin* skin = meshToExport.GetIGameSkin ();

	if ( skin )
		mesh = isExportInCurrentPose ? (IGameMesh*)skin : skin->GetInitialPose ();
	else
		mesh = (IGameMesh*)( &meshToExport );

	GXUInt totalFaces = (GXUInt)mesh->GetNumberOfFaces ();
	Tab<GXInt> texMaps = mesh->GetActiveMapChannelNum ();

	if ( texMaps.Count () <= 0 ) return GX_FALSE;

	totalVertices = totalFaces * 3;
	GXUPointer stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	vboData = (GXUByte*)malloc ( totalVertices * stride );
	GXUPointer offset = 0;

	for ( GXUInt i = 0; i < totalFaces; i++ )
	{
		FaceEx* face = mesh->GetFace ( (int)i );

		for ( GXUByte j = 0; j < 3; j++ )
		{
			Point3 p = mesh->GetVertex ( (int)face->vert[ j ] );
			GXVec3* v3 = (GXVec3*)( vboData + offset );
			v3->Init ( p.x, p.y, p.z );

			offset += sizeof ( GXVec3 );

			DWORD indices[ 3 ];

			if ( mesh->GetMapFaceIndex ( (int)texMaps[ 0 ], (int)i, indices ) )
				p = mesh->GetMapVertex ( (int)texMaps[ 0 ], (int)indices[ j ] );
			else
				p = mesh->GetMapVertex ( (int)texMaps[ 0 ], (int)face->vert[ j ] );

			GXVec2* v2 = (GXVec2*)( vboData + offset );
			v2->Init ( p.x, p.y );

			offset += sizeof ( GXVec2 );

			p = mesh->GetNormal ( face, j );
			v3 = (GXVec3*)( vboData + offset );
			v3->Init ( p.x, p.y, p.z );

			offset += sizeof ( GXVec3 );

			//Skip tangent and bitangent for a while.
			offset += sizeof ( GXVec3 ) + sizeof ( GXVec3 );
		}

		const GXUByte* vertices = (const GXUByte*)( vboData + i * 3 * stride );
		const GXUByte* uvs = vertices + sizeof ( GXVec3 );

		for ( GXUByte j = 0; j < 3; j++ )
		{
			GXVec3* tangent = (GXVec3*)( vboData + ( 3 * i + j ) * stride + sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) );
			GXVec3* bitangent = (GXVec3*)( vboData + ( 3 * i + j ) * stride + sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) );

			GXGetTangentBitangent ( *tangent, *bitangent, j, vertices, stride, uvs, stride );
		}
	}

	return GX_TRUE;
}
