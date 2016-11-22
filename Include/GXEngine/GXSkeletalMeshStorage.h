//version 1.0

#ifndef GX_SKELETAL_MESH_STORAGE
#define GX_SKELETAL_MESH_STORAGE


#include <GXCommon/GXMath.h>
#include <GXCommon/GXNativeSkeletalMesh.h>
#include "GXOpenGL.h"
#include "GXAnimSolver.h"


struct GXSkeletalMeshInfoExt
{
	GXUInt					numVertices;
	GLuint					vao;
	GLuint					vbo;
	GXAABB					bounds;

	GXUShort				numBones;
	GXUTF8*					boneNames;
	GXShort*				parentIndex;
	GXQuatLocJoint*			refPose;
	GXQuatLocJoint*			bindTransform;
};

GXVoid GXCALL GXGetSkeletalMesh ( const GXWChar* fileName, GXSkeletalMeshInfoExt &info );
GXVoid GXCALL GXRemoveSkeletalMesh ( const GXSkeletalMeshInfoExt &info );
GXUInt GXCALL GXGetTotalSkeletalMeshStorageObjects ( const GXWChar** lastSkeletalMesh );

GXVoid GXCALL GXGetAnimation ( const GXWChar* fileName, GXAnimationInfoExt &info );
GXVoid GXCALL GXRemoveAnimation ( const GXAnimationInfoExt &info );
GXUInt GXCALL GXGetTotalAnimationStorageObjects ( const GXWChar** lastAnimation );

#endif //GX_SKELETAL_MESH_STORAGE
