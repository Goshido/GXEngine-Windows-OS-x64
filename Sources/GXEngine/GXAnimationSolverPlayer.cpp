// version 1.11

#include <GXEngine/GXAnimationSolverPlayer.h>
#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>

GX_DISABLE_COMMON_WARNINGS

#include <new>

GX_RESTORE_WARNING_STATE


#define DEFAULT_MULTIPLIER                      1.0f
#define DEFAULT_ANIMATION_POSITION              0.0f
#define DEFAULT_FRAME_INTERVAL                  0.016f
#define DEFAULT_FRAME_INTERPOLATION_FACTOR      0.0f

#define INVALID_BONE_INDEX                      0xFFFFu


class GXBoneFinderNode final : public GXAVLTreeNode
{
    public:
        const GXUTF8*       _boneName;
        GXUShort            _boneIndex;

    public:
        GXBoneFinderNode ();
        explicit GXBoneFinderNode ( const GXUTF8* boneName, GXUShort boneIndex );
        ~GXBoneFinderNode () override;

        static eGXCompareResult GXCALL Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );

    private:
        GXBoneFinderNode ( const GXBoneFinderNode &other ) = delete;
        GXBoneFinderNode& operator = ( const GXBoneFinderNode &other ) = delete;
};

GXBoneFinderNode::GXBoneFinderNode ():
    _boneName ( nullptr ),
    _boneIndex ( static_cast<GXUShort> ( INVALID_BONE_INDEX ) )
{
    // NOTHING
}

GXBoneFinderNode::GXBoneFinderNode ( const GXUTF8* boneName, GXUShort boneIndex ):
    _boneName ( boneName ),
    _boneIndex ( boneIndex )
{
    // NOTHING
}

GXBoneFinderNode::~GXBoneFinderNode ()
{
    // NOTHING
}

eGXCompareResult GXCALL GXBoneFinderNode::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
    const GXBoneFinderNode& aNode = static_cast<const GXBoneFinderNode&> ( a );
    const GXBoneFinderNode& bNode = static_cast<const GXBoneFinderNode&> ( b );
    return static_cast<eGXCompareResult> ( GXUTF8cmp ( aNode._boneName, bNode._boneName ) );
}

//--------------------------------------------------------------------------------------------------

class GXBoneFinder final : public GXMemoryInspector, public GXAVLTree
{
    private:
        GXBoneFinderNode*       _cacheFriendlyNodes;

    public:
        explicit GXBoneFinder ( const GXAnimationInfo &animInfo );
        ~GXBoneFinder () override;

        GXUShort FindBoneIndex ( const GXUTF8* boneName ) const;

    private:
        GXBoneFinder ( const GXBoneFinder &other ) = delete;
        GXBoneFinder& operator = ( const GXBoneFinder &other ) = delete;
};

GXBoneFinder::GXBoneFinder ( const GXAnimationInfo &animInfo )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXBoneFinder" )
    GXAVLTree ( &GXBoneFinderNode::Compare, GX_FALSE )
{
    _cacheFriendlyNodes = static_cast<GXBoneFinderNode*> ( Malloc ( sizeof ( GXBoneFinderNode ) * animInfo._totalBones ) );

    for ( GXUShort i = 0u; i < animInfo._totalBones; ++i )
    {
        ::new ( _cacheFriendlyNodes + i ) GXBoneFinderNode ( animInfo._boneNames + i * GX_BONE_NAME_SIZE, i );
        Add ( *( _cacheFriendlyNodes + i ) );
    }
}

GXBoneFinder::~GXBoneFinder ()
{
    Free ( _cacheFriendlyNodes );
}

GXUShort GXBoneFinder::FindBoneIndex ( const GXUTF8* boneName ) const
{
    GXBoneFinderNode finderNode;
    finderNode._boneName = boneName;
    const GXBoneFinderNode* node = static_cast<const GXBoneFinderNode*> ( Find ( finderNode ) );

    if ( !node )
        return GX_UNKNOWN_BONE_INDEX;

    return node->_boneIndex;
}

