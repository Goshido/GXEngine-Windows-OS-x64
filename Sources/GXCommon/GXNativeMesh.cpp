// vesrion 1.2

#include <GXCommon/GXNativeMesh.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


class GXNativeMeshLoaderMemoryInspector final : public GXMemoryInspector
{
public:
    GXNativeMeshLoaderMemoryInspector ();
    ~GXNativeMeshLoaderMemoryInspector () override;

private:
    GXNativeMeshLoaderMemoryInspector ( const GXNativeMeshLoaderMemoryInspector &other );
    GXNativeMeshLoaderMemoryInspector& operator = ( const GXNativeMeshLoaderMemoryInspector &other );
};

GXNativeMeshLoaderMemoryInspector::GXNativeMeshLoaderMemoryInspector ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXNativeMeshLoaderMemoryInspector" )
{
    // NOTHING
}

GXNativeMeshLoaderMemoryInspector::~GXNativeMeshLoaderMemoryInspector ()
{
    // NOTHING
}

static GXNativeMeshLoaderMemoryInspector gx_NativeMeshLoaderMemoryInspector;

//-------------------------------------------------------------------------------------------------------------

GXMeshInfo::GXMeshInfo ():
    totalVertices ( 0u ),
    vboData ( nullptr )
{
    // NOTHING
}

GXVoid GXMeshInfo::Cleanup ()
{
    gx_NativeMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &vboData ) );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeMesh ( GXMeshInfo &info, const GXWChar* fileName )
{
    GXUByte* data;
    GXUPointer fileSize;

    GXFile file ( fileName );

    if ( !file.LoadContent ( data, fileSize, eGXFileContentOwner::GXFile, GX_TRUE ) ) return;

    GXNativeMeshHeader* h = reinterpret_cast<GXNativeMeshHeader*> ( data );
    info.totalVertices = h->totalVertices;

    GXUPointer size = info.totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) );
    info.vboData = static_cast<GXFloat*> ( gx_NativeMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.vboData, data + h->vboOffset, size );
}
