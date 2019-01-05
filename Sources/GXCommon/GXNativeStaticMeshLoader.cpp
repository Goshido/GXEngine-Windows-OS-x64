// version 1.3

#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


class GXNativeStaticMeshLoaderMemoryInspector final : public GXMemoryInspector
{
public:
    GXNativeStaticMeshLoaderMemoryInspector ();
    ~GXNativeStaticMeshLoaderMemoryInspector () override;

private:
    GXNativeStaticMeshLoaderMemoryInspector ( const GXNativeStaticMeshLoaderMemoryInspector &other );
    GXNativeStaticMeshLoaderMemoryInspector& operator = ( const GXNativeStaticMeshLoaderMemoryInspector &other );
};

GXNativeStaticMeshLoaderMemoryInspector::GXNativeStaticMeshLoaderMemoryInspector ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXNativeStaticMeshLoaderMemoryInspector" )
{
    // NOTHING
}

GXNativeStaticMeshLoaderMemoryInspector::~GXNativeStaticMeshLoaderMemoryInspector ()
{
    // NOTHING
}

static GXNativeStaticMeshLoaderMemoryInspector gx_NativeMeshLoaderMemoryInspector;

//-------------------------------------------------------------------------------------------------------------

GXNativeStaticMeshInfo::GXNativeStaticMeshInfo ():
    vboData ( nullptr ),
    eboData ( nullptr )
{
    // NOTHING
}

GXVoid GXNativeStaticMeshInfo::Cleanup ()
{
    gx_NativeMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &vboData ) );
    gx_NativeMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &eboData ) );
}

//----------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeStaticMesh ( const GXWChar* fileName, GXNativeStaticMeshInfo &info )
{
    GXUByte* data;
    GXUPointer size;

    GXFile file ( fileName );

    if ( !file.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_TRUE ) ) return;

    const GXNativeStaticMeshHeader* h = reinterpret_cast<const GXNativeStaticMeshHeader*> ( data );

    info.bounds = h->bounds;

    info.numVertices = h->numVertices;
    info.numUVs = h->numUVs;
    info.numNormals = h->numNormals;
    info.numTBPairs = h->numTBPairs;

    size = info.numVertices * sizeof ( GXVec3 ) + info.numUVs * sizeof ( GXVec2 ) + info.numNormals * sizeof ( GXVec3 ) + info.numTBPairs * 2 * sizeof ( GXVec3 );
    info.vboData = static_cast<GXUByte*> ( gx_NativeMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.vboData, data + sizeof ( GXNativeStaticMeshHeader ), size );

    info.numElements = h->numElements;

    if ( info.numElements < 1u ) return;

    size = info.numElements * sizeof ( GXUInt );
    info.eboData = static_cast<GXUByte*> ( gx_NativeMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.eboData, data + h->elementOffset, size );
}
