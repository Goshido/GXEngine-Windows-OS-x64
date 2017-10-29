//version 1.0

#include <GXPlugin_for_3ds_Max/GXSkinExporter.h>
#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXPlugin_for_3ds_Max/GXSkeleton.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXNativeSkin.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGameModifier.h>
#include <GXCommon/GXRestoreWarnings.h>


#define BONE_WEIGHT_SUM_EPSILON			1.0e-4f


class GXIndexWeightStack
{
	private:
	GXVec2		iw[ 4 ];
	GXUByte		samples;

	public:
	GXIndexWeightStack ();
	~GXIndexWeightStack ();

	GXVoid Reset ();
	GXVoid Add ( GXFloat boneIndex, GXFloat weight );
	const GXVec2* GetStackData ();
	GXVoid Check () const;

	private:
	GXVoid Normalize ();
	GXVoid Replace ( GXFloat boneIndex, GXFloat weight );
};

GXIndexWeightStack::GXIndexWeightStack ()
{
	Reset ();
}

GXIndexWeightStack::~GXIndexWeightStack ()
{
	//NOTHING
}

GXVoid GXIndexWeightStack::Reset ()
{
	samples = 0;
	memset ( iw, 0, 4 * sizeof ( GXVec2 ) );
}

GXVoid GXIndexWeightStack::Add ( GXFloat boneIndex, GXFloat weight )
{
	if ( samples >= 4 )
	{
		Replace ( boneIndex, weight );
		return;
	}

	iw[ samples ].Init ( boneIndex, weight );
	samples++;
}

const GXVec2* GXIndexWeightStack::GetStackData ()
{
	Normalize ();
	return iw;
}

GXVoid GXIndexWeightStack::Check () const
{
	GXFloat sum = 0.0f;

	for ( GXUByte i = 0; i < samples; i++ )
		sum += iw[ i ].data[ 1 ];

	if ( fabsf ( 1.0f - sum ) < BONE_WEIGHT_SUM_EPSILON )return;

	static GXChar buffer[ 777 ];
	sprintf_s ( buffer, 777, "Weight normalization error:\n%g: %f\n%g: %f\n%g: %f\n%g: %f\nSum: %f", iw[ 0 ].data[ 0 ], iw[ 0 ].data[ 1 ], iw[ 1 ].data[ 0 ], iw[ 1 ].data[ 1 ], iw[ 2 ].data[ 0 ], iw[ 2 ].data[ 1 ], iw[ 3 ].data[ 0 ], iw[ 3 ].data[ 1 ], sum );

	MessageBoxA ( 0, buffer, GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING | MB_OK );
}

GXVoid GXIndexWeightStack::Normalize ()
{
	GXFloat sum = 0.0f;

	for ( GXUByte i = 0; i < samples; i++ )
		sum += iw[ i ].data[ 1 ];

	GXFloat invSum = 1.0f / sum;

	iw[ 0 ].data[ 1 ] *= invSum;
	iw[ 1 ].data[ 1 ] *= invSum;
	iw[ 2 ].data[ 1 ] *= invSum;
	iw[ 3 ].data[ 1 ] *= invSum;
}

GXVoid GXIndexWeightStack::Replace ( GXFloat boneIndex, GXFloat weight )
{
	GXUByte m = 0;

	for ( GXUByte i = 1; i < 4; i++ )
	{
		if ( iw[ m ].data[ 1 ] > iw[ i ].data[ 1 ] ) continue;

		m = i;
	}

	iw[ m ].Init ( boneIndex, weight );
}

//------------------------------------------------------------------------------------------------

