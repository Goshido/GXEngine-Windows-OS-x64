//version 1.3

#include <GXEngine/GXSkeletalMeshQuat.h>
#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <new>


class GXBoneKeeperNode : public GXAVLTreeNode
{
	public:
		const GXUTF8*	boneName;
		GXUShort		boneIndex;

	public:
		GXBoneKeeperNode ( const GXUTF8* boneName, GXUShort boneIndex );
	
		const GXVoid* GetKey () const override;

		static GXInt GXCALL Compare ( const GXVoid* a, const GXVoid* b );
};

GXBoneKeeperNode::GXBoneKeeperNode ( const GXUTF8* boneName, GXUShort boneIndex )
{
	this->boneName = boneName;
	this->boneIndex = boneIndex;
}

const GXVoid* GXBoneKeeperNode::GetKey () const
{
	return boneName;
}

GXInt GXCALL GXBoneKeeperNode::Compare ( const GXVoid* a, const GXVoid* b )
{
	return GXUTF8cmp ( (const GXUTF8*)a, (const GXUTF8*)b );
}

//--------------------------------------------------------------------------------------------------

class GXBoneKeeper : public GXAVLTree
{
	private:
		GXBoneKeeperNode*		cacheFriendlyNodes;

	public:
		GXBoneKeeper ( const GXSkeletalMeshInfoExt &skmInfo );
		~GXBoneKeeper () override;

		GXUShort FindBoneIndex ( const GXUTF8* boneName ) const;
};

GXBoneKeeper::GXBoneKeeper ( const GXSkeletalMeshInfoExt &skmInfo ) :
GXAVLTree ( &GXBoneKeeperNode::Compare, GX_FALSE )
{
	cacheFriendlyNodes = (GXBoneKeeperNode*)malloc ( sizeof ( GXBoneKeeperNode ) * skmInfo.numBones );
	for ( GXUShort i = 0; i < skmInfo.numBones; i++ )
	{
		new ( cacheFriendlyNodes + i ) GXBoneKeeperNode ( skmInfo.boneNames + i * GX_BONE_NAME_SIZE, i );
		Add ( cacheFriendlyNodes + i );
	}
}

GXBoneKeeper::~GXBoneKeeper ()
{
	free ( cacheFriendlyNodes );
}

GXUShort GXBoneKeeper::FindBoneIndex ( const GXChar* boneName ) const
{
	const GXBoneKeeperNode* node = (const GXBoneKeeperNode*)FindByKey ( boneName );

	if ( !node )
		return 0xFFFF;

	return node->boneIndex;
}

//-------------------------------------------------------------------------------------------------

GXSkeletalMeshQuat::GXSkeletalMeshQuat ()
{
	keeper = nullptr;
	animSolver = nullptr;
	memset ( &skmInfo, 0, sizeof ( GXSkeletalMeshInfoExt ) );
}

GXSkeletalMeshQuat::~GXSkeletalMeshQuat ()
{
	GXSafeDelete ( keeper );
}

GXVoid GXSkeletalMeshQuat::GetBoneGlobalLocation ( GXVec3 &out, const GXUTF8* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	if ( i == 0xFFFF ) return;

	GXVec4 ans;
	GXVec4 buf ( globalPose[ i ].location, 1.0f );

	GXMulVec4Mat4 ( ans, buf, mod_mat );

	out = GXCreateVec3 ( ans.x, ans.y, ans.z );
}

GXVoid GXSkeletalMeshQuat::GetBoneGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXUTF8* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	if ( i == 0xFFFF ) return;

	GXVec4 ans;
	GXVec4 buf ( globalPose[ i ].location, 1.0f );
	GXMulVec4Mat4 ( ans, buf, mod_mat );
	loc = GXCreateVec3 ( ans.x, ans.y, ans.z );

	GXQuat bufRot = GXCreateQuat ( mod_mat );
	GXMulQuatQuat ( rot, globalPose[ i ].rotation, bufRot );
}

