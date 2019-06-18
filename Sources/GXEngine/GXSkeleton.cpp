// version 1.6

#include <GXEngine/GXSkeleton.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


GXSkeleton::GXSkeleton ():
    _totalBones ( 0u ),
    _boneNames ( nullptr ),
    _parentBoneIndices ( nullptr ),
    _tempPoseLocal ( nullptr ),
    _tempPoseGlobal ( nullptr ),
    _referencePose ( nullptr ),
    _inverseBindTransform ( nullptr ),
    _skinTransform ( nullptr )
{
    // NOTHING
}

GXSkeleton::~GXSkeleton ()
{
    GXSafeFree ( _boneNames );
    GXSafeFree ( _parentBoneIndices );

    GXSafeFree ( _tempPoseLocal );
    GXSafeFree ( _tempPoseGlobal );
    GXSafeFree ( _referencePose );
    GXSafeFree ( _inverseBindTransform );
    GXSafeFree ( _skinTransform );
}

GXVoid GXSkeleton::LoadFromSkm ( const GXWChar* fileName )
{
    GXSkeletalMeshData skeletalMeshData;
    GXLoadNativeSkeletalMesh ( skeletalMeshData, fileName );

    _totalBones = skeletalMeshData.totalBones;

    GXUPointer size = _totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
    _boneNames = static_cast<GXUTF8*> ( malloc ( size ) );
    memcpy ( _boneNames, skeletalMeshData.boneNames, size );

    size = _totalBones * sizeof ( GXShort );
    _parentBoneIndices = static_cast<GXShort*> ( malloc ( size ) );
    memcpy ( _parentBoneIndices, skeletalMeshData.parentBoneIndices, size );

    size = _totalBones * sizeof ( GXBoneJoint );
    _referencePose = static_cast<GXBoneJoint*> ( malloc ( size ) );
    memcpy ( _referencePose, skeletalMeshData.referencePose, size );

    _inverseBindTransform = static_cast<GXBoneJoint*> ( malloc ( size ) );
    memcpy ( _inverseBindTransform, skeletalMeshData.inverseBindTransform, size );

    _tempPoseLocal = static_cast<GXBoneJoint*> ( malloc ( size ) );
    memcpy ( _tempPoseLocal, _referencePose, size );

    _tempPoseGlobal = static_cast<GXBoneJoint*> ( malloc ( size ) );
    _skinTransform = static_cast<GXBoneJoint*> ( malloc ( size ) );

    CalculatePose ();

    skeletalMeshData.Cleanup ();
}

GXVoid GXSkeleton::UpdatePose ( GXAnimationSolver &solver, GXFloat /*deltaTime*/ )
{
    GXUInt boneNameOffset = 0u;

    for ( GXUShort i = 0u; i < _totalBones; ++i )
    {
        GXBoneJoint joint;

        if ( solver.GetBoneJoint ( joint, _boneNames + boneNameOffset ) )
            _tempPoseLocal[ i ] = joint;
        else
            _tempPoseLocal[ i ] = _referencePose[ i ];

        boneNameOffset += GX_BONE_NAME_SIZE;
    }

    CalculatePose ();
}

const GXBoneJoint* GXSkeleton::GetSkinTransform () const
{
    return _skinTransform;
}

const GXBoneJoint& GXSkeleton::GetBoneTransformWorld ( GXUShort boneIndex ) const
{
    return _tempPoseGlobal[ boneIndex ];
}

const GXBoneJoint& GXSkeleton::GetParentBoneTransformWorld ( GXUShort boneIndex ) const
{
    return _tempPoseGlobal[ _parentBoneIndices[ boneIndex ] ];
}

GXUShort GXSkeleton::GetTotalBones () const
{
    return _totalBones;
}

GXVoid GXSkeleton::CalculatePose ()
{
    // Note: Quaternion mathematics simular to column-major notation matrix mathematics.
    // So we need to do multiplication in reverse order to calculate skin transform.

    _tempPoseGlobal[ 0u ] = _tempPoseLocal[ 0u ];
    _skinTransform[ 0u ].rotation.Multiply ( _tempPoseGlobal[ 0u ].rotation, _inverseBindTransform[ 0u ].rotation );
    GXVec3 buffer;
    _tempPoseGlobal[ 0u ].rotation.TransformFast ( buffer, _inverseBindTransform[ 0u ].location );
    _skinTransform[ 0u ].location.Sum ( buffer, _tempPoseGlobal[ 0u ].location );

    for ( GXUShort i = 1u; i < _totalBones; ++i )
    {
        const GXBoneJoint& parentBoneTransformGlobal = _tempPoseGlobal[ _parentBoneIndices[ i ] ];
        const GXBoneJoint& boneInverseBindTransform = _inverseBindTransform[ i ];
        const GXBoneJoint& boneTransformLocal = _tempPoseLocal[ i ];
        GXBoneJoint& boneTransformGlobal = _tempPoseGlobal[ i ];
        GXBoneJoint& boneSkinTransform = _skinTransform[ i ];

        boneTransformGlobal.rotation.Multiply ( parentBoneTransformGlobal.rotation, boneTransformLocal.rotation );
        parentBoneTransformGlobal.rotation.TransformFast ( buffer, boneTransformLocal.location );
        boneTransformGlobal.location.Sum ( parentBoneTransformGlobal.location, buffer );

        boneSkinTransform.rotation.Multiply ( boneTransformGlobal.rotation, boneInverseBindTransform.rotation );
        boneTransformGlobal.rotation.TransformFast ( buffer, boneInverseBindTransform.location );
        boneSkinTransform.location.Sum ( boneTransformGlobal.location, buffer );
    }
}
