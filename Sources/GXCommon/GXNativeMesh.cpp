//vesrion 1.0

#include <GXCommon/GXNativeMesh.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


GXMeshInfo::GXMeshInfo ()
{
	totalVertices = 0;
	vboData = nullptr;
}

GXVoid GXMeshInfo::Cleanup ()
{
	GXSafeFree ( vboData );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeMesh ( GXMeshInfo &info, const GXWChar* fileName )
{
	GXUByte* data;
	GXUBigInt fileSize;

	if ( !GXLoadFile ( fileName, (GXVoid**)&data, fileSize, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadNativeMesh::Error - Can't load file %s\n", fileName );
		return;
	}

	GXNativeMeshHeader* h = (GXNativeMeshHeader*)data;

	info.totalVertices = h->totalVertices;

	GXUPointer size = info.totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) );
	info.vboData = (GXFloat*)malloc ( size );
	memcpy ( info.vboData, data + h->vboOffset, size );

	free ( data );
}
