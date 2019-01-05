// version 1.2

#include <GXCommon/GXNativeSkin.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


class GXNativeSkinLoaderMemoryInspector final : public GXMemoryInspector
{
public:
    GXNativeSkinLoaderMemoryInspector ();
    ~GXNativeSkinLoaderMemoryInspector () override;

private:
    GXNativeSkinLoaderMemoryInspector ( const GXNativeSkinLoaderMemoryInspector &other );
    GXNativeSkinLoaderMemoryInspector& operator = ( const GXNativeSkinLoaderMemoryInspector &other );
};

GXNativeSkinLoaderMemoryInspector::GXNativeSkinLoaderMemoryInspector ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXNativeSkinLoaderMemoryInspector" )
{
    // NOTHING
}

GXNativeSkinLoaderMemoryInspector::~GXNativeSkinLoaderMemoryInspector ()
{
    // NOTHING
}

static GXNativeSkinLoaderMemoryInspector gx_NativeSkinLoaderMemoryInspector;

//-------------------------------------------------------------------------------------------------------------

GXSkinInfo::GXSkinInfo ()
{
    totalVertices = 0u;
    vboData = nullptr;
}

GXVoid GXSkinInfo::Cleanup ()
{
    gx_NativeSkinLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &vboData ) );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkin ( GXSkinInfo &info, const GXWChar* fileName )
{
    GXUByte* data;
    GXUPointer fileSize;

    GXFile file ( fileName );

    if ( !file.LoadContent ( data, fileSize, eGXFileContentOwner::GXFile, GX_TRUE ) ) return;

    GXNativeSkinHeader* h = reinterpret_cast<GXNativeSkinHeader*> ( data );

    info.totalVertices = h->totalVertices;

    GXUPointer size = info.totalVertices * ( sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
    info.vboData = static_cast<GXFloat*> ( gx_NativeSkinLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.vboData, data + h->vboOffset, size );
}
