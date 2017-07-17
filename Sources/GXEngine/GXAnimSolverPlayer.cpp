//version 1.4

#include <GXEngine/GXAnimSolverPlayer.h>
#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXMemory.h>
#include <new>


#define DEFAULT_MULTIPLIER					1.0f
#define DEFAULT_ANIMATION_POSITION			0.0f
#define DEFAULT_FRAME_INTERVAL				0.016f
#define DEFAULT_FRAME_INTERPOLATION_FACTOR	0.0f

#define INVALID_BONE_INDEX					0xFFFF


class GXBoneFinderNode : public GXAVLTreeNode
{
	public:
		const GXUTF8*	boneName;
		GXUShort		boneIndex;

	public:
		GXBoneFinderNode ();
		explicit GXBoneFinderNode ( const GXUTF8* boneName, GXUShort boneIndex );
		~GXBoneFinderNode () override;

		static GXInt GXCALL Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );
};

GXBoneFinderNode::GXBoneFinderNode ()
{
	boneName = nullptr;
	boneIndex = INVALID_BONE_INDEX;
}

GXBoneFinderNode::GXBoneFinderNode ( const GXUTF8* boneName, GXUShort boneIndex )
{
	this->boneName = boneName;
	this->boneIndex = boneIndex;
}

GXBoneFinderNode::~GXBoneFinderNode ()
{
	// NOTHING
}

GXInt GXCALL GXBoneFinderNode::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
	GXBoneFinderNode& aNode = (GXBoneFinderNode&)a;
	GXBoneFinderNode& bNode = (GXBoneFinderNode&)b;
	return strcmp ( aNode.boneName, bNode.boneName );
}

//--------------------------------------------------------------------------------------------------

class GXBoneFinder : public GXAVLTree
{
	private:
		GXBoneFinderNode*	cacheFriendlyNodes;

	public:
		explicit GXBoneFinder ( const GXAnimationInfo &animInfo );
		~GXBoneFinder () override;

		GXUShort FindBoneIndex ( const GXUTF8* boneName ) const;
};

GXBoneFinder::GXBoneFinder ( const GXAnimationInfo &animInfo ) :
GXAVLTree ( &GXBoneFinderNode::Compare, GX_FALSE )
{
	cacheFriendlyNodes = (GXBoneFinderNode*)malloc ( sizeof ( GXBoneFinderNode ) * animInfo.numBones );
	for ( GXUShort i = 0; i < animInfo.numBones; i++ )
	{
		new ( cacheFriendlyNodes + i ) GXBoneFinderNode ( animInfo.boneNames + i * GX_BONE_NAME_SIZE, i );
		Add ( *( cacheFriendlyNodes + i ) );
	}
}

GXBoneFinder::~GXBoneFinder ()
{
	free ( cacheFriendlyNodes );
}

GXUShort GXBoneFinder::FindBoneIndex ( const GXUTF8* boneName ) const
{
	GXBoneFinderNode finderNode;
	finderNode.boneName = boneName;
	const GXBoneFinderNode* node = (const GXBoneFinderNode*)Find ( finderNode );

	if ( !node )
		return 0xFFFF;

	return node->boneIndex;
}

//--------------------------------------------------------------------------------------------------

GXAnimSolverPlayer::GXAnimSolverPlayer ( GXUShort solverID ):
GXAnimSolver ( solverID )
{
	animPos = DEFAULT_ANIMATION_POSITION;
	frameInterval = DEFAULT_FRAME_INTERVAL;
	frameInterpolationFactor = DEFAULT_FRAME_INTERPOLATION_FACTOR;
	finder = nullptr;
	
	DisableNormalization ();
	SetAnimationMultiplier ( DEFAULT_MULTIPLIER );
}

GXAnimSolverPlayer::~GXAnimSolverPlayer ()
{
	GXSafeDelete ( finder );
}

GXBool GXAnimSolverPlayer::GetBone ( const GXUTF8* boneName, GXQuat &rotation, GXVec3 &location )
{
	if ( !finder || !animData ) return GX_FALSE;

	GXUShort boneIndex = finder->FindBoneIndex ( boneName );

	if ( boneIndex == 0xFFFF ) return GX_FALSE;

	GXUInt doneFrame = (GXUInt)( animPos * animData->numFrames );
	if ( doneFrame >= animData->numFrames )
		doneFrame = animData->numFrames - 1;

	GXUInt nextFrame = doneFrame + 1;
	if ( nextFrame >= animData->numFrames )
		nextFrame = 0;

	const GXQuatLocJoint* doneJoint = animData->keys + doneFrame * animData->numBones + boneIndex;
	const GXQuatLocJoint* nextJoint = animData->keys + nextFrame * animData->numBones + boneIndex;

	GXFloat alpha = frameInterpolationFactor * animData->fps;

	GXLerpVec3 ( location, doneJoint->location, nextJoint->location, alpha );
	GXQuatSLerp ( rotation, doneJoint->rotation, nextJoint->rotation, alpha );

	if ( isNormalize )
		GXNormalizeQuat ( rotation );

	return GX_TRUE;
}

GXVoid GXAnimSolverPlayer::Update ( GXFloat delta )
{
	GXFloat alpha = delta * multiplier;

	animPos += alpha * delta2PartFactor;
	frameInterpolationFactor += alpha;

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

	if ( frameInterpolationFactor > frameInterval )
	{
		while ( frameInterpolationFactor > frameInterval )
			frameInterpolationFactor -= frameInterval;
	}
	else if ( frameInterpolationFactor < 0.0f )
	{
		while ( frameInterpolationFactor < 0.0f )
			frameInterpolationFactor += frameInterval;
	}
}

GXVoid GXAnimSolverPlayer::SetAnimationSequence ( const GXAnimationInfo* animData )
{
	GXSafeDelete ( finder );
	finder = new GXBoneFinder ( *animData );

	this->animData = animData; 

	frameInterval = 1.0f / animData->fps;
	frameInterpolationFactor = 0.0f;
	GXFloat totalTime = animData->numFrames * frameInterval;
	delta2PartFactor = 1.0f / totalTime;
}

GXVoid GXAnimSolverPlayer::SetAnimationMultiplier ( GXFloat multiplier )
{
	this->multiplier = multiplier;
}

GXVoid GXAnimSolverPlayer::EnableNormalization ()
{
	isNormalize = GX_TRUE;
}

GXVoid GXAnimSolverPlayer::DisableNormalization ()
{
	isNormalize = GX_FALSE;
}
