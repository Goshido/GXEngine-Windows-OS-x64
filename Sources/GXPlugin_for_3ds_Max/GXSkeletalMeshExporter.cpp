//version 1.1

#include <GXPlugin_for_3ds_Max/GXSkeletalMeshExporter.h>
#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGameModifier.h>
#include <GXCommon/GXRestoreWarnings.h>


#define UNDEFINED_BONE_INDEX			0xFFFF
#define UNKNOWN_BONE_NUMBER				0xFFFF
#define ROOT_BONE_INDEX					0xFFFF


struct GXBone
{
	GXUTF8			name[ GX_BONE_NAME_SIZE ];
	GXUShort		ownIndex;
	GXUShort		parentIndex;
	GXBool			isRootBone;

	IGameNode*		node;

	GXMat4			transformWorld;
	GXQuatLocJoint	referenceTransform;
	GXQuatLocJoint	bindTransform;

	GXBone*			father;
	GXBone*			brother;
	GXBone*			son;

	GXVoid Init ( GXUShort newID, IGameNode* newNode, const GXMat4 &newTransformWorld );
	GXVoid AddSon ( GXBone* newSon );

	GXBone& operator = ( const GXBone &other );
};

GXVoid GXBone::Init ( GXUShort newOwnIndex, IGameNode* newNode, const GXMat4 &newTransformWorld )
{
	GXUTF8* s = nullptr;
	GXToUTF8 ( &s, newNode->GetName () );
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

	ownIndex = newOwnIndex;
	parentIndex = UNDEFINED_BONE_INDEX;
	isRootBone = GX_FALSE;
	node = newNode;
	transformWorld = newTransformWorld;

	father = brother = son = nullptr;
}

GXVoid GXBone::AddSon ( GXBone* newSon )
{
	GXBone* elderSon = this->son;
	son = newSon;
	son->brother = elderSon;
	son->father = this;
}

GXBone& GXBone::operator = ( const GXBone &other )
{
	memcpy ( this, &other, sizeof ( GXBone ) );
	return *this;
}

//-----------------------------------------------------------------------------------------

class GXSkeleton
{
	private:
		GXBool		isValid;
		IGameSkin*	skin;

		GXUShort	totalBones;
		GXUShort	currentBoneIndex;
		GXBone*		bones;
		GXBone*		rootBone;

	public:
		explicit GXSkeleton ( IGameObject &skinMesh );
		~GXSkeleton ();

		GXBool IsValid ();

		const GXUTF8* GetBoneName ( GXUShort boneIndex ) const;

		GXUShort GetBoneIndex ( IGameNode* bone ) const;
		GXUShort GetBoneParentIndex ( GXUShort boneIndex ) const;

		const GXQuatLocJoint* GetBoneReferenceTransform ( GXUShort boneIndex ) const;
		const GXQuatLocJoint* GetBoneBindTransform ( GXUShort boneIndex ) const;

		GXUShort GetTotalBones () const;

	private:
		GXVoid AddBone ( IGameNode* bone );

		GXVoid ExtractBones ( IGameObject &skinMesh );
		GXVoid LinkBones ();

		GXVoid RebuildBoneArray ();
		GXVoid RegroupBones ( GXBone* bone );

		GXVoid CalculateReferenceTransform ();
		GXVoid CalculateBindTransform ();

		GXBool IsRootBone ( IGameNode* boneFather ) const;
		GXUShort FindBoneIndex ( IGameNode* bone ) const;
};

GXSkeleton::GXSkeleton ( IGameObject &skinMesh )
{
	isValid = GX_TRUE;
	skin = nullptr;

	totalBones = (GXUShort)0;
	currentBoneIndex = (GXUShort)0;
	bones = nullptr;
	rootBone = nullptr;

	ExtractBones ( skinMesh );
	LinkBones ();
	RebuildBoneArray ();

	CalculateReferenceTransform ();
	CalculateBindTransform ();
}

GXSkeleton::~GXSkeleton ()
{
	GXSafeFree ( bones );
}

GXBool GXSkeleton::IsValid ()
{
	return isValid;
}

const GXUTF8* GXSkeleton::GetBoneName ( GXUShort boneIndex ) const
{
	if ( !isValid ) return nullptr;

	return bones[ boneIndex ].name;
}

GXUShort GXSkeleton::GetBoneIndex ( IGameNode* bone ) const
{
	if ( !isValid ) return UNDEFINED_BONE_INDEX;

	return FindBoneIndex ( bone );
}

GXUShort GXSkeleton::GetBoneParentIndex ( GXUShort boneIndex ) const
{
	if ( !isValid ) return UNDEFINED_BONE_INDEX;

	if ( bones[ boneIndex ].isRootBone ) return UNDEFINED_BONE_INDEX;

	return bones[ boneIndex ].parentIndex;
}

