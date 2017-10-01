//version 1.0

#include <GXEngine/GXSkeleton.h>
#include <GXCommon/GXMemory.h>


GXSkeleton::GXSkeleton ()
{
	numBones = 0;
	boneNames = nullptr;
	parentBoneIndices = nullptr;
	pose = nullptr;
	tempPoseLocal = nullptr;
	tempPoseGlobal = nullptr;
	referencePose = nullptr;
	bindTransform = nullptr;
}

GXSkeleton::~GXSkeleton ()
{
	if ( numBones == 0 ) return;

	free ( boneNames );
	free ( parentBoneIndices );
	free ( pose );
	free ( tempPoseLocal );
	free ( tempPoseGlobal );
	free ( referencePose );
	free ( bindTransform );
}

GXVoid GXSkeleton::LoadFromSkm ( const GXWChar* fileName )
{
	GXSkeletalMeshData skeletalMeshData;
	GXLoadNativeSkeletalMesh ( fileName, skeletalMeshData );

	numBones = skeletalMeshData.numBones;

	GXUInt size = numBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	boneNames = (GXUTF8*)malloc ( size );
	memcpy ( boneNames, skeletalMeshData.boneNames, size );

	size = numBones * sizeof ( GXShort );
	parentBoneIndices = (GXShort*)malloc ( size );
	memcpy ( parentBoneIndices, skeletalMeshData.parentIndex, size );

	size = numBones * sizeof ( GXQuatLocJoint );
	referencePose = (GXQuatLocJoint*)malloc ( size );
	memcpy ( referencePose, skeletalMeshData.refPose, size );

	bindTransform = (GXQuatLocJoint*)malloc ( size );
	memcpy ( bindTransform, skeletalMeshData.bindTransform, size );

	tempPoseLocal = (GXQuatLocJoint*)malloc ( size );
	memcpy ( tempPoseLocal, referencePose, size );

	tempPoseGlobal = (GXQuatLocJoint*)malloc ( size );
	pose = (GXQuatLocJoint*)malloc ( size );

	CalculatePose ();

	skeletalMeshData.Cleanup ();
}

GXVoid GXSkeleton::UpdatePose ( GXAnimationSolver &solver )
{
	GXUInt boneNameOffset = 0;

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		GXQuat rotation;
		GXVec3 location;

		if ( solver.GetBone ( boneNames + boneNameOffset, rotation, location ) )
		{
			tempPoseLocal[ i ].rotation = rotation;
			tempPoseLocal[ i ].location = location;
		}
		else
		{
			tempPoseLocal[ i ].rotation = referencePose[ i ].rotation;
			tempPoseLocal[ i ].location = referencePose[ i ].location;
		}

		boneNameOffset += GX_BONE_NAME_SIZE;
	}

	CalculatePose ();
}

const GXQuatLocJoint* GXSkeleton::GetPose () const
{
	return pose;
}

GXUShort GXSkeleton::GetTotalBones () const
{
	return numBones;
}

GXVoid GXSkeleton::CalculatePose ()
{
	tempPoseGlobal[ 0 ].rotation = tempPoseLocal[ 0 ].rotation;
	tempPoseGlobal[ 0 ].location = tempPoseLocal[ 0 ].location;

	pose[ 0 ].rotation.Multiply ( bindTransform[ 0 ].rotation, tempPoseGlobal[ 0 ].rotation );
	GXVec3 alpha;
	pose[ 0 ].rotation.Transform ( alpha, bindTransform[ 0 ].location );
	pose[ 0 ].location.Sum ( alpha, tempPoseGlobal[ 0 ].location );

	for ( GXUShort i = 1; i < numBones; i++ )
	{
		GXShort parentIndex = parentBoneIndices[ i ];

		tempPoseGlobal[ i ].rotation.Multiply ( tempPoseGlobal[ parentIndex ].rotation, tempPoseLocal[ i ].rotation );
		tempPoseGlobal[ parentIndex ].rotation.Transform ( alpha, tempPoseLocal[ i ].location );
		tempPoseGlobal[ i ].location.Sum ( alpha, tempPoseGlobal[ parentIndex ].location );

		pose[ i ].rotation.Multiply ( bindTransform[ i ].rotation, tempPoseGlobal[ i ].rotation );
		pose[ i ].rotation.Transform ( alpha, bindTransform[ i ].location );
		pose[ i ].location.Sum ( alpha, tempPoseGlobal[ i ].location );
	}
}
