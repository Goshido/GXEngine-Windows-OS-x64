// version 1.4

#include <GXEngine/GXSkeleton.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


GXSkeleton::GXSkeleton ():
	totalBones ( 0u ),
	boneNames ( nullptr ),
	parentBoneIndices ( nullptr ),
	tempPoseLocal ( nullptr ),
	tempPoseGlobal ( nullptr ),
	referencePose ( nullptr ),
	inverseBindTransform ( nullptr ),
	skinTransform ( nullptr )
{
	// NOTHING
}

GXSkeleton::~GXSkeleton ()
{
	GXSafeFree ( boneNames );
	GXSafeFree ( parentBoneIndices );

	GXSafeFree ( tempPoseLocal );
	GXSafeFree ( tempPoseGlobal );
	GXSafeFree ( referencePose );
	GXSafeFree ( inverseBindTransform );
	GXSafeFree ( skinTransform );
}

GXVoid GXSkeleton::LoadFromSkm ( const GXWChar* fileName )
{
	GXSkeletalMeshData skeletalMeshData;
	GXLoadNativeSkeletalMesh ( skeletalMeshData, fileName );

	totalBones = skeletalMeshData.totalBones;

	GXUPointer size = totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	boneNames = static_cast<GXUTF8*> ( malloc ( size ) );
	memcpy ( boneNames, skeletalMeshData.boneNames, size );

	size = totalBones * sizeof ( GXShort );
	parentBoneIndices = static_cast<GXShort*> ( malloc ( size ) );
	memcpy ( parentBoneIndices, skeletalMeshData.parentBoneIndices, size );

	size = totalBones * sizeof ( GXBoneJoint );
	referencePose = static_cast<GXBoneJoint*> ( malloc ( size ) );
	memcpy ( referencePose, skeletalMeshData.referencePose, size );

	inverseBindTransform = static_cast<GXBoneJoint*> ( malloc ( size ) );
	memcpy ( inverseBindTransform, skeletalMeshData.inverseBindTransform, size );

	tempPoseLocal = static_cast<GXBoneJoint*> ( malloc ( size ) );
	memcpy ( tempPoseLocal, referencePose, size );

	tempPoseGlobal = static_cast<GXBoneJoint*> ( malloc ( size ) );
	skinTransform = static_cast<GXBoneJoint*> ( malloc ( size ) );

	CalculatePose ();

	skeletalMeshData.Cleanup ();
}

GXVoid GXSkeleton::UpdatePose ( GXAnimationSolver &solver, GXFloat /*deltaTime*/ )
{
	GXUInt boneNameOffset = 0u;

	for ( GXUShort i = 0u; i < totalBones; ++i )
	{
		GXBoneJoint joint;

		if ( solver.GetBoneJoint ( joint, boneNames + boneNameOffset ) )
			tempPoseLocal[ i ] = joint;
		else
			tempPoseLocal[ i ] = referencePose[ i ];

		boneNameOffset += GX_BONE_NAME_SIZE;
	}

	CalculatePose ();
}

const GXBoneJoint* GXSkeleton::GetSkinTransform () const
{
	return skinTransform;
}

const GXBoneJoint& GXSkeleton::GetBoneTransformWorld ( GXUShort boneIndex ) const
{
	return tempPoseGlobal[ boneIndex ];
}

const GXBoneJoint& GXSkeleton::GetParentBoneTransformWorld ( GXUShort boneIndex ) const
{
	return tempPoseGlobal[ parentBoneIndices[ boneIndex ] ];
}

GXUShort GXSkeleton::GetTotalBones () const
{
	return totalBones;
}

GXVoid GXSkeleton::CalculatePose ()
{
	// Note: Quaternion mathematics simular to column-major notation matrix mathematics.
	// So we need to do multiplication in reverse order to calculate skin transform.

	tempPoseGlobal[ 0u ] = tempPoseLocal[ 0u ];
	skinTransform[ 0u ].rotation.Multiply ( tempPoseGlobal[ 0u ].rotation, inverseBindTransform[ 0u ].rotation );
	GXVec3 buffer;
	tempPoseGlobal[ 0u ].rotation.TransformFast ( buffer, inverseBindTransform[ 0u ].location );
	skinTransform[ 0u ].location.Sum ( buffer, tempPoseGlobal[ 0u ].location );

	for ( GXUShort i = 1u; i < totalBones; ++i )
	{
		const GXBoneJoint& parentBoneTransformGlobal = tempPoseGlobal[ parentBoneIndices[ i ] ];
		const GXBoneJoint& boneInverseBindTransform = inverseBindTransform[ i ];
		const GXBoneJoint& boneTransformLocal = tempPoseLocal[ i ];
		GXBoneJoint& boneTransformGlobal = tempPoseGlobal[ i ];
		GXBoneJoint& boneSkinTransform = skinTransform[ i ];

		boneTransformGlobal.rotation.Multiply ( parentBoneTransformGlobal.rotation, boneTransformLocal.rotation );
		parentBoneTransformGlobal.rotation.TransformFast ( buffer, boneTransformLocal.location );
		boneTransformGlobal.location.Sum ( parentBoneTransformGlobal.location, buffer );

		boneSkinTransform.rotation.Multiply ( boneTransformGlobal.rotation, boneInverseBindTransform.rotation );
		boneTransformGlobal.rotation.TransformFast ( buffer, boneInverseBindTransform.location );
		boneSkinTransform.location.Sum ( boneTransformGlobal.location, buffer );
	}
}
