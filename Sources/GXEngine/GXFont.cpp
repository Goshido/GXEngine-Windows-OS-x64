// version 1.8

#include <GXEngine/GXFont.h>
#include <GXEngineDLL/GXEngineAPI.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>


#define ATLAS_UNDEFINED		-1
#define ATLAS_NEW_LINE		0u
#define ATLAS_NEW_TEXTURE	1u
#define ATLAS_FILL			2u
#define ATLAS_RESOLUTION	512u
#define ATLAS_ONE_PIXEL		0.0019531f	// 1 pixel in uv coordinate system
#define ATLAS_SPACING		2u

#define MAXIMUM_GLYPHS		0x7FFFu

//---------------------------------------------------------------------------------------------------------------------

extern HMODULE gx_GXEngineDLLModuleHandle;

static FT_Library				gx_ft_Library				= nullptr;

static PFNFTNEWMEMORYFACE		GXFtNewMemoryFace			= nullptr;
static PFNFTDONEFACE			GXFtDoneFace				= nullptr;

static PFNFTSETCHARSIZE			GXFtSetCharSize				= nullptr;
static PFNFTSETPIXELSIZES		GXFtSetPixelSizes			= nullptr;

static PFNFTGETCHARINDEX		GXFtGetCharIndex			= nullptr;
static PFNFTGETKERNING			GXFtGetKerning				= nullptr;

static PFNFTLOADGLYPH			GXFtLoadGlyph				= nullptr;
static PFNFTRENDERGLYPH			GXFtRenderGlyph				= nullptr;

static PFNGXFREETYPEDESTROY		GXFreeTypeDestroy			= nullptr;

static GXFontEntry*				gx_FontEntries				= nullptr;

struct GXGlyph
{
	GXVec2		min;
	GXVec2		max;
	GXFloat		offsetY;
	GXUShort	advance;
	GXByte		atlasID;
};

class GXFontEntry
{
	friend class GXFont;

	private:
		GXFontEntry*	prev;
		GXFontEntry*	next;
		GXInt			refs;

		GXTexture2D*	atlases;
		GXByte			lastAtlasID;
		GXUShort		left;
		GXUShort		top;
		GXUShort		bottom;

		GXUShort		size;

		FT_Face			face;
		GXWChar*		fileName;
		GXUShort		spaceAdvance;

		GXGlyph			glyphs[ MAXIMUM_GLYPHS ];
		GXVoid*			mappedFile;

	public:
		explicit GXFontEntry ( const GXWChar* fileName, GXUShort size );

		const GXWChar* GetFileName () const;
		GXUShort GetSize () const;

		GXBool GetGlyph ( GXUInt symbol, GXGlyphInfo &info );
		GXInt GetKerning ( GXUInt symbol, GXUInt prevSymbol ) const;
		GXUShort GetSpaceAdvance () const;
		GXTexture2D* GetAtlasTexture ( GXByte atlasID );

		GXUInt GXCDECLCALL GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, va_list parameters );

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXFontEntry ();

		GXVoid RenderGlyph ( GXUInt symbol );
		GXUByte CheckAtlas ( GXUInt width, GXUInt heigth ) const;
		GXVoid CreateAtlas ();

		GXFontEntry () = delete;
		GXFontEntry ( const GXFontEntry &other ) = delete;
		GXFontEntry& operator = ( const GXFontEntry &other ) = delete;
};

GXFontEntry::GXFontEntry ( const GXWChar* fileName, GXUShort size ):
	prev ( nullptr ),
	next ( gx_FontEntries ),
	refs ( 1 ),
	atlases ( nullptr ),
	lastAtlasID ( ATLAS_UNDEFINED ),
	left ( 0u ),
	bottom ( 0u ),
	top ( 0u ),
	size ( size )
{
	GXWcsclone ( &this->fileName, fileName );

	if ( gx_FontEntries )
		gx_FontEntries->prev = this;

	gx_FontEntries = this;

	for ( GXUShort i = 0u; i < MAXIMUM_GLYPHS; i++ )
		glyphs[ i ].atlasID = ATLAS_UNDEFINED;

	GXUShort temp = static_cast<GXUShort> ( size * 0.5f );
	spaceAdvance = temp == 0u ? 1u : temp;
	GXUPointer totalSize = 0u;

	if ( !GXLoadFile ( fileName, &mappedFile, totalSize, GX_TRUE ) )
	{
		GXLogW ( L"GXFontEntry::GXFontEntry::Error - не могу загрузить файл шрифта %s\n", fileName );
		face = nullptr;

		return;
	}

	if ( !GXFtNewMemoryFace ( gx_ft_Library, static_cast<FT_Byte*> ( mappedFile ), static_cast<FT_Long> ( totalSize ), 0, &face ) ) return;

	GXLogW ( L"GXFontEntry::GXFontEntry::Error - GXFtNewMemoryFace выполнилась с ошибкой для шрифта %s failed\n", fileName );
	free ( mappedFile );
	face = nullptr;
}

