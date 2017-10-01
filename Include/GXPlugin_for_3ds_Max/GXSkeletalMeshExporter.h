//version 1.1

#ifndef GX_SKELETAL_MESH_EXPORTER
#define GX_SKELETAL_MESH_EXPORTER


#include <GXCommon/GXMemory.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGame.h>
#include <GXCommon/GXRestoreWarnings.h>


class GXSkeleton;
class GXIndexWeightStack;
class GXSkeletalMeshExporter
{
	private:
		GXBool					isValid;

		GXSkeleton*				skeleton;
		GXIndexWeightStack*		stack;

		GXUInt					numVertices;
		GXUByte*				vboData;

	public:
		explicit GXSkeletalMeshExporter ( INode &selection, const GXUTF8* fileName );
		~GXSkeletalMeshExporter ();

	private:
		GXVoid Init ( INode &selection );
		GXVoid ExtractGeometryData ( IGameObject &skinMesh );
		GXVoid Save ( const GXUTF8* fileName ) const;
};

struct GXBone;
class GXAnimationExporter
{
	private:
		IGameSkin*	skin;
		IGameScene*	game;
		IGameNode*	node;

		GXUShort	totalBones;
		GXBone*		bones;

		GXBool		isValid;

	public:
		explicit GXAnimationExporter ( INode* selection, const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame );
		~GXAnimationExporter ();

	private:
		GXVoid Init ( INode* selection );
		GXVoid Save ( const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame );

		GXVoid ExtractBones ( IGameObject* skinMesh );
		GXVoid ExtractFrame ( GXUInt frame );
};


#endif //GX_SKELETAL_MESH_EXPORTER
