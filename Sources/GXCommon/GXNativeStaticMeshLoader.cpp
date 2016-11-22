//version 1.0

#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>


GXNativeStaticMeshInfo::GXNativeStaticMeshInfo ()
{
	vboData = 0;
	eboData = 0;
}

GXVoid GXNativeStaticMeshInfo::Cleanup ()
{
	GXSafeFree ( vboData );
	GXSafeFree ( eboData );
}

//----------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeStaticMesh ( const GXWChar* fileName, GXNativeStaticMeshInfo &info )
{
	GXUInt size;
	GXUByte* data;

	if ( !GXLoadFile ( fileName, (GXVoid**)&data, size, GX_TRUE ) )
	{
		GXLogA ( "GXLoadNativeStaticMesh::Error - Can't load file\n" );
		return;
	}

	GXLoadNativeStaticMesh ( data, info );

	free ( data );
}

GXVoid GXCALL GXLoadNativeStaticMesh ( const GXUByte* mappedFile, GXNativeStaticMeshInfo &info )
{
	GXNativeStaticMeshHeader* h = (GXNativeStaticMeshHeader*)mappedFile;

	info.bounds = h->bounds;

	info.numVertices = h->numVertices;
	info.numUVs = h->numUVs;
	info.numNormals = h->numNormals;
	info.numTBPairs = h->numTBPairs;

	GXUInt size = info.numVertices * sizeof ( GXVec3 ) + info.numUVs * sizeof ( GXVec2 ) + info.numNormals * sizeof ( GXVec3 ) + info.numTBPairs * 2 * sizeof ( GXVec3 );
	info.vboData = (GXUByte*)malloc ( size );
	memcpy ( info.vboData, mappedFile + sizeof ( GXNativeStaticMeshHeader ), size );

	info.numElements = h->numElements;
	if ( info.numElements > 0 )
	{
		size = info.numElements * sizeof ( GXUInt );
		info.eboData = (GXUByte*)malloc ( size );
		memcpy ( info.eboData, mappedFile + h->elementOffset, size );
	}
}
