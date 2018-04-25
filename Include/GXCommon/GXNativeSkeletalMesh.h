// version 1.3

#ifndef GX_NATIVE_SKELETAL_MESH
#define GX_NATIVE_SKELETAL_MESH


#include "GXMath.h"


#define GX_BONE_NAME_SIZE				64
#define GX_FLOATS_PER_BONE				7
#define GX_MAXIMUM_BONES_PER_MESH		80

#define GX_UNKNOWN_BONE_INDEX			0xFFFEu
#define GX_ROOT_BONE_PARENT_INDEX		0xFFFFu


#pragma pack ( push )
#pragma pack ( 1 )

struct GXNativeSkeletalMeshHeader
{
	GXUInt		totalVertices;
	GXUBigInt	vboOffset;						// VBO element struct: position (GXVec3), uv (GXVec2), normal (GXVec3), tangent (GXVec3), bitangent (GXVec3), indices (GXVec4), weights (GXVec4).

	GXUShort	totalBones;
	GXUBigInt	boneNamesOffset;				// array of totalBones elements: 64 byte slot with zero terminated UTF8 string.
	GXUBigInt	parentBoneIndicesOffset;		// array of totalBones elements: zero based index (GXUShort).
	GXUBigInt	referensePoseOffset;			// array of totalBones elements: location (GXVec3), rotation (GXQuat). Relative parent.
	GXUBigInt	inverseBindTransformOffset;		// array of totalBones elements: location (GXVec3), rotation (GXQuat).
};

#pragma pack ( pop )

struct GXBoneJoint
{
	GXQuat	rotation;
	GXVec3	location;
};

struct GXSkeletalMeshData
{
	GXUInt					totalVertices;
	GXFloat*				vboData;

	GXUShort				totalBones;
	GXUTF8*					boneNames;
	GXUShort*				parentBoneIndices;
	GXBoneJoint*			referencePose;
	GXBoneJoint*			inverseBindTransform;

	GXSkeletalMeshData ();
	GXVoid Cleanup ();
};

#pragma pack ( push )
#pragma pack ( 1 )

struct GXNativeAnimationHeader
{
	GXFloat		fps;
	GXUInt		totalFrames;

	GXUShort	totalBones;
	GXUBigInt	boneNamesOffset;		// array of totalBones elements: 64 byte slot with zero terminated UTF-8 string
	GXUBigInt	keysOffset;				// array of [totalBones * totalFrames] elements: rotation (GXQuat), location (GXVec3). Relative parent.
	GXUBigInt	keysOffset2;
};

#pragma pack ( pop )

struct GXAnimationInfo
{
	GXUShort				totalBones;
	GXUTF8*					boneNames;

	GXFloat					fps;
	GXUInt					totalFrames;
	GXBoneJoint*			keys;

	GXAnimationInfo ();
	GXVoid Cleanup ();
};

//------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeletalMesh ( GXSkeletalMeshData &info, const GXWChar* fileName );
GXVoid GXCALL GXLoadNativeAnimation ( GXAnimationInfo &info, const GXWChar* fileName );


#endif // GX_NATIVE_SKELETAL_MESH