//--------------------------------------------------------------------------------------------------

GXAnimationSolverPlayer::GXAnimationSolverPlayer ( GXUShort solverID ):
    GXAnimationSolver ( solverID ),
    _finder ( nullptr ),
    _animPos ( DEFAULT_ANIMATION_POSITION ),
    _animationInfo ( nullptr ),
    _frameInterval ( DEFAULT_FRAME_INTERVAL ),
    _frameInterpolationFactor ( DEFAULT_FRAME_INTERPOLATION_FACTOR )
{
    DisableNormalization ();
    SetAnimationMultiplier ( DEFAULT_MULTIPLIER );
}

GXAnimationSolverPlayer::~GXAnimationSolverPlayer ()
{
    GXSafeDelete ( _finder );
}

GXBool GXAnimationSolverPlayer::GetBoneJoint ( GXBoneJoint &joint, const GXUTF8* boneName )
{
    if ( !_finder || !_animationInfo ) return GX_FALSE;

    GXUShort boneIndex = _finder->FindBoneIndex ( boneName );

    if ( boneIndex == GX_UNKNOWN_BONE_INDEX ) return GX_FALSE;

    GXUInt doneFrame = static_cast<GXUInt> ( _animPos * _animationInfo->_totalFrames );

    if ( doneFrame >= _animationInfo->_totalFrames )
        doneFrame = _animationInfo->_totalFrames - 1;

    GXUInt nextFrame = doneFrame + 1;

    if ( nextFrame >= _animationInfo->_totalFrames )
        nextFrame = 0;

    const GXBoneJoint* doneJoint = _animationInfo->_keys + doneFrame * _animationInfo->_totalBones + boneIndex;
    const GXBoneJoint* nextJoint = _animationInfo->_keys + nextFrame * _animationInfo->_totalBones + boneIndex;

    GXFloat alpha = _frameInterpolationFactor * _animationInfo->_fps;

    joint._location.LinearInterpolation ( doneJoint->_location, nextJoint->_location, alpha );
    joint._rotation.SphericalLinearInterpolation ( doneJoint->_rotation, nextJoint->_rotation, alpha );

    if ( _isNormalize )
        joint._rotation.Normalize ();

    return GX_TRUE;
}

GXVoid GXAnimationSolverPlayer::Update ( GXFloat delta )
{
    GXFloat alpha = delta * _speed;

    _animPos += alpha * _deltaToPartFactor;
    _frameInterpolationFactor += alpha;

    if ( _animPos > 1.0f )
    {
        while ( _animPos > 1.0f )
        {
            _animPos -= 1.0f;
        }
    }
    else if ( _animPos < 0.0f )
    {
        while ( _animPos < 0.0f )
        {
            _animPos += 1.0f;
        }
    }

    if ( _frameInterpolationFactor > _frameInterval )
    {
        while ( _frameInterpolationFactor > _frameInterval )
        {
            _frameInterpolationFactor -= _frameInterval;
        }

        return;
    }

    if ( _frameInterpolationFactor >= 0.0f ) return;

    while ( _frameInterpolationFactor < 0.0f )
    {
        _frameInterpolationFactor += _frameInterval;
    }
}

GXVoid GXAnimationSolverPlayer::SetAnimationSequence ( const GXAnimationInfo* animData )
{
    GXSafeDelete ( _finder );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXBoneFinder" );
    _finder = new GXBoneFinder ( *animData );

    _animationInfo = animData;

    _frameInterval = 1.0f / animData->_fps;
    _frameInterpolationFactor = 0.0f;
    GXFloat totalTime = animData->_totalFrames * _frameInterval;
    _deltaToPartFactor = 1.0f / totalTime;
}

GXVoid GXAnimationSolverPlayer::SetAnimationMultiplier ( GXFloat multiplier )
{
    _speed = multiplier;
}

GXVoid GXAnimationSolverPlayer::EnableNormalization ()
{
    _isNormalize = GX_TRUE;
}

GXVoid GXAnimationSolverPlayer::DisableNormalization ()
{
    _isNormalize = GX_FALSE;
}
