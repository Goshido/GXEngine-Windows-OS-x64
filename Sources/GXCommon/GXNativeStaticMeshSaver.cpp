// version 1.5

#include <GXCommon/GXNativeStaticMeshSaver.h>
#include <GXCommon/GXNativeStaticMeshStructs.h>
#include <GXCommon/GXFileSystem.h>


GXVoid GXCALL GXCompileNativeStaticMesh ( GXUByte** data, GXUInt &size, const GXNativeStaticMeshDesc &descriptor )
{
    size = sizeof ( GXNativeStaticMeshHeader );
    size += descriptor._numVertices * sizeof ( GXVec3 );
    size += descriptor._numUVs * sizeof ( GXVec2 );
    size += descriptor._numNormals * sizeof ( GXVec3 );
    size += 2u * descriptor._numTBPairs * sizeof ( GXVec3 ); // Tangents and Bitangents
    size += descriptor._numElements * sizeof ( GXUInt );

    *data = static_cast<GXUByte*> ( malloc ( size ) );

    GXNativeStaticMeshHeader* h = reinterpret_cast<GXNativeStaticMeshHeader*> ( *data );
    h->_numVertices = descriptor._numVertices;
    h->_numNormals = descriptor._numNormals;
    h->_numTBPairs = descriptor._numTBPairs;
    h->_numUVs = descriptor._numUVs;
    h->_numElements = descriptor._numElements;

    h->_bounds.Empty ();

    GXUInt offset = sizeof ( GXNativeStaticMeshHeader );

    for ( GXUInt i = 0u; i < descriptor._numVertices; ++i )
    {
        h->_bounds.AddVertex ( descriptor._vertices[ i ] );

        memcpy ( *data + offset, descriptor._vertices + i, sizeof ( GXVec3 ) );
        offset += sizeof ( GXVec3 );

        if ( descriptor._numUVs > 0u )
        {
            memcpy ( *data + offset, descriptor._uvs + i, sizeof ( GXVec2 ) );
            offset += sizeof ( GXVec2 );
        }

        if ( descriptor._numNormals > 0u )
        {
            memcpy ( *data + offset, descriptor._normals + i, sizeof ( GXVec3 ) );
            offset += sizeof ( GXVec3 );
        }

        if ( descriptor._numTBPairs > 0u )
        {
            memcpy ( *data + offset, descriptor._tangents + i, sizeof ( GXVec3 ) );
            offset += sizeof ( GXVec3 );
            memcpy ( *data + offset, descriptor._bitangents + i, sizeof ( GXVec3 ) );
            offset += sizeof ( GXVec3 );
        }
    }

    h->_elementOffset = offset;
    memcpy ( *data + offset, descriptor._elements, descriptor._numElements * sizeof ( GXUInt ) );
}

GXVoid GXCALL GXExportNativeStaticMesh ( const GXWChar* fileName, const GXNativeStaticMeshDesc &descriptor )
{
    GXUByte* data;
    GXUInt size;
    GXCompileNativeStaticMesh ( &data, size, descriptor );

    GXWriteToFile ( fileName, data, size );

    free ( data );
}