GXSkinExporter::GXSkinExporter ( INode &selection, const GXUTF8* fileName ):
GXExporter ( selection, fileName )
{
	totalVertices = 0;
	vboData = nullptr;

	if ( !Init () )
	{
		MessageBoxA ( 0, "Can't save skin", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
		return;
	}

	Export ();
	MessageBoxA ( 0, "Skin saved", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
}

GXSkinExporter::~GXSkinExporter ()
{
	GXSafeFree ( vboData );
}

GXUTF8* GXSkinExporter::GetWarnings () const
{
	//TODO
	return nullptr;
}

GXUTF8* GXSkinExporter::GetReport () const
{
	static GXUTF8 buffer[ 777 ];
	sprintf_s ( buffer, 777, "Vertices: %u\n", totalVertices );

	GXUTF8* report = nullptr;
	GXUTF8clone ( &report, buffer );

	return report;
}

GXBool GXSkinExporter::Export ()
{
	GXUByte stride = sizeof ( GXVec4 ) + sizeof ( GXVec4 );
	GXUInt vboSize = totalVertices * stride;
	GXUPointer size = sizeof ( GXNativeSkinHeader ) + vboSize;
	GXUByte* data = (GXUByte*)malloc ( size );

	GXNativeSkinHeader* h = (GXNativeSkinHeader*)data;
	h->totalVertices = totalVertices;

	GXUPointer offset = sizeof ( GXNativeSkinHeader );

	h->vboOffset = (GXUBigInt)offset;
	memcpy ( data + offset, vboData, vboSize );

	GXWChar* fileNameW;
	GXToWcs ( &fileNameW, fileName );

	GXBool result = GXWriteToFile ( fileNameW, data, size );

	free ( fileNameW );
	free ( data );

	return result;
}

GXBool GXSkinExporter::Init ()
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

	if ( !ExtractGeometryData ( *obj ) )
	{
		node->ReleaseIGameObject ();
		game->ReleaseIGame ();
		return GX_FALSE;
	}

	node->ReleaseIGameObject ();
	game->ReleaseIGame ();

	return GX_TRUE;
}

GXBool GXSkinExporter::ExtractGeometryData ( IGameObject &skinMesh )
{
	if ( skinMesh.GetIGameType () != IGameMesh::IGAME_MESH ) return GX_FALSE;

	IGameSkin* skin = skinMesh.GetIGameSkin ();
	IGameMesh* mesh = skin->GetInitialPose ();
	GXUInt totalFaces = (GXUInt)mesh->GetNumberOfFaces ();
	Tab<GXInt> texMaps = mesh->GetActiveMapChannelNum ();

	if ( texMaps.Count () <= 0 ) return GX_FALSE;

	totalVertices = totalFaces * 3;
	GXUPointer stride = sizeof ( GXVec4 ) + sizeof ( GXVec4 );
	vboData = (GXUByte*)malloc ( totalVertices * stride );
	GXUInt offset = 0;

	GXIndexWeightStack stack;

	for ( GXUInt i = 0; i < totalFaces; i++ )
	{
		FaceEx* face = mesh->GetFace ( (int)i );

		for ( GXUByte j = 0; j < 3; j++ )
		{
			GXUByte bpv = (GXUByte)skin->GetNumberOfBones ( (int)face->vert[ j ] );
			stack.Reset ();

			for ( GXUByte k = 0; k < bpv; k++ )
			{
				GXFloat boneIndex = (GXFloat)skeleton.GetBoneIndex ( skin->GetIGameBone ( (int)face->vert[ j ], (int)k ) );
				GXFloat boneWeight = skin->GetWeight ( (int)face->vert[ j ], (int)k );
				stack.Add ( boneIndex, boneWeight );
			}

			const GXVec2* iw = stack.GetStackData ();

			stack.Check ();

			GXVec4* v4 = (GXVec4*)( vboData + offset );
			v4->Init ( iw[ 0 ].data[ 0 ], iw[ 1 ].data[ 0 ], iw[ 2 ].data[ 0 ], iw[ 3 ].data[ 0 ] );

			offset += sizeof ( GXVec4 );

			v4 = (GXVec4*)( vboData + offset );
			v4->Init ( iw[ 0 ].data[ 1 ], iw[ 1 ].data[ 1 ], iw[ 2 ].data[ 1 ], iw[ 3 ].data[ 1 ] );

			offset += sizeof ( GXVec4 );
		}
	}

	return GX_TRUE;
}
