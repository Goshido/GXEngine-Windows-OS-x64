//version 1.2

#include <GXCommon/GXBKESaver.h>


GXVoid GXCALL GXExportBKE ( const GXWChar* fileName, const GXBakeInfo &info )
{
    const GXUShort fileNameSize = static_cast<const GXUShort> ( sizeof ( GXWChar ) * ( wcslen ( info._fileName ) + 1u ) );
    const GXUShort cacheFileNameSize = static_cast<const GXUShort> ( sizeof ( GXWChar ) * ( wcslen ( info._cacheFileName ) + 1u ) );
    const GXUShort totalFileSize = static_cast<const GXUShort> ( sizeof ( GXBakeHeader ) + fileNameSize + cacheFileNameSize );

    GXChar* buffer = static_cast<GXChar*> ( malloc ( totalFileSize ) );

    GXBakeHeader h;
    h._fileNameOffset = sizeof ( GXBakeHeader );
    h._cacheFileNameOffset = sizeof ( GXBakeHeader ) + fileNameSize;

    GXUShort offset = 0;
    memcpy ( buffer, &h, sizeof ( GXBakeHeader ) );

    offset += sizeof ( GXBakeHeader );
    memcpy ( buffer + offset, info._fileName, fileNameSize );

    offset += fileNameSize;
    memcpy ( buffer + offset, info._cacheFileName, cacheFileNameSize );

    GXWriteToFile ( fileName, buffer, totalFileSize );

    free ( buffer );
}