GXVoid GXSkeletalMeshQuat::GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXUTF8* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	if ( i == 0xFFFF ) return;

	GXVec3 ofst = GXCreateVec3 ( offset.y, offset.x, offset.z );

	GXVec3 buf;
	GXVec3 buf01;

	GXQuatTransform ( buf01, globalPose[ i ].rotation, ofst );

	GXSumVec3Vec3 ( buf, globalPose[ i ].location, buf01 );

	GXVec4 ans;
	GXVec4 buf02 ( buf, 1.0f );
	GXMulVec4Mat4 ( ans, buf02, mod_mat );

	out = GXCreateVec3 ( ans.x, ans.y, ans.z );
}

GXVoid GXSkeletalMeshQuat::GetAttacmentGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXVec3 &offset, const GXUTF8* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	if ( i == 0xFFFF ) return;

	GXVec3 ofst = GXCreateVec3 ( offset.y, offset.x, offset.z );

	GXVec3 buf;
	GXVec3 buf01;

	GXQuatTransform ( buf01, globalPose[ i ].rotation, ofst );

	GXSumVec3Vec3 ( buf, globalPose[ i ].location, buf01 );

	GXVec4 ans;
	GXVec4 buf02 ( buf, 1.0f );
	GXMulVec4Mat4 ( ans, buf02, mod_mat );

	loc = GXCreateVec3 ( ans.x, ans.y, ans.z );

	GXQuat bufRot = GXCreateQuat ( mod_mat );
	GXMulQuatQuat ( rot, globalPose[ i ].rotation, bufRot );
}

GXVoid GXSkeletalMeshQuat::SetAnimSolver ( GXAnimSolver* animSolver )
{
	this->animSolver = animSolver;
}

GXVoid GXSkeletalMeshQuat::Update ( GXFloat deltaTime )
{
	if ( animSolver )
		animSolver->Update ( deltaTime );
}

GXVoid GXSkeletalMeshQuat::InitReferencePose ()
{
	keeper = new GXBoneKeeper ( skmInfo );
}

GXVoid GXSkeletalMeshQuat::InitPose ()
{
	memcpy ( localPose, skmInfo.refPose, sizeof ( GXQuatLocJoint ) * skmInfo.numBones );
	
	if ( animSolver )
	{
		for ( GXUInt i = 0; i < skmInfo.numBones; i++ )
		{
			const GXQuat* rot = nullptr;
			const GXVec3* loc = nullptr;

			animSolver->GetBone ( skmInfo.boneNames + i * GX_BONE_NAME_SIZE, &rot, &loc );

			if ( rot )
			{
				localPose[ i ].rotation = *rot;
				localPose[ i ].location = *loc;
			}
		}
	}

	//Расчёт итоговой трансформации корневой кости. Корневая кость - особый случай (у неё нет родителя).

	globalPose[ 0 ].rotation = localPose[ 0 ].rotation;
	globalPose[ 0 ].location = localPose[ 0 ].location;

	GXMulQuatQuat ( vertexTransform[ 0 ].rotation, skmInfo.bindTransform[ 0 ].rotation, globalPose[ 0 ].rotation );
	GXVec3 vTemp2;
	GXQuatTransform ( vTemp2, vertexTransform[ 0 ].rotation, skmInfo.bindTransform[ 0 ].location );
	GXSumVec3Vec3 ( vertexTransform[ 0 ].location, vTemp2, globalPose[ 0 ].location );

	//Расчёт всех оставшихся костей.

	for ( GXUInt i = 1; i < skmInfo.numBones; i++ )
	{
		GXMulQuatQuat ( globalPose[ i ].rotation, localPose[ i ].rotation, globalPose[ skmInfo.parentIndex[ i ] ].rotation );
		GXVec3 vTemp;
		GXQuatTransform ( vTemp, globalPose[ skmInfo.parentIndex[ i ] ].rotation, localPose[ i ].location );
		GXSumVec3Vec3 ( globalPose[ i ].location, vTemp, globalPose[ skmInfo.parentIndex[ i ] ].location );

		GXMulQuatQuat ( vertexTransform[ i ].rotation, skmInfo.bindTransform[ i ].rotation, globalPose[ i ].rotation );
		GXVec3 vTemp2;
		GXQuatTransform ( vTemp2, vertexTransform[ i ].rotation, skmInfo.bindTransform[ i ].location );
		GXSumVec3Vec3 ( vertexTransform[ i ].location, vTemp2, globalPose[ i ].location );
	}
}