const GXQuatLocJoint* GXSkeleton::GetBoneReferenceTransform ( GXUShort boneID ) const
{
	return &bones[ boneID ].referenceTransform;
}

const GXQuatLocJoint* GXSkeleton::GetBoneBindTransform ( GXUShort boneID ) const
{
	return &bones[ boneID ].bindTransform;
}

GXUShort GXSkeleton::GetTotalBones () const
{
	if ( !isValid ) return UNKNOWN_BONE_NUMBER;

	return totalBones;
}

GXVoid GXSkeleton::AddBone ( IGameNode* bone )
{
	if ( FindBoneIndex ( bone ) != UNDEFINED_BONE_INDEX ) return;

	GMatrix matrix;
	skin->GetInitBoneTM ( bone, matrix );

	GXMat4 transformWorld;
	memcpy ( &transformWorld, &matrix, sizeof ( GXMat4 ) );

	bones[ currentBoneIndex ].Init ( currentBoneIndex, bone, transformWorld );
	currentBoneIndex++;
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

	totalBones = (GXUShort)skin->GetTotalSkinBoneCount ();
	bones = (GXBone*)malloc ( totalBones * sizeof ( GXBone ) );

	for ( GXUShort i = 0; i < totalBones; i++ )
		AddBone ( skin->GetIGameBone ( i, false ) );
}

GXVoid GXSkeleton::LinkBones ()
{
	if ( !isValid ) return;

	for ( GXUShort i = 0; i < totalBones; i++ )
	{
		IGameNode* father = bones[ i ].node->GetNodeParent ();

		if ( IsRootBone ( father ) )
		{
			bones[ i ].isRootBone = GX_TRUE;
			rootBone = bones + i;
		}
		else
		{
			bones[ i ].isRootBone = GX_FALSE;
			bones[ i ].parentIndex = FindBoneIndex ( father );
			bones[ bones[ i ].parentIndex ].AddSon ( bones + i );
		}
	}
}

GXVoid GXSkeleton::RebuildBoneArray ()
{
	if ( !isValid ) return;

	GXBone* oldBones = bones;
	bones = (GXBone*)malloc ( totalBones * sizeof ( GXBone ) );

	currentBoneIndex = 0;
	RegroupBones ( rootBone );

	bones[ 0 ].isRootBone = GX_TRUE;
	bones[ 0 ].parentIndex = ROOT_BONE_INDEX;

	for ( GXUShort i = 1; i < totalBones; i++ )
		bones[ i ].parentIndex = FindBoneIndex ( bones[ i ].node->GetNodeParent () );

	free ( oldBones );
}

GXVoid GXSkeleton::RegroupBones ( GXBone* bone )
{
	bones[ currentBoneIndex ] = *bone;
	bones[ currentBoneIndex ].ownIndex = currentBoneIndex;
	currentBoneIndex++;

	bone = bone->son;

	while ( bone )
	{
		RegroupBones ( bone );
		bone = bone->brother;
	}
}

GXVoid GXSkeleton::CalculateReferenceTransform ()
{
	if ( !isValid ) return;

	bones[ 0 ].referenceTransform.rotation.From ( bones[ 0 ].transformWorld );
	bones[ 0 ].transformWorld.GetW ( bones[ 0 ].referenceTransform.location );

	for ( GXUShort i = 1; i < totalBones; i++ )
	{
		GXMat4 inverseParentTransform;
		inverseParentTransform.Inverse ( bones[ bones[ i ].parentIndex ].transformWorld );

		GXMat4 referenceTransform;
		referenceTransform.Multiply ( inverseParentTransform, bones[ i ].transformWorld );

		bones[ i ].referenceTransform.rotation.From ( referenceTransform );
		bones[ i ].referenceTransform.rotation.Normalize ();

		referenceTransform.GetW ( bones[ i ].referenceTransform.location );
	}
}

GXVoid GXSkeleton::CalculateBindTransform ()
{
	if ( !isValid ) return;

	for ( GXUInt i = 0; i < totalBones; i++ )
	{
		GXMat4 inverseBoneTransformWorld;
		inverseBoneTransformWorld.Inverse ( bones[ i ].transformWorld );

		bones[ i ].bindTransform.rotation.From ( inverseBoneTransformWorld );
		bones[ i ].bindTransform.rotation.Normalize ();

		inverseBoneTransformWorld.GetW ( bones[ i ].bindTransform.location );
	}
}

GXBool GXSkeleton::IsRootBone ( IGameNode* boneFather ) const
{
	if ( currentBoneIndex == 0 || !boneFather ) return GX_TRUE;

	for ( GXUShort i = 0; i < currentBoneIndex; i++ )
	{
		if ( bones[ i ].node == boneFather ) return GX_FALSE;
	}

	return GX_TRUE;
}

