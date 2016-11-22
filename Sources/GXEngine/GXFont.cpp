//version 1.3

#include <GXEngine/GXFont.h>
#include <GXEngine/GXTextureUtils.h>
#include <GXEngine/GXFont.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>


#define GX_ATLAS_UNDEFINED		-1
#define GX_ATLAS_NEW_LINE		0
#define GX_ATLAS_NEW_TEXTURE	1
#define GX_ATLAS_FILL			2
#define GX_ATLAS_RESOLUTION		512
#define GX_ATLAS_ONE_PIXEL		0.0019531f	//1 pixel in uv coordinate system
#define GX_ATLAS_SPACING		2


PFNFTNEWMEMORYFACE		GXFtNewMemoryFace			= 0;
PFNFTDONEFACE			GXFtDoneFace				= 0;

PFNFTSETCHARSIZE		GXFtSetCharSize				= 0;
PFNFTSETPIXELSIZES		GXFtSetPixelSizes			= 0;

PFNFTGETCHARINDEX		GXFtGetCharIndex			= 0;
PFNFTGETKERNING			GXFtGetKerning				= 0;

PFNFTLOADGLYPH			GXFtLoadGlyph				= 0;
PFNFTRENDERGLYPH		GXFtRenderGlyph				= 0;

PFNGXFREETYPEDESTROY	GXFreeTypeDestroy			= 0;

//-------------------------------------------------------------------------

extern HMODULE gx_GXEngineDLLModuleHandle;

FT_Library	gx_ft_Library;


GXBool GXCALL GXFontInit ()
{
	gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXFontInit::Error - Не удалось загрузить GXEngine.dll" );
		return GX_FALSE;
	}

	PFNGXFREETYPEINIT GXFreeTypeInit;
	GXFreeTypeInit = ( PFNGXFREETYPEINIT )GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXFreeTypeInit" );
	if ( !GXFreeTypeInit )
	{
		GXLogW ( L"GXFontInit::Error - Не удалось найти функцию GXFreeTypeInit" );
		return GX_FALSE;
	}

	GXFreeTypeFunctions out;
	out.FT_Done_Face = &GXFtDoneFace;
	out.FT_Get_Char_Index = &GXFtGetCharIndex;
	out.FT_Get_Kerning = &GXFtGetKerning;
	out.FT_Load_Glyph = &GXFtLoadGlyph;
	out.FT_New_Memory_Face = &GXFtNewMemoryFace;
	out.FT_Render_Glyph = &GXFtRenderGlyph;
	out.FT_Set_Char_Size = &GXFtSetCharSize;
	out.FT_Set_Pixel_Sizes = &GXFtSetPixelSizes;
	out.GXFreeTypeDestroy = &GXFreeTypeDestroy;

	return GXFreeTypeInit ( out, gx_ft_Library );
}

GXBool GXCALL GXFontDestroy ()
{
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXFontDestroy::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll" );
		return GX_FALSE;
	}

	GXFreeTypeDestroy ();

	if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
	{
		GXLogW ( L"GXFontDestroy::Error - Не удалось выгрузить библиотеку GXEngine.dll" );
		return GX_FALSE;
	}

	return GX_TRUE;
}

//---------------------------------------------------------------------------------------------------

GXFont::GXFont ( FT_Face face, GXUShort size )
{
	this->face = face;
	this->size = size;

	left = bottom = top = 0;

	for ( GXUShort i = 0; i < 0x7FFF; i++ )
		glyphs[ i ].atlasID = GX_ATLAS_UNDEFINED;

	atlases = 0;
	atlasID = GX_ATLAS_UNDEFINED;

	GXUShort temp =  (GXUShort)( size * 0.5f );
	spaceAdvance = temp == 0 ? 1 : temp;
}

GXFont::~GXFont ()
{
	if ( atlasID == GX_ATLAS_UNDEFINED )
		return;

	glDeleteTextures ( atlasID + 1, atlases );

	free ( atlases );
}