const GXWChar* GXFontEntry::GetFileName () const
{
	return fileName;
}

GXUShort GXFontEntry::GetSize () const
{
	return size;
}

GXBool GXFontEntry::GetGlyph ( GXUInt symbol, GXGlyphInfo &info )
{
	if ( symbol > MAXIMUM_GLYPHS ) return GX_FALSE;

	const GXGlyph& glyph = glyphs[ symbol ];

	if ( glyphs[ symbol ].atlasID != ATLAS_UNDEFINED )
	{
		info.atlas = atlases + glyph.atlasID;
		info.min = glyph.min;
		info.max = glyph.max;
		info.offsetY = glyph.offsetY;
		info.width = ( glyph.max.GetX () - glyph.min.GetX () ) * ATLAS_RESOLUTION;
		info.height = ( glyph.max.GetY () - glyph.min.GetY () ) * ATLAS_RESOLUTION;
		info.advance = glyph.advance;

		return GX_TRUE;
	}

	RenderGlyph ( symbol );

	info.atlas = atlases + glyph.atlasID;
	info.min = glyph.min;
	info.max = glyph.max;
	info.offsetY = glyph.offsetY;
	info.width = ( glyph.max.GetX () - glyph.min.GetX () ) * ATLAS_RESOLUTION;
	info.height = ( glyph.max.GetY () - glyph.min.GetY () ) * ATLAS_RESOLUTION;
	info.advance = glyph.advance;

	return GX_TRUE;
}

GXInt GXFontEntry::GetKerning ( GXUInt symbol, GXUInt prevSymbol ) const
{
	FT_UInt glyphIndex = GXFtGetCharIndex ( face, symbol );
	FT_UInt prevGlyphIndex = GXFtGetCharIndex ( face, prevSymbol );

	if ( !FT_HAS_KERNING ( face ) ) return 0;

	FT_Vector delta;
	GXFtGetKerning ( face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta );
	return static_cast<GXInt> ( delta.x >> 6 );
}

GXUShort GXFontEntry::GetSpaceAdvance () const
{
	return spaceAdvance;
}

GXTexture2D* GXFontEntry::GetAtlasTexture ( GXByte atlasID )
{
	if ( atlasID > atlasID )
	{
		GXLogW ( L"GXFontEntry::GetAtlasTexture::Error - Wrong atlas ID\n" );
		return nullptr;
	}

	return atlases + atlasID;
}

