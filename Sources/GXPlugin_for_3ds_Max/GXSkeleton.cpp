//version 1.1

#include <GXPlugin_for_3ds_Max/GXSkeleton.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGameModifier.h>
#include <GXCommon/GXRestoreWarnings.h>


#define UNKNOWN_BONE_NUMBER				0xFFFF


GXSkeleton::GXSkeleton ()
{
	isValid = GX_FALSE;

	totalBones = (GXUShort)0;
	bones = nullptr;
	rootBone = nullptr;
}

GXSkeleton::GXSkeleton ( IGameObject &skinMesh )
{
	isValid = GX_TRUE;

	totalBones = (GXUShort)0;
	bones = nullptr;
	rootBone = nullptr;

	From ( skinMesh );
}

GXSkeleton::~GXSkeleton ()
{
	GXSafeFree ( bones );
}

GXVoid GXSkeleton::From ( IGameObject &skinMesh )
{
	isValid = GX_TRUE;

	if ( !ExtractBones ( skinMesh ) )
	{
		isValid = GX_FALSE;
		return;
	}

	LinkBones ();
	RebuildBoneArray ();
}

GXVoid GXSkeleton::Reset ()
{
	isValid = GX_TRUE;

	totalBones = (GXUShort)0;
	GXSafeFree ( bones );
	rootBone = nullptr;
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
	if ( !isValid ) return GX_UNKNOWN_BONE_INDEX;

	return FindBoneIndex ( bone );
}

GXUShort GXSkeleton::GetParentBoneIndex ( GXUShort boneIndex ) const
{
	if ( !isValid ) return GX_UNKNOWN_BONE_INDEX;

	if ( bones[ boneIndex ].isRootBone ) return GX_ROOT_BONE_PARENT_INDEX;

	return bones[ boneIndex ].parentIndex;
}


GXVoid GXSkeleton::CalculateReferenceTransform ()
{
	if ( !isValid ) return;

	GXBone& baseBone = bones[ 0 ];

	baseBone.referenceTransform.rotation.From ( baseBone.transformWorld );
	baseBone.transformWorld.GetW ( baseBone.referenceTransform.location );
	baseBone.referenceTransform2 = baseBone.transformWorld;

	for ( GXUShort i = 1; i < totalBones; i++ )
	{
		//Note: X * P = G
		//where:
		//		X - bone reference transform
		//		P - parent bone world transform,
		//		G - bone world transform
		//We know P and G, so:
		//		X * P * P^(-1) = G * P^(-1)
		//=>	X = G * P^(-1)

		GXBone& bone = bones[ i ];
		const GXBone& parentBone = bones[ bones[ i ].parentIndex ];

		GXMat4 inverseParentTransform;
		inverseParentTransform.Inverse ( parentBone.transformWorld );

		GXMat4 referenceTransform;
		referenceTransform.Multiply ( bone.transformWorld, inverseParentTransform );

		bone.referenceTransform.rotation.From ( referenceTransform );
		bone.referenceTransform.rotation.Normalize ();

		referenceTransform.GetW ( bone.referenceTransform.location );

		bone.referenceTransform2 = referenceTransform;
	}
}

GXVoid GXSkeleton::CalculateInverseBindTransform ()
{
	if ( !isValid ) return;

	for ( GXUInt i = 0; i < totalBones; i++ )
	{
		GXBone& bone = bones[ i ];

		GXMat4 inverseBoneTransformWorld;
		inverseBoneTransformWorld.Inverse ( bone.transformWorld );

		bone.inverseBindTransform.rotation.From ( inverseBoneTransformWorld );
		bone.inverseBindTransform.rotation.Normalize ();

		inverseBoneTransformWorld.GetW ( bone.inverseBindTransform.location );

		bone.inverseBindTransform2 = inverseBoneTransformWorld;
	}
}

GXVoid GXSkeleton::CalculatePoseTransform ( GXUInt frame )
{
	if ( !isValid ) return;

	TimeValue time = (TimeValue)( frame * GetTicksPerFrame () );

	GXBone& baseBone = bones[ 0 ];

	GMatrix matrix = baseBone.node->GetObjectTM ( time );
	GXMat4 boneTransform;
	memcpy ( &boneTransform, &matrix, sizeof ( GXMat4 ) );

	baseBone.poseTransform.rotation.From ( boneTransform );
	boneTransform.GetW ( baseBone.poseTransform.location );
	baseBone.poseTransform2 = boneTransform;

	for ( GXUShort i = (GXUShort)1; i < totalBones; i++ )
	{
		//Note: X * P = G
		//where:
		//		X - bone reference transform
		//		P - parent bone world transform,
		//		G - bone world transform
		//We know P and G, so:
		//		X * P * P^(-1) = G * P^(-1)
		//=>	X = G * P^(-1)

		GXBone& bone = bones[ i ];
		const GXBone& parentBone = bones[ bones[ i ].parentIndex ];

		matrix = bone.node->GetObjectTM ( time );
		memcpy ( &boneTransform, &matrix, sizeof ( GXMat4 ) );

		matrix = parentBone.node->GetObjectTM ( (TimeValue)time );
		GXMat4 parentBoneTransform;
		memcpy ( &parentBoneTransform, &matrix, sizeof ( GXMat4 ) );

		GXMat4 inverseParentBoneTransform;
		inverseParentBoneTransform.Inverse ( parentBoneTransform );

		GXMat4 poseTransform;
		poseTransform.Multiply ( boneTransform, inverseParentBoneTransform );

		bone.poseTransform.rotation.From ( poseTransform );
		bone.poseTransform.rotation.Normalize ();

		poseTransform.GetW ( bone.poseTransform.location );

		bone.poseTransform2 = poseTransform;
	}
}