GXBool GXFont::GetGlyph ( GXUInt symbol, GXGlyphInfo &info )
{
	if ( symbol > 0x7FFF )
		return GX_FALSE;

	if ( glyphs[ symbol ].atlasID != GX_ATLAS_UNDEFINED )
	{
		info.atlas = atlases[ glyphs[ symbol ].atlasID ];
		memcpy ( &info.min, &glyphs[ symbol ].min, sizeof ( GXVec2 ) );
		memcpy ( &info.max, &glyphs[ symbol ].max, sizeof ( GXVec2 ) );
		info.offsetY = glyphs[ symbol ].offsetY;
		info.width = ( glyphs[ symbol ].max.x - glyphs[ symbol ].min.x ) * GX_ATLAS_RESOLUTION;
		info.height = ( glyphs[ symbol ].max.y - glyphs[ symbol ].min.y ) * GX_ATLAS_RESOLUTION;
		info.advance = glyphs[ symbol ].advance;

		return GX_TRUE;
	}

	RenderGlyph ( symbol );

	info.atlas = atlases[ glyphs[ symbol ].atlasID ];
	memcpy ( &info.min, &glyphs[ symbol ].min, sizeof ( GXVec2 ) );
	memcpy ( &info.max, &glyphs[ symbol ].max, sizeof ( GXVec2 ) );
	info.offsetY = glyphs[ symbol ].offsetY;
	info.width = ( glyphs[ symbol ].max.x - glyphs[ symbol ].min.x ) * GX_ATLAS_RESOLUTION;
	info.height = ( glyphs[ symbol ].max.y - glyphs[ symbol ].min.y ) * GX_ATLAS_RESOLUTION;
	info.advance = glyphs[ symbol ].advance;

	return GX_TRUE;
}

GXInt GXFont::GetKerning ( GXUInt symbol, GXUInt prevSymbol )
{
	FT_UInt glyphIndex = GXFtGetCharIndex ( face, symbol );
	FT_UInt prevGlyphIndex = GXFtGetCharIndex ( face, prevSymbol );

	if ( FT_HAS_KERNING ( face ) )
	{
		FT_Vector delta;
		GXFtGetKerning ( face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta );
		return (GXInt)( delta.x >> 6 );
	}

	return 0;
}

GXUShort GXFont::GetSpaceAdvance ()
{
	return spaceAdvance;
}

GXUShort GXFont::GetSize ()
{
	return size;
}

GXVoid GXFont::GetAtlasTexture (  GXByte atlasID, GXTexture &texture )
{
	if ( atlasID > this->atlasID )
	{
		GXLogW ( L"GXFont::GetAtlasTexture::Error - Wrong atlas ID\n" );
		return;
	}

	texture.channels = 1;
	texture.width = texture.height = GX_ATLAS_RESOLUTION;
	texture.texObj = atlases[ atlasID ];
}

GXUInt GXCDECLCALL GXFont::GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, ... )
{
	GXInt penX = 0;

	GXWChar* text = 0;

	if ( bufferNumSymbols )
	{
		GXWChar* temp = (GXWChar*)malloc ( bufferNumSymbols * sizeof ( GXWChar ) );

		va_list ap;
		va_start ( ap, format );
		vswprintf_s ( temp, bufferNumSymbols, format, ap );
		va_end ( ap );

		text = temp;
	}
	else
		text = (GXWChar*)format;

	GXUInt len = GXWcslen ( text );

	GXUInt prevSymbol = 0;
	GXGlyphInfo info;

	for ( GXUInt i = 0; i < len; i++ )
	{
		GXUInt symbol = (GXUInt)text[ i ];

		if ( symbol == ' ' )
		{
			penX += GetSpaceAdvance ();
			continue;
		}

		GetGlyph ( symbol, info );

		if ( prevSymbol != 0 )
			penX += GetKerning ( symbol, prevSymbol );

		prevSymbol = symbol;

		penX += info.advance;
	}

	if ( bufferNumSymbols )
		free ( text );

	return penX;
}

