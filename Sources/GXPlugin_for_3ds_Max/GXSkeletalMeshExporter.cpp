//version 1.1

#include <GXPlugin_for_3ds_Max/GXSkeletalMeshExporter.h>
#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGameModifier.h>
#include <GXCommon/GXRestoreWarnings.h>


#define GX_BONE_UNDEFINED	-1

struct GXBone
{
	GXUTF8			name[ GX_BONE_NAME_SIZE ];
	GXUShort		id;
	GXShort			parentID;
	IGameNode*		node;

	GXMat4			globalMat;
	GXQuatLocJoint	globalTransform;
	GXQuatLocJoint	refTransform;
	GXQuatLocJoint	bindTransform;

	GXBone*			father;
	GXBone*			brother;
	GXBone*			son;

	GXVoid Init ( GXUShort newID, IGameNode* newNode, const GXMat4 &newGlobalMatrix );
	GXVoid AddSon ( GXBone* newSon );
};

GXVoid GXBone::Init ( GXUShort newID, IGameNode* newNode, const GXMat4 &newGlobalMatrix )
{
	GXUTF8* s = nullptr;
	GXToUTF8 ( &s, node->GetName () );

	GXUInt size = GXUTF8size ( s );
	if ( size > GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) )
	{
		memcpy ( name, s, ( GX_BONE_NAME_SIZE - 1 ) * sizeof ( GXUTF8 ) );
		name[ GX_BONE_NAME_SIZE - 1 ] = 0;
	}
	else
	{
		memcpy ( name, s, size );
	}

	free ( s );

	id = newID;
	parentID = GX_BONE_UNDEFINED;
	node = newNode;

	globalMat = newGlobalMatrix;
	globalTransform.rotation.From ( globalMat );
	globalMat.GetW ( globalTransform.location );

	father = brother = son = nullptr;
}

GXVoid GXBone::AddSon ( GXBone* newSon )
{
	GXBone* elderSon = this->son;
	son = newSon;
	son->brother = elderSon;
	son->father = this;
}

//-----------------------------------------------------------------------------------------

class GXSkeleton
{
	private:
		GXBool		isValid;
		IGameSkin*	skin;

		GXUShort	numBones;
		GXUShort	currentBoneID;
		GXBone*		bones;
		GXBone*		rootBone;

	public:
		explicit GXSkeleton ( IGameObject &skinMesh );
		~GXSkeleton ();

		GXBool IsValid ();

		GXUShort GetBoneIndex ( IGameNode* bone ) const;
		const GXUTF8* GetBoneName ( GXUShort boneID ) const;
		GXShort GetBoneParent ( GXUShort boneID ) const;
		const GXQuatLocJoint* GetBoneRefTransform ( GXUShort boneID ) const;
		const GXQuatLocJoint* GetBoneBindTransform ( GXUShort boneID ) const;
		GXUShort GetBoneNumber () const;

	private:
		GXVoid AddBone ( IGameNode* bone );

		GXVoid ExtractBones ( IGameObject &skinMesh );
		GXVoid LinkBones ();

		GXVoid RebuildBoneArray ();
		GXVoid RegroupBones ( GXBone* bone, GXBone* allBones );
		GXVoid MakeReferencePose ();
		GXVoid MakeBindTransform ();

		GXShort Find ( IGameNode* bone ) const;
};

GXSkeleton::GXSkeleton ( IGameObject &skinMesh )
{
	isValid = GX_TRUE;
	skin = nullptr;

	numBones = (GXUShort)0;
	currentBoneID = (GXUShort)0;
	bones = nullptr;

	ExtractBones ( skinMesh );
	LinkBones ();

	RebuildBoneArray ();
	MakeReferencePose ();
	MakeBindTransform ();
}

GXSkeleton::~GXSkeleton ()
{
	GXSafeFree ( bones );
}

GXBool GXSkeleton::IsValid ()
{
	return isValid;
}

GXUShort GXSkeleton::GetBoneIndex ( IGameNode* bone ) const
{
	if ( !isValid ) return 0xFFFF;

	return (GXUShort)Find ( bone );
}

const GXUTF8* GXSkeleton::GetBoneName ( GXUShort boneID ) const
{
	if ( !isValid ) return nullptr;

	return bones[ boneID ].name;
}

