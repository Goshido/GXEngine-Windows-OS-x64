//version 1.1

#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>

#define STB_IMAGE_IMPLEMENTATION

#define STBI_NO_SIMD // TODO: check if SIMD works on all ARM processors.
#define STBI_NO_STDIO
#define STBI_NO_FAILURE_STRINGS

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

#include <STB/stb_image.h>


GXBool GXCALL GXLoadImage ( const GXWChar* fileName, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXUByte** data )
{
	GXUInt len;
	stbi_uc* mappedFile;

	if ( !GXLoadFile ( fileName, (GXVoid**)&mappedFile, len, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadImage::Error - Can't load file %s\n", fileName );
		return GX_FALSE;
	}

	GXInt comp;
	*data = (GXUByte*)stbi_load_from_memory ( mappedFile, len, (GXInt*)&width, (GXInt*)&height, &comp, 0 );
	free ( mappedFile );

	if ( !( *data ) )
		return GX_FALSE;

	GXInt line = width * comp;
	GXUByte* tmp = (GXUByte*)malloc ( line );

	for ( GXInt i = height / 2; i > 0; i-- )
	{
		GXUByte* p = *data + i * line;
		GXUByte* n = *data + ( height - i - 1 ) * line;

		memcpy ( tmp, p, line );
		memcpy ( p, n, line );
		memcpy ( n, tmp, line );
	}

	free ( tmp );

	numChannels = comp;
	return GX_TRUE;
}
