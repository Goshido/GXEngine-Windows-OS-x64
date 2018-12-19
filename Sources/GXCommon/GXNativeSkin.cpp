// version 1.1

#include <GXCommon/GXNativeSkin.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


GXSkinInfo::GXSkinInfo ()
{
    totalVertices = 0u;
    vboData = nullptr;
}

GXVoid GXSkinInfo::Cleanup ()
{
    GXSafeFree ( vboData );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkin ( GXSkinInfo &info, const GXWChar* fileName )
{
    GXUByte* data;
    GXUBigInt fileSize;

    if ( !GXLoadFile ( fileName, reinterpret_cast<GXVoid**> ( &data ), fileSize, GX_TRUE ) )
    {
        GXLogW ( L"GXLoadNativeSkin::Error - Can't load file %s\n", fileName );
        return;
    }

    GXNativeSkinHeader* h = reinterpret_cast<GXNativeSkinHeader*> ( data );

    info.totalVertices = h->totalVertices;

    GXUPointer size = info.totalVertices * ( sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
    info.vboData = static_cast<GXFloat*> ( malloc ( size ) );
    memcpy ( info.vboData, data + h->vboOffset, size );

    free ( data );
}
