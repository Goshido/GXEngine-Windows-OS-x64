//version 1.3

#ifndef GX_SKELETAL_MESH_QUAT
#define GX_SKELETAL_MESH_QUAT


#include "GXSkeletalMesh.h"
#include "GXAnimSolver.h"
#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXEngine/GXSkeletalMeshStorage.h>


#define GX_MAX_BONES_FLOATS MAX_BONES * 7		//Так как одна кость это GXQuat + GXVec3, то MAX_BONES_FLOATS = MAX_BONES * 7

class GXBoneKeeper;
class GXSkeletalMeshQuat : public GXMesh
{
	protected:
		GXSkeletalMeshInfoExt		skmInfo;

		GXQuatLocJoint				localPose[ MAX_BONES ];
		GXQuatLocJoint				globalPose[ MAX_BONES ];
		GXQuatLocJoint				vertexTransform[ MAX_BONES ];

		GXAnimSolver*				animSolver;
		GXBoneKeeper*				keeper;

	public:
		GXSkeletalMeshQuat ();
		virtual ~GXSkeletalMeshQuat ();

		virtual	GXVoid GetBoneGlobalLocation ( GXVec3 &out, const GXUTF8* bone );
		virtual	GXVoid GetBoneGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXUTF8* bone );
		virtual	GXVoid GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXUTF8* bone );
		virtual	GXVoid GetAttacmentGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXVec3 &offset, const GXUTF8* bone );
		GXVoid SetAnimSolver ( GXAnimSolver* animSolver );

		GXVoid Update ( GXFloat deltaTime );

	protected:
		GXVoid InitReferencePose ();
		GXVoid InitPose ();
};


#endif //GX_SKELETAL_MESH_QUAT
