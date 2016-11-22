//version 1.1


#include <GXCommon/GXNativeStaticMeshSaver.h>
#include <GXCommon/GXNativeStaticMeshStructs.h>
#include <GXCommon/GXFileSystem.h>


GXVoid GXCALL GXCompileNativeStaticMesh ( GXUByte** data, GXUInt &size, const GXNativeStaticMeshDesc &descriptor )
{
	size = sizeof ( GXNativeStaticMeshHeader );
	size += descriptor.numVertices * sizeof ( GXVec3 );
	size += descriptor.numUVs * sizeof ( GXVec2 );
	size += descriptor.numNormals * sizeof ( GXVec3 );
	size += 2 * descriptor.numTBPairs * sizeof ( GXVec3 );	//Tangents and Bitangents
	size += descriptor.numElements * sizeof ( GXUInt );

	*data = (GXUByte*)malloc ( size );

	GXNativeStaticMeshHeader* h = (GXNativeStaticMeshHeader*)(*data);
	h->numVertices = descriptor.numVertices;
	h->numNormals = descriptor.numNormals;
	h->numTBPairs = descriptor.numTBPairs;
	h->numUVs = descriptor.numUVs;
	h->numElements = descriptor.numElements;

	GXSetAABBEmpty ( h->bounds );

	GXUInt offset = sizeof ( GXNativeStaticMeshHeader );
	for ( GXUInt i = 0; i < descriptor.numVertices; i++ )
	{
		GXAddVertexToAABB ( h->bounds, descriptor.vertices[ i ] );

		memcpy ( *data + offset, descriptor.vertices + i, sizeof ( GXVec3 ) );
		offset += sizeof ( GXVec3 );

		if ( descriptor.numUVs > 0 )
		{
			memcpy ( *data + offset, descriptor.uvs + i, sizeof ( GXVec2 ) );
			offset += sizeof ( GXVec2 );
		}

		if ( descriptor.numNormals > 0 )
		{
			memcpy ( *data + offset, descriptor.normals + i, sizeof ( GXVec3 ) );
			offset += sizeof ( GXVec3 );
		}

		if ( descriptor.numTBPairs > 0 )
		{
			memcpy ( *data + offset, descriptor.tangents + i, sizeof ( GXVec3 ) );
			offset += sizeof ( GXVec3 );
			memcpy ( *data + offset, descriptor.bitangents + i, sizeof ( GXVec3 ) );
			offset += sizeof ( GXVec3 );
		}
	}

	h->elementOffset = offset;
	memcpy ( *data + offset, descriptor.elements, descriptor.numElements * sizeof ( GXUInt ) );
}

GXVoid GXCALL GXExportNativeStaticMesh ( const GXWChar* fileName, const GXNativeStaticMeshDesc &descriptor )
{
	GXUByte* data;
	GXUInt size;
	GXCompileNativeStaticMesh ( &data, size, descriptor );

	GXWriteToFile ( fileName, data, size );

	free ( data );
}
