//version 1.4

#include <GXEngine/GXAnimSolverPlayer.h>
#include <GXCommon/GXAVLTree.h>
#include <new>


class GXBoneFinderNode : public GXAVLTreeNode
{
	public:
		const GXUTF8*	boneName;
		GXUShort		boneIndex;

	public:
		GXBoneFinderNode ( const GXUTF8* boneName, GXUShort boneIndex );
	
		virtual const GXVoid* GetKey ();

		static GXInt GXCALL Compare ( const GXVoid* a, const GXVoid* b );
};

GXBoneFinderNode::GXBoneFinderNode ( const GXUTF8* boneName, GXUShort boneIndex )
{
	this->boneName = boneName;
	this->boneIndex = boneIndex;
}

const GXVoid* GXBoneFinderNode::GetKey ()
{
	return boneName;
}

GXInt GXCALL GXBoneFinderNode::Compare ( const GXVoid* a, const GXVoid* b )
{
	return strcmp ( (const GXUTF8*)a, (const GXUTF8*)b );
}

//--------------------------------------------------------------------------------------------------

class GXBoneFinder : public GXAVLTree
{
	private:
		GXBoneFinderNode*	cacheFriendlyNodes;

	public:
		GXBoneFinder ( const GXAnimationInfoExt &animInfo );
		virtual ~GXBoneFinder ();

		GXUShort FindBoneIndex ( const GXUTF8* boneName );
};

GXBoneFinder::GXBoneFinder ( const GXAnimationInfoExt &animInfo ) :
GXAVLTree ( &GXBoneFinderNode::Compare, GX_FALSE )
{
	cacheFriendlyNodes = (GXBoneFinderNode*)malloc ( sizeof ( GXBoneFinderNode ) * animInfo.numBones );
	for ( GXUShort i = 0; i < animInfo.numBones; i++ )
	{
		new ( cacheFriendlyNodes + i ) GXBoneFinderNode ( animInfo.boneNames + i * GX_BONE_NAME_SIZE, i );
		Add ( cacheFriendlyNodes + i );
	}
}

GXBoneFinder::~GXBoneFinder ()
{
	free ( cacheFriendlyNodes );
}

GXUShort GXBoneFinder::FindBoneIndex ( const GXUTF8* boneName )
{
	const GXBoneFinderNode* node = (const GXBoneFinderNode*)FindByKey ( boneName );

	if ( !node )
		return 0xFFFF;

	return node->boneIndex;
}

//--------------------------------------------------------------------------------------------------

GXAnimSolverPlayer::GXAnimSolverPlayer ( GXUShort solverID ):
GXAnimSolver ( solverID )
{
	animPos = 0.0f;
	finder = 0;
}

GXAnimSolverPlayer::~GXAnimSolverPlayer ()
{
	GXSafeDelete ( finder );
}

GXVoid GXAnimSolverPlayer::GetBone ( const GXUTF8* boneName, const GXQuat** rot, const GXVec3** loc )
{
	if ( !finder || !animData )
	{
		*rot = 0;
		*loc = 0;
		return;
	}

	GXUShort boneIndex = finder->FindBoneIndex ( boneName );

	if ( boneIndex == 0xFFFF )
	{
		*rot = 0;
		*loc = 0;
		return;
	}

	GXUInt frame = (GXUInt)( animPos * animData->numFrames );
	if ( frame >= animData->numFrames )
		frame = animData->numFrames - 1;

	const GXQuatLocJoint* joint = animData->keys + frame * animData->numBones + boneIndex;
	
	*rot = &joint->rotation;
	*loc = &joint->location;
}

GXVoid GXAnimSolverPlayer::Update ( GXFloat delta )
{
	animPos += delta * multiplier * delta2PartFartor;

	if ( animPos > 1.0f )
	{
		while ( animPos > 1.0f )
			animPos -= 1.0f;
	}
	else if ( animPos < 0.0f )
	{
		while ( animPos < 0.0f )
			animPos += 1.0f;
	}
}

GXVoid GXAnimSolverPlayer::SetAnimationSequence ( const GXAnimationInfoExt* animData )
{
	GXSafeDelete ( finder );
	finder = new GXBoneFinder ( *animData );

	this->animData = animData; 
	GXFloat totalTime = (GXFloat)animData->numFrames / animData->fps;
	delta2PartFartor = 1.0f / totalTime;
}

GXVoid GXAnimSolverPlayer::SetAnimationMultiplier ( GXFloat multiplier )
{
	this->multiplier = multiplier;
}