const GXQuatLocJoint& GXSkeleton::GetBoneReferenceTransform ( GXUShort boneIndex ) const
{
	return bones[ boneIndex ].referenceTransform;
}

const GXMat4& GXSkeleton::GetBoneReferenceTransform2 ( GXUShort boneIndex ) const
{
	return bones[ boneIndex ].referenceTransform2;
}

const GXQuatLocJoint& GXSkeleton::GetBoneInverseBindTransform ( GXUShort boneIndex ) const
{
	return bones[ boneIndex ].inverseBindTransform;
}

const GXMat4& GXSkeleton::GetBoneInverseBindTransform2 ( GXUShort boneIndex ) const
{
	return bones[ boneIndex ].inverseBindTransform2;
}

const GXQuatLocJoint& GXSkeleton::GetBonePoseTransform ( GXUShort boneIndex ) const
{
	return bones[ boneIndex ].poseTransform;
}

const GXMat4& GXSkeleton::GetBonePoseTransform2 ( GXUShort boneIndex ) const
{
	return bones[ boneIndex ].poseTransform2;
}

GXUShort GXSkeleton::GetTotalBones () const
{
	if ( !isValid ) return UNKNOWN_BONE_NUMBER;

	return totalBones;
}

GXBool GXSkeleton::ExtractBones ( IGameObject &skinMesh )
{
	IGameSkin* skin = skinMesh.GetIGameSkin ();

	if ( !skin || skin->GetSkinType () != IGameSkin::IGAME_SKIN ) return GX_FALSE;

	totalBones = (GXUShort)skin->GetTotalBoneCount ();
	bones = (GXBone*)malloc ( totalBones * sizeof ( GXBone ) );

	for ( GXUShort i = 0; i < totalBones; i++ )
	{
		IGameNode* nativeBone = skin->GetIGameBone ( i, true );

		GMatrix matrix;
		skin->GetInitBoneTM ( nativeBone, matrix );

		GXMat4 transformWorld;
		memcpy ( &transformWorld, &matrix, sizeof ( GXMat4 ) );

		bones[ i ].Init ( i, nativeBone, transformWorld );
	}

	return GX_TRUE;
}

GXVoid GXSkeleton::LinkBones ()
{
	if ( !isValid ) return;

	for ( GXUShort i = 0; i < totalBones; i++ )
	{
		IGameNode* father = bones[ i ].node->GetNodeParent ();

		bones[ i ].ownIndex = i;

		if ( IsRootBone ( father ) )
		{
			bones[ i ].isRootBone = GX_TRUE;
			bones[ i ].parentIndex = GX_ROOT_BONE_PARENT_INDEX;
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

	bones[ 0 ] = *rootBone;
	bones[ 0 ].ownIndex = (GXUShort)0;

	GXUShort currentBoneIndex = (GXUShort)1;
	RegroupBones ( bones[ 0 ].son, currentBoneIndex, bones[ 0 ].ownIndex );

	free ( oldBones );
}

GXVoid GXSkeleton::RegroupBones ( GXBone* bone, GXUShort &currentBoneIndex, GXUShort parentBoneIndex )
{
	if ( !bone ) return;

	GXUShort thisBoneIndex = currentBoneIndex;

	bones[ thisBoneIndex ] = *bone;
	bones[ thisBoneIndex ].ownIndex = thisBoneIndex;
	bones[ thisBoneIndex ].parentIndex = parentBoneIndex;

	currentBoneIndex++;

	GXBone* brother = bone->brother;

	while ( brother )
	{
		GXUShort brotherBoneIndex = currentBoneIndex;

		bones[ brotherBoneIndex ] = *brother;
		bones[ brotherBoneIndex ].ownIndex = brotherBoneIndex;
		bones[ brotherBoneIndex ].parentIndex = parentBoneIndex;

		currentBoneIndex++;

		RegroupBones ( brother->son, currentBoneIndex, brotherBoneIndex );

		brother = brother->brother;
	}

	RegroupBones ( bone->son, currentBoneIndex, thisBoneIndex );
}

GXBool GXSkeleton::IsRootBone ( IGameNode* boneFather ) const
{
	if ( !boneFather ) return GX_TRUE;

	for ( GXUShort i = 0; i < totalBones; i++ )
	{
		if ( bones[ i ].node == boneFather ) return GX_FALSE;
	}

	return GX_TRUE;
}

GXUShort GXSkeleton::FindBoneIndex ( IGameNode* bone ) const
{
	if ( totalBones == 0 || !bone ) return GX_UNKNOWN_BONE_INDEX;

	for ( GXUShort i = 0; i < totalBones; i++ )
	{
		if ( bones[ i ].node == bone ) return i;
	}

	return GX_UNKNOWN_BONE_INDEX;
}
