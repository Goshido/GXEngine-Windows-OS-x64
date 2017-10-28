//version 1.1

#ifndef GX_BONE
#define GX_BONE


#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGame.h>
#include <GXCommon/GXRestoreWarnings.h>


struct GXBone
{
	GXUTF8			name[ GX_BONE_NAME_SIZE ];
	GXUShort		ownIndex;
	GXUShort		parentIndex;
	GXBool			isRootBone;

	IGameNode*		node;

	GXMat4			transformWorld;
	GXBoneJoint		referenceTransform;
	GXBoneJoint		inverseBindTransform;
	GXBoneJoint		poseTransform;

	GXBone*			father;
	GXBone*			brother;
	GXBone*			son;

	GXVoid Init ( GXUShort newID, IGameNode* newNode, const GXMat4 &newTransformWorld );
	GXVoid AddSon ( GXBone* newSon );

	GXBone& operator = ( const GXBone &other );
};


#endif //GX_BONE
