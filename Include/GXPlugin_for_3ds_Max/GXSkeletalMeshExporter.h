//version 1.1

#ifndef GX_SKELETAL_MESH_EXPORTER
#define GX_SKELETAL_MESH_EXPORTER


#include "GXSkeleton.h"
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGame.h>
#include <GXCommon/GXRestoreWarnings.h>


class GXSkeletalMeshExporter
{
	private:
		GXSkeleton				skeleton;
		GXUInt					numVertices;
		GXUByte*				vboData;

	public:
		explicit GXSkeletalMeshExporter ( INode &selection, const GXUTF8* fileName, const GXUTF8* reportFileName );
		~GXSkeletalMeshExporter ();

	private:
		GXBool Init ( INode &selection );
		GXBool ExtractGeometryData ( IGameObject &skinMesh );
		GXVoid Save ( const GXUTF8* fileName ) const;
		GXVoid SaveReport ( const GXUTF8* fileName ) const;
};


#endif //GX_SKELETAL_MESH_EXPORTER
