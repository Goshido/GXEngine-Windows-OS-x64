// version 1.5

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


GX_DISABLE_COMMON_WARNINGS

#include <STB/stb_image.h>

GX_RESTORE_WARNING_STATE 


GXBool GXCALL GXLoadLDRImage ( const GXWChar* fileName, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXUByte** data )
{
	GXUPointer len;
	stbi_uc* mappedFile;

	if ( !GXLoadFile ( fileName, reinterpret_cast<GXVoid**> ( &mappedFile ), len, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadLDRImage::Error - Can't load file %s\n", fileName );
		return GX_FALSE;
	}

	GXInt comp;
	GXInt w;
	GXInt h;

	*data = static_cast<GXUByte*> ( stbi_load_from_memory ( mappedFile, static_cast<int>( len ), &w, &h, &comp, 0 ) );
	free ( mappedFile );

	if ( !( *data ) ) return GX_FALSE;

	width = static_cast<GXUInt> ( w );
	height = static_cast<GXUInt> ( h );

	GXUPointer lineSize = width * comp * sizeof ( GXUByte );
	GXUByte* tmp = static_cast<GXUByte*> ( malloc ( lineSize ) );
	GXUByte* p = *data;
	GXUByte* n = *data + lineSize * ( height - 1u );

	while ( p < n )
	{
		memcpy ( tmp, p, lineSize );
		memcpy ( p, n, lineSize );
		memcpy ( n, tmp, lineSize );

		p += lineSize;
		n -= lineSize;
	}

	free ( tmp );

	numChannels = static_cast<GXUByte> ( comp );
	return GX_TRUE;
}

GXBool GXCALL GXLoadHDRImage ( const GXWChar* fileName, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXFloat** data )
{
	GXUPointer len;
	stbi_uc* mappedFile;

	if ( !GXLoadFile ( fileName, reinterpret_cast<GXVoid**> ( &mappedFile ), len, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadHDRImage::Error - Can't load file %s\n", fileName );
		return GX_FALSE;
	}

	GXInt comp;
	GXInt w;
	GXInt h;

	*data = static_cast<GXFloat*> ( stbi_loadf_from_memory ( mappedFile, static_cast<int>( len ), &w, &h, &comp, 0 ) );
	free ( mappedFile );

	if ( !( *data ) ) return GX_FALSE;

	width = static_cast<GXUInt> ( w );
	height = static_cast<GXUInt> ( h );

	GXUPointer floatsPerLine = width * comp;
	GXUPointer lineSize = floatsPerLine * sizeof ( GXFloat );
	GXFloat* tmp = static_cast<GXFloat*> ( malloc ( lineSize ) );
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

	numChannels = static_cast<GXUByte> ( comp );
	return GX_TRUE;
}
