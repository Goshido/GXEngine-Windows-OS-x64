//version 1.1

#include <GXCommon/GXBKELoader.h>
#include <GXCommon/GXFileSystem.h>


GXVoid GXCALL GXLoadBKE ( const GXWChar* fileName, GXBakeInfo &out_bake_info )
{
	GXChar* buffer = 0;
	GXUInt size;

	if ( !GXLoadFile ( fileName, (GXVoid**)&buffer, size, GX_TRUE ) )
	{
		GXDebugBox ( L"GXLoadBKE::Error - Не могу загрузить файл" );
		GXLogW ( L"GXLoadBKE::Error - Не могу загрузить файл %s", fileName );
	}

	GXBakeHeader h;
	memcpy ( &h, buffer, sizeof ( GXBakeHeader ) );
	GXUShort fileNameSize = (GXUShort)( sizeof ( GXWChar ) * ( wcslen ( (GXWChar*)( buffer + h.us_FileNameOffset ) ) + 1 ) );
	GXUShort cacheFileNameSize = (GXUShort)( sizeof ( GXWChar ) * ( wcslen ( (GXWChar*)( buffer + h.us_CacheFileNameOffset ) ) + 1 ) );
	out_bake_info.fileName = (GXWChar*)malloc ( fileNameSize );
	memcpy ( out_bake_info.fileName, buffer + h.us_FileNameOffset, fileNameSize );
	out_bake_info.cacheFileName = (GXWChar*)malloc ( cacheFileNameSize );
	memcpy ( out_bake_info.cacheFileName, buffer + h.us_CacheFileNameOffset, cacheFileNameSize );

	GXSafeFree ( buffer );
}