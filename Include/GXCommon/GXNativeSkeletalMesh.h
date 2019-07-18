// version 1.9

#ifndef GX_NATIVE_SKELETAL_MESH
#define GX_NATIVE_SKELETAL_MESH


#include "GXMath.h"


#define GX_BONE_NAME_SIZE               64u
#define GX_FLOATS_PER_BONE              7u
#define GX_MAXIMUM_BONES_PER_MESH       80u

#define GX_UNKNOWN_BONE_INDEX           0xFFFEu
#define GX_ROOT_BONE_PARENT_INDEX       0xFFFFu


#pragma pack ( push )
#pragma pack ( 1 )

struct GXNativeSkeletalMeshHeader final
{
    GXUInt          _totalVertices;
    GXUBigInt       _vboOffset;                      // VBO element struct: position (GXVec3), uv (GXVec2), normal (GXVec3), tangent (GXVec3), bitangent (GXVec3), indices (GXVec4), weights (GXVec4).

    GXUShort        _totalBones;
    GXUBigInt       _boneNamesOffset;                // array of totalBones elements: 64 byte slot with zero terminated UTF8 string.
    GXUBigInt       _parentBoneIndicesOffset;        // array of totalBones elements: zero based index (GXUShort).
    GXUBigInt       _referensePoseOffset;            // array of totalBones elements: location (GXVec3), rotation (GXQuat). Relative parent.
    GXUBigInt       _inverseBindTransformOffset;     // array of totalBones elements: location (GXVec3), rotation (GXQuat).
};

#pragma pack ( pop )

struct GXBoneJoint final
{
    GXQuat      _rotation;
    GXVec3      _location;
};

struct GXSkeletalMeshData final
{
    GXUInt          _totalVertices;
    GXFloat*        _vboData;

    GXUShort        _totalBones;
    GXUTF8*         _boneNames;
    GXUShort*       _parentBoneIndices;
    GXBoneJoint*    _referencePose;
    GXBoneJoint*    _inverseBindTransform;

    GXSkeletalMeshData ();
    GXVoid Cleanup ();
};

#pragma pack ( push )
#pragma pack ( 1 )

struct GXNativeAnimationHeader final
{
    GXFloat         _fps;
    GXUInt          _totalFrames;

    GXUShort        _totalBones;
    GXUBigInt       _boneNamesOffset;       // array of totalBones elements: 64 byte slot with zero terminated UTF-8 string
    GXUBigInt       _keysOffset;            // array of [totalBones * totalFrames] elements: rotation (GXQuat), location (GXVec3). Relative parent.
    GXUBigInt       _keysOffset2;
};

#pragma pack ( pop )

struct GXAnimationInfo final
{
    GXUShort        _totalBones;
    GXUTF8*         _boneNames;

    GXFloat         _fps;
    GXUInt          _totalFrames;
    GXBoneJoint*    _keys;

    GXAnimationInfo ();
    GXVoid Cleanup ();
};

//------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeletalMesh ( GXSkeletalMeshData &info, const GXWChar* fileName );
GXVoid GXCALL GXLoadNativeAnimation ( GXAnimationInfo &info, const GXWChar* fileName );


#endif // GX_NATIVE_SKELETAL_MESH
