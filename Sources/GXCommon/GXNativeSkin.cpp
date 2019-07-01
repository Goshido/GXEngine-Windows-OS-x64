// version 1.3

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
    GXNativeSkinLoaderMemoryInspector ( const GXNativeSkinLoaderMemoryInspector &other ) = delete;
    GXNativeSkinLoaderMemoryInspector& operator = ( const GXNativeSkinLoaderMemoryInspector &other ) = delete;
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
    _totalVertices = 0u;
    _vboData = nullptr;
}

GXVoid GXSkinInfo::Cleanup ()
{
    gx_NativeSkinLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_vboData ) );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkin ( GXSkinInfo &info, const GXWChar* fileName )
{
    GXUByte* data;
    GXUPointer fileSize;

    GXFile file ( fileName );

    if ( !file.LoadContent ( data, fileSize, eGXFileContentOwner::GXFile, GX_TRUE ) ) return;

    GXNativeSkinHeader* h = reinterpret_cast<GXNativeSkinHeader*> ( data );

    info._totalVertices = h->_totalVertices;

    GXUPointer size = info._totalVertices * ( sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
    info._vboData = static_cast<GXFloat*> ( gx_NativeSkinLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._vboData, data + h->_vboOffset, size );
}