GXVoid GXFont::RenderGlyph ( GXUInt symbol )
{
	if ( GXFtSetPixelSizes ( face, size, size ) )
		GXLogA ( "GXFont::RenderGlyph::Error - Can't set font size %i\n", size );

	FT_UInt glyphIndex = GXFtGetCharIndex ( face, symbol );

	if ( GXFtLoadGlyph ( face, glyphIndex, FT_LOAD_RENDER ) )
		GXLogA ( "GXFont::RenderGlyph::Error - FT_Load_Glyph failed at symbol #%i\n", symbol );

	if ( GXFtRenderGlyph ( face->glyph, FT_RENDER_MODE_NORMAL ) )
		GXLogA ( "GXFont::RenderGlyph::Error - FT_Render_Glyph failed at symbol #%i\n", symbol );

	glyphs[ symbol ].offsetY = (GXFloat)( ( face->glyph->metrics.horiBearingY - face->glyph->metrics.height ) >> 6 );
	glyphs[ symbol ].advance = (GXUShort)( face->glyph->advance.x >> 6 );

	FT_Bitmap bitmap = face->glyph->bitmap;

	switch ( CheckAtlas ( bitmap.width, bitmap.rows ) )
	{
		case GX_ATLAS_NEW_LINE:
			left = 0;
			bottom = top;
			top = bottom;
		break;

		case GX_ATLAS_NEW_TEXTURE:
			CreateAtlas ();

			left = 0;
			bottom = 0;
			top = 0;
		break;
	}

	glyphs[ symbol ].min = GXCreateVec2 ( ( left ) * GX_ATLAS_ONE_PIXEL, ( bottom ) * GX_ATLAS_ONE_PIXEL );
	glyphs[ symbol ].max = GXCreateVec2 ( ( left + bitmap.width + 1 ) * GX_ATLAS_ONE_PIXEL, ( bottom + bitmap.rows ) * GX_ATLAS_ONE_PIXEL );

	glyphs[ symbol ].atlasID = atlasID;

	GXUByte* buffer = (GXUByte*)malloc ( bitmap.width * bitmap.rows );

	for ( GXUInt h = 0; h < (GXUInt)bitmap.rows; h ++ )
		for ( GXUInt w = 0; w < (GXUInt)bitmap.width; w ++ )
			buffer[ h * bitmap.width + w ] = bitmap.buffer[ ( bitmap.rows - 1 - h ) * bitmap.width + w ];

	glBindTexture ( GL_TEXTURE_2D, atlases[ atlasID ] );
	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
	glTexSubImage2D ( GL_TEXTURE_2D, 0, left, bottom, bitmap.width, bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, buffer );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	free ( buffer );

	left += bitmap.width + GX_ATLAS_SPACING;

	if ( top < ( bottom + bitmap.rows + GX_ATLAS_SPACING ) )
		top = bottom + bitmap.rows + GX_ATLAS_SPACING;
}

GXVoid GXFont::CreateAtlas ()
{
	if ( atlasID == GX_ATLAS_UNDEFINED )
	{
		atlasID = 0;
		atlases = (GLuint*)malloc ( sizeof ( GLuint ) );
	}
	else
	{
		GLuint* temp = (GLuint*)malloc ( ( atlasID + 2 ) * sizeof ( GLuint ) );
		for ( GXUShort i = 0; i < atlasID; i++ )
			temp[ i ] = atlases[ i ];

		atlasID++;
		free ( atlases );
		atlases = temp;
	}

	GXUInt size = GX_ATLAS_RESOLUTION * GX_ATLAS_RESOLUTION;
	GXUByte* data = (GXUByte*)malloc ( size );

	memset ( data, 0, size );

	GXGLTextureStruct ts;
	ts.format = GL_RED;
	ts.internalFormat = GL_R8;
	ts.type = GL_UNSIGNED_BYTE;
	ts.width = GX_ATLAS_RESOLUTION;
	ts.height = GX_ATLAS_RESOLUTION;
	ts.wrap = GL_CLAMP_TO_EDGE;
	ts.anisotropy = 1;
	ts.resampling = GX_TEXTURE_RESAMPLING_NONE;
	ts.data = data;

	atlases[ atlasID ] = GXCreateTexture ( ts );

	free ( data );
}

GXUByte GXFont::CheckAtlas ( GXUInt width, GXUInt height )
{
	if ( atlasID == GX_ATLAS_UNDEFINED )
		return GX_ATLAS_NEW_TEXTURE;

	if ( ( left + width ) >= GX_ATLAS_RESOLUTION )
	{
		if ( ( bottom + height ) >= GX_ATLAS_RESOLUTION )
			return GX_ATLAS_NEW_TEXTURE;
		else
			return GX_ATLAS_NEW_LINE;
	}

	if ( ( bottom + height ) >= GX_ATLAS_RESOLUTION )
		return GX_ATLAS_NEW_TEXTURE;

	return GX_ATLAS_FILL;
}
