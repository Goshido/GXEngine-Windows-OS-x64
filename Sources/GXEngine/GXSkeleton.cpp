//version 1.0

#include <GXEngine/GXSkeleton.h>


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

GXVoid GXSkeleton::UpdatePose ( GXAnimSolver &solver )
{
	GXUInt boneNameOffset = 0;
	for ( GXUShort i = 0; i < numBones; i++ )
	{
		const GXQuat* rotation = nullptr;
		const GXVec3* location = nullptr;

		solver.GetBone ( boneNames + boneNameOffset, &rotation, &location );

		if ( rotation )
		{
			tempPoseLocal[ i ].rotation = *rotation;
			tempPoseLocal[ i ].location = *location;
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

GXVoid GXSkeleton::CalculatePose ()
{
	tempPoseGlobal[ 0 ].rotation = tempPoseLocal[ 0 ].rotation;
	tempPoseGlobal[ 0 ].location = tempPoseLocal[ 0 ].location;

	GXMulQuatQuat ( pose[ 0 ].rotation, bindTransform[ 0 ].rotation, tempPoseGlobal[ 0 ].rotation );
	GXVec3 alpha;
	GXQuatTransform ( alpha, pose[ 0 ].rotation, bindTransform[ 0 ].location );
	GXSumVec3Vec3 ( pose[ 0 ].location, alpha, tempPoseGlobal[ 0 ].location );

	for ( GXUShort i = 0; i < numBones; i++ )
	{
		GXShort parentIndex = parentBoneIndices[ i ];

		GXMulQuatQuat ( tempPoseGlobal[ i ].rotation, tempPoseLocal[ i ].rotation, tempPoseGlobal[ parentIndex ].rotation );
		GXQuatTransform ( alpha, tempPoseGlobal[ parentIndex ].rotation, tempPoseLocal[ i ].location );
		GXSumVec3Vec3 ( tempPoseGlobal[ i ].location, alpha, tempPoseGlobal[ parentIndex ].location );

		GXMulQuatQuat ( pose[ i ].rotation, bindTransform[ i ].rotation, tempPoseGlobal[ i ].rotation );
		GXQuatTransform ( alpha, pose[ i ].rotation, bindTransform[ i ].location );
		GXSumVec3Vec3 ( pose[ i ].location, alpha, tempPoseGlobal[ i ].location );
	}
}
