// version 1.5

#include <GXEngine/GXAnimationSolverPlayer.h>
#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <new>
#include <GXCommon/GXRestoreWarnings.h>


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
	cacheFriendlyNodes = (GXBoneFinderNode*)malloc ( sizeof ( GXBoneFinderNode ) * animInfo.totalBones );
	for ( GXUShort i = 0; i < animInfo.totalBones; i++ )
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
		return GX_UNKNOWN_BONE_INDEX;

	return node->boneIndex;
}

//--------------------------------------------------------------------------------------------------

GXAnimationSolverPlayer::GXAnimationSolverPlayer ( GXUShort solverID ) :
	GXAnimationSolver ( solverID )
{
	animPos = DEFAULT_ANIMATION_POSITION;
	frameInterval = DEFAULT_FRAME_INTERVAL;
	frameInterpolationFactor = DEFAULT_FRAME_INTERPOLATION_FACTOR;
	finder = nullptr;

	DisableNormalization ();
	SetAnimationMultiplier ( DEFAULT_MULTIPLIER );
}

GXAnimationSolverPlayer::~GXAnimationSolverPlayer ()
{
	GXSafeDelete ( finder );
}

GXBool GXAnimationSolverPlayer::GetBoneJoint ( GXBoneJoint &joint, const GXUTF8* boneName )
{
	if ( !finder || !animationInfo ) return GX_FALSE;

	GXUShort boneIndex = finder->FindBoneIndex ( boneName );

	if ( boneIndex == GX_UNKNOWN_BONE_INDEX ) return GX_FALSE;

	GXUInt doneFrame = (GXUInt)( animPos * animationInfo->totalFrames );

	if ( doneFrame >= animationInfo->totalFrames )
		doneFrame = animationInfo->totalFrames - 1;

	GXUInt nextFrame = doneFrame + 1;

	if ( nextFrame >= animationInfo->totalFrames )
		nextFrame = 0;

	const GXBoneJoint* doneJoint = animationInfo->keys + doneFrame * animationInfo->totalBones + boneIndex;
	const GXBoneJoint* nextJoint = animationInfo->keys + nextFrame * animationInfo->totalBones + boneIndex;

	GXFloat alpha = frameInterpolationFactor * animationInfo->fps;

	joint.location.LinearInterpolation ( doneJoint->location, nextJoint->location, alpha );
	joint.rotation.SphericalLinearInterpolation ( doneJoint->rotation, nextJoint->rotation, alpha );

	if ( isNormalize )
		joint.rotation.Normalize ();

	return GX_TRUE;
}

GXVoid GXAnimationSolverPlayer::Update ( GXFloat delta )
{
	GXFloat alpha = delta * speed;

	animPos += alpha * deltaToPartFactor;
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

GXVoid GXAnimationSolverPlayer::SetAnimationSequence ( const GXAnimationInfo* animData )
{
	GXSafeDelete ( finder );
	finder = new GXBoneFinder ( *animData );

	animationInfo = animData;

	frameInterval = 1.0f / animData->fps;
	frameInterpolationFactor = 0.0f;
	GXFloat totalTime = animData->totalFrames * frameInterval;
	deltaToPartFactor = 1.0f / totalTime;
}

GXVoid GXAnimationSolverPlayer::SetAnimationMultiplier ( GXFloat multiplier )
{
	speed = multiplier;
}

GXVoid GXAnimationSolverPlayer::EnableNormalization ()
{
	isNormalize = GX_TRUE;
}

GXVoid GXAnimationSolverPlayer::DisableNormalization ()
{
	isNormalize = GX_FALSE;
}
