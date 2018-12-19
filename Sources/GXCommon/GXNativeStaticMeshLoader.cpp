// version 1.3

#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


GXNativeStaticMeshInfo::GXNativeStaticMeshInfo ()
{
    vboData = nullptr;
    eboData = nullptr;
}

GXVoid GXNativeStaticMeshInfo::Cleanup ()
{
    GXSafeFree ( vboData );
    GXSafeFree ( eboData );
}

//----------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeStaticMesh ( const GXWChar* fileName, GXNativeStaticMeshInfo &info )
{
    GXUPointer size = 0u;
    GXUByte* data = nullptr;

    if ( !GXLoadFile ( fileName, reinterpret_cast<GXVoid**> ( &data ), size, GX_TRUE ) )
    {
        GXLogW ( L"GXLoadNativeStaticMesh::Error - Can't load file %s\n", fileName );
        return;
    }

    const GXNativeStaticMeshHeader* h = reinterpret_cast<const GXNativeStaticMeshHeader*> ( data );

    info.bounds = h->bounds;

    info.numVertices = h->numVertices;
    info.numUVs = h->numUVs;
    info.numNormals = h->numNormals;
    info.numTBPairs = h->numTBPairs;

    size = info.numVertices * sizeof ( GXVec3 ) + info.numUVs * sizeof ( GXVec2 ) + info.numNormals * sizeof ( GXVec3 ) + info.numTBPairs * 2 * sizeof ( GXVec3 );
    info.vboData = static_cast<GXUByte*> ( malloc ( size ) );
    memcpy ( info.vboData, data + sizeof ( GXNativeStaticMeshHeader ), size );

    info.numElements = h->numElements;

    if ( info.numElements > 0u )
    {
        size = info.numElements * sizeof ( GXUInt );
        info.eboData = static_cast<GXUByte*> ( malloc ( size ) );
        memcpy ( info.eboData, data + h->elementOffset, size );
    }

    free ( data );
}