GXShort GXSkeleton::GetBoneParent ( GXUShort boneID ) const
{
	if ( !isValid ) return GX_BONE_UNDEFINED;

	return bones[ boneID ].parentID;
}

const GXQuatLocJoint* GXSkeleton::GetBoneRefTransform ( GXUShort boneID ) const
{
	return &bones[ boneID ].refTransform;
}

const GXQuatLocJoint* GXSkeleton::GetBoneBindTransform ( GXUShort boneID ) const
{
	return &bones[ boneID ].bindTransform;
}

GXUShort GXSkeleton::GetBoneNumber () const
{
	if ( !isValid ) return 0xFFFF;

	return numBones;
}

GXVoid GXSkeleton::AddBone ( IGameNode* bone )
{
	GXShort id = Find ( bone );

	if ( id != GX_BONE_UNDEFINED ) return;

	GMatrix mat;
	skin->GetInitBoneTM ( bone, mat );
	GXMat4& m = (GXMat4&)mat;

	bones[ currentBoneID ].Init ( currentBoneID, bone, m );
	currentBoneID++;
}

GXVoid GXSkeleton::ExtractBones ( IGameObject &skinMesh )
{
	skin = skinMesh.GetIGameSkin ();
	if ( !skin )
	{
		isValid = GX_FALSE;
		return;
	}

	if ( IGameSkin::IGAME_SKIN != skin->GetSkinType () )
	{
		isValid = GX_FALSE;
		return;
	}

	numBones = (GXUShort)skin->GetTotalSkinBoneCount ();
	bones = (GXBone*)malloc ( numBones * sizeof ( GXBone ) );

	for ( GXUShort i = 0; i < numBones; i++ )
		AddBone ( skin->GetIGameBone ( i, false ) );
}

GXVoid GXSkeleton::LinkBones ()
{
	if ( !isValid ) return;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		IGameNode* father = bones[ i ].node->GetNodeParent ();

		bones[ i ].parentID = Find ( father );

		if ( bones[ i ].parentID != GX_BONE_UNDEFINED )
			bones[ bones[ i ].parentID ].AddSon ( bones + i );
		else
			rootBone = bones + i;
	}
}

GXVoid GXSkeleton::RebuildBoneArray ()
{
	if ( !isValid ) return;

	GXBone* oldBones = bones;
	bones = (GXBone*)malloc ( numBones * sizeof ( GXBone ) );

	currentBoneID = 0;
	RegroupBones ( rootBone, bones );

	for ( GXUShort i = 0; i < numBones; i++ )
		bones[ i ].parentID = Find ( bones[ i ].node->GetNodeParent () );

	free ( oldBones );
}

GXVoid GXSkeleton::RegroupBones ( GXBone* bone, GXBone* allBones )
{
	memcpy ( allBones + currentBoneID, bone, sizeof ( GXBone ) );
	allBones[ currentBoneID ].id = currentBoneID;
	currentBoneID++;

	bone = bone->son;

	while ( bone )
	{
		RegroupBones ( bone, allBones );
		bone = bone->brother;
	}
}

GXVoid GXSkeleton::MakeReferencePose ()
{
	if ( !isValid ) return;

	bones[ 0 ].refTransform.rotation.From ( bones[ 0 ].globalMat );
	bones[ 0 ].globalMat.GetW ( bones[ 0 ].refTransform.location );

	for ( GXUShort i = 1; i < numBones; i++ )
	{
		GXMat4 ref;
		GXMat4 invParent;

		invParent.Inverse ( bones[ bones[ i ].parentID ].globalMat );
		ref.Multiply ( bones[ i ].globalMat, invParent );

		bones[ i ].refTransform.rotation.From ( ref );
		ref.GetW ( bones[ i ].refTransform.location );
	}
}

GXVoid GXSkeleton::MakeBindTransform ()
{
	if ( !isValid ) return;

	for ( GXUInt i = 0; i < numBones; i++ )
	{
		bones[ i ].bindTransform.rotation.Inverse ( bones[ i ].globalTransform.rotation );
		bones[ i ].bindTransform.location = bones[ i ].globalTransform.location;
		bones[ i ].bindTransform.location.Reverse ();
	}
}