GXUShort GXSkeleton::FindBoneIndex ( IGameNode* bone ) const
{
	if ( currentBoneIndex == 0 || !bone ) return UNDEFINED_BONE_INDEX;

	for ( GXUShort i = 0; i < currentBoneIndex; i++ )
	{
		if ( bones[ i ].node == bone ) return i;
	}

	return UNDEFINED_BONE_INDEX;
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
		if ( iw[ i ].data[ 1 ] >= iw[ m ].data[ 1 ] ) continue;

		m = i;
	}

	iw[ m ].Init ( boneIndex, weight );
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
	{
		MessageBoxA ( 0, "Can't save skeletal mesh", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
	}
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

			stack->Check ();

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
	GXUInt numBones = skeleton->GetTotalBones ();

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
		parent[ i ] = (GXShort)skeleton->GetBoneParentIndex ( i );

	offset += numBones * sizeof ( GXShort );

	h->refPoseOffset = offset;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		memcpy ( data + offset, skeleton->GetBoneReferenceTransform ( i ), sizeof ( GXQuatLocJoint ) );
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
	skin = 0;
	totalBones = 0;
	bones = nullptr;
	game = nullptr;
	node = nullptr;

	isValid = GX_TRUE;

	Init ( selection );

	if ( isValid )
	{
		Save ( fileName, startFrame, lastFrame );
		node->ReleaseIGameObject ();
		game->ReleaseIGame ();
		MessageBoxA ( 0, "Animation is saved", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
	}
	else
	{
		MessageBoxA ( 0, "Can't save animation", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
	}
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
	coordSystem.xAxis = 1;
	coordSystem.yAxis = 2;
	coordSystem.zAxis = 4;
	coordSystem.uAxis = 1;
	coordSystem.vAxis = 0;
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
	GXUInt size = sizeof ( GXNativeAnimationHeader ) + totalBones * ( GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) ) + numFrames * totalBones * sizeof ( GXQuatLocJoint );

	GXUByte* data = (GXUByte*)malloc ( size );

	GXNativeAnimationHeader* h = (GXNativeAnimationHeader*)data;

	h->fps = (GXFloat)GetFrameRate ();
	h->numFrames = numFrames;
	h->numBones = totalBones;

	GXUInt offset = sizeof ( GXNativeAnimationHeader );
	h->boneNamesOffset = offset;

	for ( GXUShort i = 0; i < totalBones; i++ )
	{
		memcpy ( data + offset, bones[ i ].name, GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) );
		offset += GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	}

	h->keysOffset = offset;

	for ( GXUInt frame = startFrame; frame <= lastFrame; frame++ )
	{
		ExtractFrame ( frame );

		for ( GXUShort i = 0; i < totalBones; i++ )
		{
			memcpy ( data + offset, &bones[ i ].referenceTransform, sizeof ( GXQuatLocJoint ) );
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

	totalBones = (GXUShort)skin->GetTotalSkinBoneCount ();
	bones = (GXBone*)malloc ( totalBones * sizeof ( GXBone ) );

	GXMat4 matrix;
	matrix.Identity ();

	for ( GXUShort i = 0; i < totalBones; i++ )
		bones[ i ].Init ( i, skin->GetIGameBone ( i, false ), matrix );
}

GXVoid GXAnimationExporter::ExtractFrame ( GXUInt frame )
{
	GXUInt time = frame * GetTicksPerFrame ();

	for ( GXUShort i = (GXUShort)0; i < totalBones; i++ )
	{
		GMatrix matrix = bones[ i ].node->GetWorldTM ( (TimeValue)time );
		GXMat4 transformWorld;
		memcpy ( &transformWorld, &matrix, sizeof ( GXMat4 ) );

		IGameNode* parent = bones[ i ].node->GetNodeParent ();

		if ( !parent )
		{
			bones[ i ].referenceTransform.rotation.From ( transformWorld );
			bones[ i ].referenceTransform.rotation.Normalize ();

			transformWorld.GetW ( bones[ i ].referenceTransform.location );

			continue;
		}

		matrix = parent->GetWorldTM ( (TimeValue)time );
		GXMat4 parentTransformWorld;
		memcpy ( &parentTransformWorld, &matrix, sizeof ( GXMat4 ) );

		GXMat4 inverseParentTransformWorld;
		inverseParentTransformWorld.Inverse ( parentTransformWorld );

		GXMat4 referenceTransform;
		referenceTransform.Multiply ( inverseParentTransformWorld, transformWorld );

		bones[ i ].referenceTransform.rotation.From ( referenceTransform );
		bones[ i ].referenceTransform.rotation.Normalize ();

		referenceTransform.GetW ( bones[ i ].referenceTransform.location );
	}
}
