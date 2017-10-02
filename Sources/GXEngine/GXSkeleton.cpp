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

		if ( solver.GetBone ( rotation, location, boneNames + boneNameOffset ) )
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

	GXMat4 bindPoseMatrix;
	bindPoseMatrix.From ( bindTransform[ 0 ].rotation, bindTransform[ 0 ].location );

	GXMat4 poseGlobalMatrix;
	poseGlobalMatrix.From ( tempPoseGlobal[ 0 ].rotation, tempPoseGlobal[ 0 ].location );

	GXMat4 poseMatrix;
	poseMatrix.Multiply ( poseGlobalMatrix, bindPoseMatrix );

	pose[ 0 ].rotation.From ( poseMatrix );
	poseMatrix.GetW ( pose[ 0 ].location );

	/*
	GXVec3 alpha;

	pose[ 0 ].rotation.Multiply ( tempPoseGlobal[ 0 ].rotation, bindTransform[ 0 ].rotation );
	tempPoseGlobal[ 0 ].rotation.Transform ( alpha, bindTransform[ 0 ].location );
	pose[ 0 ].location.Sum ( alpha, tempPoseGlobal[ 0 ].location );
	*/

	for ( GXUShort i = 1; i < numBones; i++ )
	{
		GXShort parentIndex = parentBoneIndices[ i ];

		GXMat4 poseLocalMatrix;
		poseLocalMatrix.From ( tempPoseLocal[ i ].rotation, tempPoseLocal[ i ].location );

		GXMat4 poseParentGlobalMatrix;
		poseParentGlobalMatrix.From ( tempPoseGlobal[ parentIndex ].rotation, tempPoseGlobal[ parentIndex ].location );

		poseGlobalMatrix.Multiply ( poseParentGlobalMatrix, poseLocalMatrix );
		tempPoseGlobal[ i ].rotation.From ( poseGlobalMatrix );
		poseGlobalMatrix.GetW ( tempPoseGlobal[ i ].location );

		bindPoseMatrix.From ( bindTransform[ i ].rotation, bindTransform[ i ].location );
		poseMatrix.Multiply ( poseGlobalMatrix, bindPoseMatrix );

		pose[ i ].rotation.From ( poseMatrix );
		poseMatrix.GetW ( pose[ i ].location );

		/*
		tempPoseGlobal[ i ].rotation.Multiply ( tempPoseGlobal[ parentIndex ].rotation, tempPoseLocal[ i ].rotation );
		tempPoseGlobal[ parentIndex ].rotation.Transform ( alpha, tempPoseLocal[ i ].location );
		tempPoseGlobal[ i ].location.Sum ( alpha, tempPoseGlobal[ parentIndex ].location );

		pose[ i ].rotation.Multiply ( tempPoseGlobal[ i ].rotation, bindTransform[ i ].rotation );
		tempPoseGlobal[ i ].rotation.Transform ( alpha, bindTransform[ i ].location );
		pose[ i ].location.Sum ( alpha, tempPoseGlobal[ i ].location );
		*/
	}
}