GXShort GXSkeleton::Find ( IGameNode* bone ) const
{
	if ( currentBoneID == 0 || !bone ) return GX_BONE_UNDEFINED;

	for ( GXUShort i = 0; i < currentBoneID; i++ )
	{
		if ( bones[ i ].node == bone ) return (GXShort)i;
	}

	return GX_BONE_UNDEFINED;
}

//------------------------------------------------------------------------------------------------

class GXIndexWeightStack
{
	private:
		GXVec2		iw[ 4 ];
		GXUByte		samples;

	public:
		GXIndexWeightStack ();
		~GXIndexWeightStack ();

		GXVoid Reset ();
		GXVoid Add ( GXFloat boneID, GXFloat weight );
		const GXVec2* GetStackData ();

	private:
		GXVoid Normalize ();
		GXVoid Replace ( GXFloat boneID, GXFloat weight );
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

GXVoid GXIndexWeightStack::Add ( GXFloat boneID, GXFloat weight )
{
	if ( samples >= 4 )
	{
		Replace ( boneID, weight );
		samples++;
		return;
	}

	iw[ samples ].Init ( boneID, weight );
	samples++;
}

const GXVec2* GXIndexWeightStack::GetStackData ()
{
	Normalize ();
	return iw;
}

GXVoid GXIndexWeightStack::Normalize ()
{
	if ( samples <= 4 ) return;

	GXFloat invSum = 1.0f / ( iw[ 0 ].data[ 1 ] + iw[ 1 ].data[ 1 ] + iw[ 2 ].data[ 1 ] + iw[ 3 ].data[ 1 ] );

	iw[ 0 ].data[ 1 ] *= invSum;
	iw[ 1 ].data[ 1 ] *= invSum;
	iw[ 2 ].data[ 1 ] *= invSum;
	iw[ 3 ].data[ 1 ] *= invSum;
}

GXVoid GXIndexWeightStack::Replace ( GXFloat boneID, GXFloat weight )
{
	GXUByte m = 0;

	for ( GXUByte i = 1; i < 4; i++ )
	{
		if ( iw[ i ].data[ 1 ] >= iw[ m ].data[ 1 ] ) continue;

		m = i;
	}

	iw[ m ].Init ( boneID, weight );
}

//------------------------------------------------------------------------------------------------

GXSkeletalMeshExporter::GXSkeletalMeshExporter ( INode &selection, const GXUTF8* fileName )
{
	isValid = GX_TRUE;

	skeleton = nullptr;
	numVertices = 0;
	vboData = nullptr;
	stack = nullptr;

	Init ( selection );

	if ( isValid )
	{
		Save ( fileName );
		MessageBoxA ( 0, "Skeletal mesh saved", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
	}
	else
		MessageBoxA ( 0, "Can't save skeletal mesh", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
}

GXSkeletalMeshExporter::~GXSkeletalMeshExporter ()
{
	GXSafeDelete ( skeleton );
	GXSafeDelete ( stack );
	GXSafeFree ( vboData );
}

GXVoid GXSkeletalMeshExporter::Init ( INode &selection )
{
	isValid = GX_TRUE;

	IGameScene* game = GetIGameInterface ();

	IGameConversionManager* cm = GetConversionManager ();
	
	UserCoord coordSystem;
	coordSystem.rotation = 0;
	coordSystem.uAxis = 1;
	coordSystem.vAxis = 0;
	coordSystem.xAxis = 1;
	coordSystem.yAxis = 2;
	coordSystem.zAxis = 4;
	cm->SetUserCoordSystem ( coordSystem );

	cm->SetCoordSystem ( IGameConversionManager::IGAME_USER );

	game->InitialiseIGame ();
	game->SetStaticFrame ( 0 );

	IGameNode* node = game->GetIGameNode ( &selection );

	if ( !node )
	{
		isValid = GX_FALSE;
		game->ReleaseIGame ();

		return;
	}

	IGameObject* obj = node->GetIGameObject ();
	obj->InitializeData ();

	skeleton = new GXSkeleton ( *obj );

	if ( !skeleton->IsValid () )
	{
		isValid = GX_FALSE;

		node->ReleaseIGameObject ();
		game->ReleaseIGame ();

		return;
	}

	ExtractGeometryData ( *obj );

	if ( !isValid )
	{
		GXSafeDelete ( skeleton );

		node->ReleaseIGameObject ();
		game->ReleaseIGame ();

		return;
	}

	node->ReleaseIGameObject ();
	game->ReleaseIGame ();
}

GXVoid GXSkeletalMeshExporter::ExtractGeometryData ( IGameObject &skinMesh )
{
	if ( skinMesh.GetIGameType () != IGameMesh::IGAME_MESH )
	{
		isValid = GX_FALSE;
		return;
	}

	IGameSkin* skin = skinMesh.GetIGameSkin ();
	IGameMesh* mesh = (IGameMesh*)( &skinMesh );
	GXUInt numFaces = (GXUInt)mesh->GetNumberOfFaces ();
	Tab<GXInt> texMaps = mesh->GetActiveMapChannelNum ();

	if ( texMaps.Count () <= 0 )
	{
		isValid = GX_FALSE;
		return;
	}

	numVertices = numFaces * 3;
	GXUByte stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );
	vboData = (GXUByte*)malloc ( numVertices * stride );
	GXUInt offset = 0;

	stack = new GXIndexWeightStack ();

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

			//Skip tangent and bitangent
			offset += sizeof ( GXVec3 ) + sizeof ( GXVec3 );

			GXUByte bpv = (GXUByte)skin->GetNumberOfBones ( (int)face->vert[ j ] );
			stack->Reset ();

			for ( GXUByte k = 0; k < bpv; k++ )
			{
				GXFloat boneID = (GXFloat)skeleton->GetBoneIndex ( skin->GetIGameBone ( (int)face->vert[ j ], (int)k ) );
				GXFloat weight = skin->GetWeight ( (int)face->vert[ j ], (int)k );
				stack->Add ( boneID, weight );
			}

			const GXVec2* iw = stack->GetStackData ();

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
}

GXVoid GXSkeletalMeshExporter::Save ( const GXUTF8* fileName ) const
{
	GXUInt numBones = skeleton->GetBoneNumber ();

	GXUByte stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );
	GXUInt vboSize = numVertices * stride;
	GXUPointer size = sizeof ( GXNativeSkeletalMeshHeader ) + vboSize + numBones * ( GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) + sizeof ( GXShort ) + sizeof ( GXQuatLocJoint ) + sizeof ( GXQuatLocJoint ) );
	GXUByte* data = (GXUByte*)malloc ( size );