GXUInt GXCDECLCALL GXFontEntry::GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, va_list parameters )
{
	GXInt penX = 0;

	GXWChar* text = nullptr;

	if ( bufferNumSymbols )
	{
		GXWChar* temp = static_cast<GXWChar*> ( malloc ( bufferNumSymbols * sizeof ( GXWChar ) ) );
		vswprintf_s ( temp, bufferNumSymbols, format, parameters );

		text = temp;
	}
	else
	{
		text = const_cast<GXWChar*> ( format );
	}

	GXUInt len = GXWcslen ( text );

	GXUInt prevSymbol = 0u;
	GXGlyphInfo info;

	for ( GXUInt i = 0u; i < len; i++ )
	{
		GXUInt symbol = static_cast<GXUInt> ( text[ i ] );

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

	return static_cast<GXUInt> ( penX );
}

GXVoid GXFontEntry::AddRef ()
{
	refs++;
}

GXVoid GXFontEntry::Release ()
{
	refs--;

	if ( refs > 0 ) return;

	delete this;
}

GXFontEntry::~GXFontEntry ()
{
	if ( GXFtDoneFace ( face ) )
		GXLogW ( L"GXFontEntry::~GXFontEntry::Error - GXFtDoneFace выполнилась с ошибкой\n" );

	if ( lastAtlasID != ATLAS_UNDEFINED )
	{
		for ( GXByte i = 0; i <= lastAtlasID; i++ )
			atlases[ i ].FreeResources ();

		free ( atlases );
	}

	free ( mappedFile );

	GXSafeFree ( fileName );

	if ( gx_FontEntries == this )
		gx_FontEntries = gx_FontEntries->next;
	else
		prev->next = next;

	if ( next )
		next->prev = prev;
}

GXVoid GXFontEntry::RenderGlyph ( GXUInt symbol )
{
	if ( GXFtSetPixelSizes ( face, size, size ) )
		GXLogA ( "GXFontEntry::RenderGlyph::Error - Can't set font size %i\n", size );

	FT_UInt glyphIndex = GXFtGetCharIndex ( face, symbol );

	if ( GXFtLoadGlyph ( face, glyphIndex, FT_LOAD_RENDER ) )
		GXLogA ( "GXFontEntry::RenderGlyph::Error - FT_Load_Glyph failed at symbol #%i\n", symbol );

	if ( GXFtRenderGlyph ( face->glyph, FT_RENDER_MODE_NORMAL ) )
		GXLogA ( "GXFontEntry::RenderGlyph::Error - FT_Render_Glyph failed at symbol #%i\n", symbol );

	GXGlyph& glyph = glyphs[ symbol ];

	glyph.offsetY = static_cast<GXFloat> ( ( face->glyph->metrics.horiBearingY - face->glyph->metrics.height ) >> 6 );
	glyph.advance = static_cast<GXUShort> ( face->glyph->advance.x >> 6 );

	FT_Bitmap bitmap = face->glyph->bitmap;

	switch ( CheckAtlas ( static_cast<GXUInt> ( bitmap.width ), static_cast<GXUInt> ( bitmap.rows ) ) )
	{
		case ATLAS_NEW_LINE:
			left = 0u;
			bottom = top;
			top = bottom;
		break;

		case ATLAS_NEW_TEXTURE:
			CreateAtlas ();

			left = 0u;
			bottom = 0u;
			top = 0u;
		break;

		default:
			// NOTHING
		break;
	}

	glyph.min.Init ( left * ATLAS_ONE_PIXEL, bottom * ATLAS_ONE_PIXEL );
	glyph.max.Init ( ( left + bitmap.width + 1 ) * ATLAS_ONE_PIXEL, ( bottom + bitmap.rows ) * ATLAS_ONE_PIXEL );

	glyph.atlasID = lastAtlasID;

	GXUByte* buffer = static_cast<GXUByte*> ( malloc ( static_cast<GXUPointer> ( bitmap.width * bitmap.rows ) ) );

	for ( GXUInt h = 0u; h < static_cast<GXUInt> ( bitmap.rows ); h++ )
	{
		for ( GXUInt w = 0u; w < static_cast<GXUInt> ( bitmap.width ); w++ )
			buffer[ h * bitmap.width + w ] = bitmap.buffer[ ( bitmap.rows - 1 - h ) * bitmap.width + w ];
	}

	atlases[ lastAtlasID ].FillRegionPixelData ( left, bottom, static_cast<GXUShort> ( bitmap.width ), static_cast<GXUShort> ( bitmap.rows ), buffer );

	free ( buffer );

	left += static_cast<GXUShort> ( bitmap.width + ATLAS_SPACING );

	if ( top >= ( bottom + bitmap.rows + ATLAS_SPACING ) ) return;

	top = static_cast<GXUShort> ( bottom + static_cast<GXUShort> ( bitmap.rows ) + ATLAS_SPACING );
}

GXUByte GXFontEntry::CheckAtlas ( GXUInt width, GXUInt height ) const
{
	if ( lastAtlasID == ATLAS_UNDEFINED )
		return ATLAS_NEW_TEXTURE;

	if ( ( left + width ) >= ATLAS_RESOLUTION )
	{
		if ( ( bottom + height ) >= ATLAS_RESOLUTION )
			return ATLAS_NEW_TEXTURE;
		else
			return ATLAS_NEW_LINE;
	}

	if ( ( bottom + height ) >= ATLAS_RESOLUTION )
		return ATLAS_NEW_TEXTURE;

	return ATLAS_FILL;
}

GXVoid GXFontEntry::CreateAtlas ()
{
	if ( lastAtlasID == ATLAS_UNDEFINED )
	{
		lastAtlasID = 0;
		atlases = static_cast<GXTexture2D*> ( malloc ( sizeof ( GXTexture2D ) ) );
	}
	else
	{
		GXTexture2D* temp = static_cast<GXTexture2D*> ( malloc ( ( lastAtlasID + 2 ) * sizeof ( GXTexture2D ) ) );

		for ( GXByte i = 0u; i < lastAtlasID; i++ )
			temp[ i ] = atlases[ i ];

		lastAtlasID++;
		free ( atlases );
		atlases = temp;
	}

	atlases[ lastAtlasID ].InitResources ( ATLAS_RESOLUTION, ATLAS_RESOLUTION, GL_R8, GX_FALSE, GL_CLAMP_TO_EDGE );
	atlases[ lastAtlasID ].FillWholePixelData ( nullptr );
}

//---------------------------------------------------------------------------------------------------------------------

GXFont::GXFont ( const GXWChar* fileName, GXUShort size )
{
	for ( GXFontEntry* p = gx_FontEntries; p; p = p->next )
	{
		if ( GXWcscmp ( p->GetFileName (), fileName ) != 0 || p->GetSize () != size ) continue;

		p->AddRef ();
		fontEntry = p;

		return;
	}

	fontEntry = new GXFontEntry ( fileName, size );
}

GXFont::~GXFont ()
{
	if ( !fontEntry ) return;

	fontEntry->Release ();
}

GXBool GXFont::GetGlyph ( GXUInt symbol, GXGlyphInfo &info ) const
{
	return fontEntry->GetGlyph ( symbol, info );
}

GXInt GXFont::GetKerning ( GXUInt symbol, GXUInt prevSymbol ) const
{
	return fontEntry->GetKerning ( symbol, prevSymbol );
}

GXUShort GXFont::GetSpaceAdvance () const
{
	return fontEntry->GetSpaceAdvance ();
}

GXUShort GXFont::GetSize () const
{
	return fontEntry->GetSize ();
}

GXTexture2D* GXFont::GetAtlasTexture ( GXByte atlasID ) const
{
	return fontEntry->GetAtlasTexture ( atlasID );
}

GXUInt GXCDECLCALL GXFont::GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, ... ) const
{
	va_list parameters;

	va_start ( parameters, format );
	GXUInt result = fontEntry->GetTextLength ( bufferNumSymbols, format, parameters );
	va_end ( parameters );

	return result;
}

