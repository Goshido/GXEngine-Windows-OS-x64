//version 1.2

#include <GXCommon/GXBKESaver.h>


GXVoid GXCALL GXExportBKE ( const GXWChar* fileName, const GXBakeInfo &info )
{
	GXUShort fileNameSize = (GXUShort)( sizeof ( GXWChar ) * ( wcslen ( info.fileName ) + 1 ) );
	GXUShort cacheFileNameSize = (GXUShort)( sizeof ( GXWChar ) * ( wcslen ( info.cacheFileName ) + 1 ) );
	GXUShort totalFileSize = (GXUShort)( sizeof ( GXBakeHeader ) + fileNameSize + cacheFileNameSize );

	GXChar* buffer = ( GXChar* )malloc ( totalFileSize );

	GXBakeHeader h;
	h.us_FileNameOffset = sizeof ( GXBakeHeader );
	h.us_CacheFileNameOffset = sizeof ( GXBakeHeader ) + fileNameSize;

	GXUShort offset = 0;
	memcpy ( buffer, &h, sizeof ( GXBakeHeader ) );

	offset += sizeof ( GXBakeHeader );
	memcpy ( buffer + offset, info.fileName, fileNameSize );

	offset += fileNameSize;
	memcpy ( buffer + offset, info.cacheFileName, cacheFileNameSize );

	GXWriteToFile ( fileName, buffer, totalFileSize );

	free ( buffer );
}