	GXNativeSkeletalMeshHeader* h = (GXNativeSkeletalMeshHeader*)data;
	h->numVertices = numVertices;

	GXUInt offset = sizeof ( GXNativeSkeletalMeshHeader );

	h->vboOffset = offset;
	memcpy ( data + offset, vboData, vboSize );

	offset += vboSize;

	h->numBones = (GXUShort)numBones;
	h->boneNamesOffset = offset;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, skeleton->GetBoneName ( i ), GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) );
		offset += GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	}

	h->boneParentIndicesOffset = offset;

	GXShort* parent = (GXShort*)( data + offset );

	for ( GXUShort i = 0; i < numBones; i++ )
		parent[ i ] = skeleton->GetBoneParent ( i );

	offset += numBones * sizeof ( GXShort );

	h->refPoseOffset = offset;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, skeleton->GetBoneRefTransform ( i ), sizeof ( GXQuatLocJoint ) );
		offset += sizeof ( GXQuatLocJoint );
	}

	h->bindTransformOffset = offset;
	
	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, skeleton->GetBoneBindTransform ( i ), sizeof ( GXQuatLocJoint ) );
		offset += sizeof ( GXQuatLocJoint );
	}

	GXWChar* fileNameW;
	GXToWcs ( &fileNameW, fileName );

	GXWriteToFile ( fileNameW, data, size );

	free ( fileNameW );
	free ( data );
}

//------------------------------------------------------------------