GXUInt GXCALL GXFont::GetTotalLoadedFonts ( const GXWChar** lastFont, GXUShort &lastSize )
{
	GXUInt total = 0u;

	for ( GXFontEntry* p = gx_FontEntries; p; p = p->next )
		total++;

	if ( total > 0u )
	{
		*lastFont = gx_FontEntries->GetFileName ();
		lastSize = gx_FontEntries->GetSize ();
	}
	else
	{
		*lastFont = nullptr;
		lastSize = 0u;
	}

	return total;
}

GXBool GXCALL GXFont::InitFreeTypeLibrary ()
{
	gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );

	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXFont::InitFreeType::Error - Не удалось загрузить GXEngine.dll\n" );
		return GX_FALSE;
	}

	PFNGXFREETYPEINIT GXFreeTypeInit;
	GXFreeTypeInit = reinterpret_cast<PFNGXFREETYPEINIT> ( reinterpret_cast<GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXFreeTypeInit" ) ) );

	if ( !GXFreeTypeInit )
	{
		GXLogW ( L"GXFont::InitFreeType::Error - Не удалось найти функцию GXFreeTypeInit\n" );
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

GXBool GXCALL GXFont::DestroyFreeTypeLibrary ()
{
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXFont::DestroyFreeTypeLibrary::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll\n" );
		return GX_FALSE;
	}

	GXFreeTypeDestroy ();

	if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
	{
		GXLogW ( L"GXFont::DestroyFreeTypeLibrary::Error - Не удалось выгрузить библиотеку GXEngine.dll\n" );
		return GX_FALSE;
	}

	return GX_TRUE;
}
