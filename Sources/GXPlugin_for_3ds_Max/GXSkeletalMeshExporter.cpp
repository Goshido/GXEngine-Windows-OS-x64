//version 1.1

#include <GXPlugin_for_3ds_Max/GXSkeletalMeshExporter.h>
#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXPlugin_for_3ds_Max/GXSkeleton.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXNativeSkeletalMesh.h>
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

GXSkeletalMeshExporter::GXSkeletalMeshExporter ( INode &selection, const GXUTF8* fileName, const GXUTF8* reportFileName )
{
	numVertices = 0;
	vboData = nullptr;

	if ( !Init ( selection ) )
	{
		MessageBoxA ( 0, "Can't save skeletal mesh", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
		return;
	}

	Save ( fileName );

	if ( reportFileName )
		SaveReport ( reportFileName );

	MessageBoxA ( 0, "Skeletal mesh saved", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
}

GXSkeletalMeshExporter::~GXSkeletalMeshExporter ()
{
	GXSafeFree ( vboData );
}

GXBool GXSkeletalMeshExporter::Init ( INode &selection )
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

GXBool GXSkeletalMeshExporter::ExtractGeometryData ( IGameObject &skinMesh )
{
	if ( skinMesh.GetIGameType () != IGameMesh::IGAME_MESH ) return GX_FALSE;

	IGameSkin* skin = skinMesh.GetIGameSkin ();
	IGameMesh* mesh = (IGameMesh*)( &skinMesh );
	GXUInt numFaces = (GXUInt)mesh->GetNumberOfFaces ();
	Tab<GXInt> texMaps = mesh->GetActiveMapChannelNum ();

	if ( texMaps.Count () <= 0 ) return GX_FALSE;

	numVertices = numFaces * 3;
	GXUByte stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );
	vboData = (GXUByte*)malloc ( numVertices * stride );
	GXUInt offset = 0;

	GXIndexWeightStack stack;

	for ( GXUInt i = 0; i < numFaces; i++ )
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

GXVoid GXSkeletalMeshExporter::Save ( const GXUTF8* fileName ) const
{
	GXUInt numBones = skeleton.GetTotalBones ();

	GXUByte stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );
	GXUInt vboSize = numVertices * stride;
	GXUPointer size = sizeof ( GXNativeSkeletalMeshHeader ) + vboSize + numBones * ( GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) + sizeof ( GXShort ) + sizeof ( GXQuatLocJoint ) + sizeof ( GXMat4 ) + sizeof ( GXQuatLocJoint ) + sizeof ( GXMat4 ) );
	GXUByte* data = (GXUByte*)malloc ( size );

	GXNativeSkeletalMeshHeader* h = (GXNativeSkeletalMeshHeader*)data;
	h->totalVertices = numVertices;

	GXUPointer offset = (GXUBigInt)sizeof ( GXNativeSkeletalMeshHeader );

	h->vboOffset = (GXUBigInt)offset;
	memcpy ( data + offset, vboData, vboSize );

	offset += (GXUBigInt)vboSize;

	h->totalBones = (GXUShort)numBones;
	h->boneNamesOffset = offset;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, skeleton.GetBoneName ( i ), GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) );
		offset += GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	}

	h->parentBoneIndicesOffset = (GXUBigInt)offset;

	GXUShort* parentIndices = (GXUShort*)( data + offset );

	for ( GXUShort i = 0; i < numBones; i++ )
		parentIndices[ i ] = skeleton.GetParentBoneIndex ( i );

	offset += numBones * sizeof ( GXShort );

	h->referensePoseOffset = (GXUBigInt)offset;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, &( skeleton.GetBoneReferenceTransform ( i ) ), sizeof ( GXQuatLocJoint ) );
		offset += sizeof ( GXQuatLocJoint );
	}

	h->referensePoseOffset2 = (GXUBigInt)offset;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, &( skeleton.GetBoneReferenceTransform2 ( i ) ), sizeof ( GXMat4 ) );
		offset += sizeof ( GXMat4 );
	}

	h->inverseBindTransformOffset = (GXUBigInt)offset;
	
	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, &( skeleton.GetBoneInverseBindTransform ( i ) ), sizeof ( GXQuatLocJoint ) );
		offset += sizeof ( GXQuatLocJoint );
	}

	h->inverseBindTransformOffset2 = (GXUBigInt)offset;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, &( skeleton.GetBoneInverseBindTransform2 ( i ) ), sizeof ( GXMat4 ) );
		offset += sizeof ( GXMat4 );
	}

	GXWChar* fileNameW;
	GXToWcs ( &fileNameW, fileName );

	GXWriteToFile ( fileNameW, data, size );

	free ( fileNameW );
	free ( data );
}

GXVoid GXSkeletalMeshExporter::SaveReport ( const GXUTF8* fileName ) const
{
	GXWChar* fileNameW;
	GXToWcs ( &fileNameW, fileName );

	static GXUTF8 buffer[ 777 ];
	GXWriteFileStream fs ( fileNameW );

	free ( fileNameW );

	sprintf_s ( buffer, 777, "Vertices: %u\n", numVertices );
	fs.Write ( buffer, strlen ( buffer ) );

	sprintf_s ( buffer, 777, "Bones: %hu\n\n", skeleton.GetTotalBones () );
	fs.Write ( buffer, strlen ( buffer ) );

	sprintf_s ( buffer, 777, "Hierarchy:\n" );
	fs.Write ( buffer, strlen ( buffer ) );

	sprintf_s ( buffer, 777, "Bone #0: %s -> Parent bone: NONE\n", skeleton.GetBoneName ( 0 ) );
	fs.Write ( buffer, strlen ( buffer ) );

	for ( GXUShort i = 1; i < skeleton.GetTotalBones (); i++ )
	{
		sprintf_s ( buffer, 777, "Bone #%hu: %s -> Parent bone: %s\n", i, skeleton.GetBoneName ( i ), skeleton.GetBoneName ( skeleton.GetParentBoneIndex ( i ) ) );
		fs.Write ( buffer, strlen ( buffer ) );
	}

	fs.Close ();
}
