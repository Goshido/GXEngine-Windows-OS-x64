// version 1.4

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
    GXNativeStaticMeshLoaderMemoryInspector ( const GXNativeStaticMeshLoaderMemoryInspector &other ) = delete;
    GXNativeStaticMeshLoaderMemoryInspector& operator = ( const GXNativeStaticMeshLoaderMemoryInspector &other ) = delete;
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
    _vboData ( nullptr ),
    _eboData ( nullptr )
{
    // NOTHING
}

GXVoid GXNativeStaticMeshInfo::Cleanup ()
{
    gx_NativeMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_vboData ) );
    gx_NativeMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_eboData ) );
}

//----------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeStaticMesh ( const GXWChar* fileName, GXNativeStaticMeshInfo &info )
{
    GXUByte* data;
    GXUPointer size;

    GXFile file ( fileName );

    if ( !file.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_TRUE ) ) return;

    const GXNativeStaticMeshHeader* h = reinterpret_cast<const GXNativeStaticMeshHeader*> ( data );

    info._bounds = h->_bounds;

    info._numVertices = h->_numVertices;
    info._numUVs = h->_numUVs;
    info._numNormals = h->_numNormals;
    info._numTBPairs = h->_numTBPairs;

    size = info._numVertices * sizeof ( GXVec3 ) + info._numUVs * sizeof ( GXVec2 ) + info._numNormals * sizeof ( GXVec3 ) + info._numTBPairs * 2 * sizeof ( GXVec3 );
    info._vboData = static_cast<GXUByte*> ( gx_NativeMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._vboData, data + sizeof ( GXNativeStaticMeshHeader ), size );

    info._numElements = h->_numElements;

    if ( info._numElements < 1u ) return;

    size = info._numElements * sizeof ( GXUInt );
    info._eboData = static_cast<GXUByte*> ( gx_NativeMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._eboData, data + h->_elementOffset, size );
}