GXAnimationExporter::GXAnimationExporter ( INode* selection, const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame )
{
	isValid = GX_TRUE;
	skin = 0;
	numBones = 0;
	bones = 0;
	game = 0;
	node = 0;

	Init ( selection );

	if ( isValid )
	{
		Save ( fileName, startFrame, lastFrame );
		node->ReleaseIGameObject ();
		game->ReleaseIGame ();
		MessageBoxA ( 0, "Animation is saved", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
	}
	else
		MessageBoxA ( 0, "Can't save animation", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
}

GXAnimationExporter::~GXAnimationExporter ()
{
	GXSafeFree ( bones );
}

GXVoid GXAnimationExporter::Init ( INode* selection )
{
	game = GetIGameInterface ();

	IGameConversionManager* cm = GetConversionManager ();
	
	UserCoord coordSystem;
	coordSystem.rotation = 0;
	coordSystem.uAxis = 1;
	coordSystem.vAxis = 0;
	coordSystem.xAxis = 1;
	coordSystem.yAxis = 2;
	coordSystem.zAxis = 4;
	cm->SetUserCoordSystem ( coordSystem );

	cm->SetCoordSystem ( IGameConversionManager::IGAME_USER );

	game->InitialiseIGame ();
	game->SetStaticFrame ( 0 );

	node = game->GetIGameNode ( selection );
	if ( !node )
	{
		isValid = GX_FALSE;
		game->ReleaseIGame ();

		return;
	}

	IGameObject* obj = node->GetIGameObject ();
	obj->InitializeData ();

	ExtractBones ( obj );

	if ( !isValid )
	{
		node->ReleaseIGameObject ();
		game->ReleaseIGame ();
	}
}

GXVoid GXAnimationExporter::Save ( const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame )
{
	if ( !isValid ) return;

	GXUInt numFrames = ( lastFrame - startFrame ) + 1;
	GXUInt size = sizeof ( GXNativeAnimationHeader ) + numBones * ( GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) ) + numFrames * numBones * sizeof ( GXQuatLocJoint );

	GXUByte* data = (GXUByte*)malloc ( size );

	GXNativeAnimationHeader* h = (GXNativeAnimationHeader*)data;

	h->fps = (GXFloat)GetFrameRate ();
	h->numFrames = numFrames;
	h->numBones = numBones;

	GXUInt offset = sizeof ( GXNativeAnimationHeader );
	h->boneNamesOffset = offset;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, bones[ i ].name, GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) );
		offset += GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	}

	h->keysOffset = offset;

	for ( GXUInt frame = startFrame; frame <= lastFrame; frame++ )
	{
		ExtractFrame ( frame );

		for ( GXUShort i = 0; i < numBones; i++ )
		{
			memcpy ( data + offset, &bones[ i ].refTransform, sizeof ( GXQuatLocJoint ) );
			offset += sizeof ( GXQuatLocJoint );
		}
	}

	GXWChar* fileNameW;
	GXToWcs ( &fileNameW, fileName );

	GXWriteToFile ( fileNameW, data, size );

	free ( fileNameW );
	free ( data );
}

GXVoid GXAnimationExporter::ExtractBones ( IGameObject* skinMesh )
{
	skin = skinMesh->GetIGameSkin ();
	if ( !skin )
	{
		isValid = GX_FALSE;
		return;
	}

	if ( IGameSkin::IGAME_SKIN != skin->GetSkinType () )
	{
		isValid = GX_FALSE;
		return;
	}

	numBones = (GXUShort)skin->GetTotalSkinBoneCount ();
	bones = (GXBone*)malloc ( numBones * sizeof ( GXBone ) );

	GXMat4 m;
	m.Identity ();

	for ( GXUShort i = 0; i < numBones; i++ )
		bones[ i ].Init ( i, skin->GetIGameBone ( i, false ), m );
}

GXVoid GXAnimationExporter::ExtractFrame ( GXUInt frame )
{
	GXUInt time = frame * GetTicksPerFrame ();

	for ( GXUShort i = (GXUShort)0; i < numBones; i++ )
	{
		GMatrix globalTransform = bones[ i ].node->GetWorldTM ( (TimeValue)time );
		GXMat4& globalMat = (GXMat4&)globalTransform;
		IGameNode* parent = bones[ i ].node->GetNodeParent ();

		if ( !parent )
		{
			globalMat.GetW ( bones[ i ].refTransform.location );
			bones[ i ].refTransform.rotation.From ( globalMat );

			continue;
		}

		GMatrix parentTransform = parent->GetWorldTM ( (TimeValue)time );
		GXMat4& parentMat = (GXMat4&)parentTransform;

		GXMat4 invParentMat;
		invParentMat.Inverse ( parentMat );

		GXMat4 refTransform;
		refTransform.Multiply ( globalMat, invParentMat );

		refTransform.GetW ( bones[ i ].refTransform.location );
		bones[ i ].refTransform.rotation.From ( refTransform );
	}
}
