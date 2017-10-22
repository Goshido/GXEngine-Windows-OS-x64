//version 1.1

#include <GXEngine/GXSkeleton.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


GXSkeleton::GXSkeleton ()
{
	totalBones = 0;

	boneNames = nullptr;
	parentBoneIndices = nullptr;

	tempPoseLocal = nullptr;
	tempPoseGlobal = nullptr;
	referencePose = nullptr;
	referencePose2 = nullptr;
	inverseBindTransform = nullptr;
	inverseBindTransform2 = nullptr;
	pose = nullptr;
	pose2 = nullptr;
}

GXSkeleton::~GXSkeleton ()
{
	if ( totalBones == 0 ) return;

	free ( boneNames );
	free ( parentBoneIndices );

	free ( tempPoseLocal );
	free ( tempPoseLocal2 );
	free ( tempPoseGlobal );
	free ( tempPoseGlobal2 );
	free ( referencePose );
	free ( referencePose2 );
	free ( inverseBindTransform );
	free ( inverseBindTransform2 );
	free ( pose );
	free ( pose2 );
}

GXVoid GXSkeleton::LoadFromSkm ( const GXWChar* fileName )
{
	GXSkeletalMeshData skeletalMeshData;
	GXLoadNativeSkeletalMesh ( skeletalMeshData, fileName );

	totalBones = skeletalMeshData.totalBones;

	GXUPointer size = totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	boneNames = (GXUTF8*)malloc ( size );
	memcpy ( boneNames, skeletalMeshData.boneNames, size );

	size = totalBones * sizeof ( GXShort );
	parentBoneIndices = (GXShort*)malloc ( size );
	memcpy ( parentBoneIndices, skeletalMeshData.parentBoneIndices, size );

	size = totalBones * sizeof ( GXQuatLocJoint );
	referencePose = (GXQuatLocJoint*)malloc ( size );
	memcpy ( referencePose, skeletalMeshData.referencePose, size );

	size = totalBones * sizeof ( GXMat4 );
	referencePose2 = (GXMat4*)malloc ( size );
	memcpy ( referencePose2, skeletalMeshData.referencePose2, size );

	size = totalBones * sizeof ( GXQuatLocJoint );
	inverseBindTransform = (GXQuatLocJoint*)malloc ( size );
	memcpy ( inverseBindTransform, skeletalMeshData.inverseBindTransform, size );

	size = totalBones * sizeof ( GXMat4 );
	inverseBindTransform2 = (GXMat4*)malloc ( size );
	memcpy ( inverseBindTransform2, skeletalMeshData.inverseBindTransform2, size );

	size = totalBones * sizeof ( GXQuatLocJoint );
	tempPoseLocal = (GXQuatLocJoint*)malloc ( size );
	memcpy ( tempPoseLocal, referencePose, size );

	size = totalBones * sizeof ( GXMat4 );
	tempPoseLocal2 = (GXMat4*)malloc ( size );
	memcpy ( tempPoseLocal2, referencePose2, size );

	size = totalBones * sizeof ( GXQuatLocJoint );
	tempPoseGlobal = (GXQuatLocJoint*)malloc ( size );

	size = totalBones * sizeof ( GXMat4 );
	tempPoseGlobal2 = (GXMat4*)malloc ( size );

	size = totalBones * sizeof ( GXQuatLocJoint );
	pose = (GXQuatLocJoint*)malloc ( size );

	size = totalBones * sizeof ( GXMat4 );
	pose2 = (GXMat4*)malloc ( size );

	CalculatePose ();

	skeletalMeshData.Cleanup ();
}

GXVoid GXSkeleton::UpdatePose ( GXAnimationSolver &solver, GXFloat /*deltaTime*/ )
{
	GXUInt boneNameOffset = 0;

	for ( GXUShort i = 0; i < totalBones; i++ )
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
			tempPoseLocal[ i ].location = referencePose[ i ].location;
			tempPoseLocal[ i ].rotation = referencePose[ i ].rotation;
		}

		GXMat4 transform;

		if ( solver.GetBone2 ( transform, boneNames + boneNameOffset ) )
			tempPoseLocal2[ i ] = transform;
		else
			tempPoseLocal2[ i ] = referencePose2[ i ];

		boneNameOffset += GX_BONE_NAME_SIZE;
	}

	CalculatePose ();
}

const GXQuatLocJoint* GXSkeleton::GetPose () const
{
	return pose;
}

const GXMat4* GXSkeleton::GetPose2 () const
{
	return pose2;
}

const GXMat4& GXSkeleton::GetBoneTransformWorld ( GXUShort boneIndex ) const
{
	return tempPoseGlobal2[ boneIndex ];
}

const GXMat4& GXSkeleton::GetParentBoneTransformWorld ( GXUShort boneIndex ) const
{
	return tempPoseGlobal2[ parentBoneIndices[ boneIndex ] ];
}

GXUShort GXSkeleton::GetTotalBones () const
{
	return totalBones;
}

GXVoid GXSkeleton::CalculatePose ()
{
	//=======================================
	tempPoseGlobal2[ 0 ] = tempPoseLocal2[ 0 ];
	pose2[ 0 ].Multiply ( inverseBindTransform2[ 0 ], tempPoseGlobal2[ 0 ] );
	//=======================================

	for ( GXUShort i = 1; i < totalBones; i++ )
	{
		GXShort parentIndex = parentBoneIndices[ i ];

		//=======================================
		tempPoseGlobal2[ i ].Multiply ( tempPoseLocal2[ i ], tempPoseGlobal2[ parentIndex ] );
		pose2[ i ].Multiply ( inverseBindTransform2[ i ], tempPoseGlobal2[ i ] );
		//=======================================
	}
}
