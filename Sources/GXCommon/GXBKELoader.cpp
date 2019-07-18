//version 1.1

#include <GXCommon/GXBKELoader.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


GXVoid GXCALL GXLoadBKE ( const GXWChar* fileName, GXBakeInfo &out_bake_info )
{
    GXChar* buffer = nullptr;
    GXUPointer size = 0u;

    if ( !GXLoadFile ( fileName, (GXVoid**)&buffer, size, GX_TRUE ) )
        GXLogW ( L"GXLoadBKE::Error - Не могу загрузить файл %s", fileName );

    GXBakeHeader h;
    memcpy ( &h, buffer, sizeof ( GXBakeHeader ) );

    const GXUShort fileNameSize = static_cast<const GXUShort> ( sizeof ( GXWChar ) * ( wcslen ( reinterpret_cast<const GXWChar*> ( buffer + h._fileNameOffset ) ) + 1u ) );
    const GXUShort cacheFileNameSize = static_cast<const GXUShort> ( sizeof ( GXWChar ) * ( wcslen ( reinterpret_cast<const GXWChar*> ( buffer + h._cacheFileNameOffset ) ) + 1u ) );
    out_bake_info._fileName = static_cast<GXWChar*> ( malloc ( fileNameSize ) );
    memcpy ( out_bake_info._fileName, buffer + h._fileNameOffset, fileNameSize );
    out_bake_info._cacheFileName = static_cast<GXWChar*> ( malloc ( cacheFileNameSize ) );
    memcpy ( out_bake_info._cacheFileName, buffer + h._cacheFileNameOffset, cacheFileNameSize );

    GXSafeFree ( buffer );
}
