//version 1.3

#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>


#define STB_IMAGE_IMPLEMENTATION

#define STBI_NO_SIMD
#define STBI_NO_STDIO
#define STBI_NO_FAILURE_STRINGS

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA
#define STBI_ONLY_HDR


#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <STB/stb_image.h>
#include <GXCommon/GXRestoreWarnings.h>


GXBool GXCALL GXLoadLDRImage ( const GXWChar* fileName, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXUByte** data )
{
	GXUPointer len;
	stbi_uc* mappedFile;

	if ( !GXLoadFile ( fileName, (GXVoid**)&mappedFile, len, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadLDRImage::Error - Can't load file %s\n", fileName );
		return GX_FALSE;
	}

	GXInt comp;
	*data = (GXUByte*)stbi_load_from_memory ( mappedFile, (int)len, (GXInt*)&width, (GXInt*)&height, &comp, 0 );
	free ( mappedFile );

	if ( !( *data ) )
		return GX_FALSE;

	GXUPointer lineSize = width * comp * sizeof ( GXUByte );
	GXUByte* tmp = (GXUByte*)malloc ( lineSize );
	GXUByte* p = *data;
	GXUByte* n = *data + lineSize * ( height - 1 );

	while ( p < n )
	{
		memcpy ( tmp, p, lineSize );
		memcpy ( p, n, lineSize );
		memcpy ( n, tmp, lineSize );

		p += lineSize;
		n -= lineSize;
	}

	free ( tmp );

	numChannels = (GXUByte)comp;
	return GX_TRUE;
}

GXBool GXCALL GXLoadHDRImage ( const GXWChar* fileName, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXFloat** data )
{
	GXUPointer len;
	stbi_uc* mappedFile;

	if ( !GXLoadFile ( fileName, (GXVoid**)&mappedFile, len, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadHDRImage::Error - Can't load file %s\n", fileName );
		return GX_FALSE;
	}

	GXInt comp;
	*data = (GXFloat*)stbi_loadf_from_memory ( mappedFile, (int)len, (GXInt*)&width, (GXInt*)&height, &comp, 0 );
	free ( mappedFile );

	if ( !( *data ) )
		return GX_FALSE;

	GXUPointer floatsPerLine = width * comp;
	GXUPointer lineSize = floatsPerLine * sizeof ( GXFloat );
	GXFloat* tmp = (GXFloat*)malloc ( lineSize );
	GXFloat* p = *data;
	GXFloat* n = *data + floatsPerLine * ( height - 1 );

	while ( p < n )
	{
		memcpy ( tmp, p, lineSize );
		memcpy ( p, n, lineSize );
		memcpy ( n, tmp, lineSize );

		p += floatsPerLine;
		n -= floatsPerLine;
	}

	free ( tmp );

	numChannels = (GXUByte)comp;
	return GX_TRUE;
}
