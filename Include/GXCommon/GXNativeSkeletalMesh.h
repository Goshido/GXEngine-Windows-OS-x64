//version 1.0

#ifndef GX_NATIVE_SKELETAL_MESH
#define GX_NATIVE_SKELETAL_MESH


#include "GXMath.h"
#include "GXStrings.h"


#define GX_BONE_NAME_SIZE	64


struct GXNativeSkeletalMeshHeader
{
	GXUInt		numVertices;
	GXUInt		vboOffset;			//VBO element struct: position (GXVec3), uv (GXVec2), normal (GXVec3), tangent (GXVec3), bitangent (GXVec3), indices (GXVec4), weights (GXVec4)

	GXUShort	numBones;
	GXUInt		boneNamesOffset;			//array of numBones elements: 64 byte slot with zero terminated UTF8 string
	GXUInt		boneParentIndicesOffset;	//array of numBones elements: zero based index (GXShort). -1 means no parent (root bone)
	GXUInt		refPoseOffset;				//array of numBones elements: location (GXVec3), rotation (GXQuat). Relative parent
	GXUInt		bindTransformOffset;		//array of numBones elements: location (GXVec3), rotation (GXQuat)
};

struct GXQuatLocJoint
{
	GXQuat	rotation;
	GXVec3	location;
};

struct GXSkeletalMeshData
{
	GXUInt					numVertices;
	GXFloat*				vboData;

	GXUShort				numBones;
	GXUTF8*					boneNames;
	GXShort*				parentIndex;
	GXQuatLocJoint*			refPose;
	GXQuatLocJoint*			bindTransform;

	GXSkeletalMeshData ();
	GXVoid Cleanup ();
};

struct GXNativeAnimationHeader
{
	GXFloat		fps;
	GXUInt		numFrames;

	GXUShort	numBones;
	GXUInt		boneNamesOffset;		//array of numBones elements: 64 byte slot with zero terminated UTF8 string
	GXUInt		keysOffset;				//array of [numBones * numFrames] elements: location (GXVec3), rotation (GXQuat). Relative parent
};

struct GXAnimationInfoExt
{
	GXUShort				numBones;
	GXUTF8*					boneNames;

	GXFloat					fps;
	GXUInt					numFrames;
	GXQuatLocJoint*			keys;
};

//------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeletalMesh ( const GXWChar* fileName, GXSkeletalMeshData &info );
GXVoid GXCALL GXLoadNativeAnimation ( const GXWChar* fileName, GXAnimationInfoExt &info );

#endif //GX_NATIVE_SKELETAL_MESH
