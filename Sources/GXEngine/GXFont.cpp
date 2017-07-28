//version 1.6

#include <GXEngine/GXFont.h>
#include <GXEngineDLL/GXEngineAPI.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>


#define ATLAS_UNDEFINED		-1
#define ATLAS_NEW_LINE		0
#define ATLAS_NEW_TEXTURE	1
#define ATLAS_FILL			2
#define ATLAS_RESOLUTION	512
#define ATLAS_ONE_PIXEL		0.0019531f	//1 pixel in uv coordinate system
#define ATLAS_SPACING		2


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

static GXFontEntry*				gx_FontHead					= nullptr;

class GXFontEntry
{
	public:
		GXFontEntry*	next;
		GXFontEntry*	prev;

	private:
		GXFont*			font;
		GXWChar*		fileName;
		GXUShort		size;

		GXInt			refs;

	public:
		explicit GXFontEntry ( GXFont &font, const GXWChar* fileName, GXUShort size );

		GXFont& GetFont () const;
		const GXWChar* GetFileName () const;
		GXUShort GetSize () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXFontEntry ();
};

GXFontEntry::GXFontEntry ( GXFont &font, const GXWChar* fileName, GXUShort size )
{
	this->font = &font;
	GXWcsclone ( &this->fileName, fileName );
	this->size = size;

	refs = 1;

	prev = nullptr;

	if ( gx_FontHead )
		gx_FontHead->prev = this;

	next = gx_FontHead;
	gx_FontHead = this;
}

GXFont& GXFontEntry::GetFont () const
{
	return *font;
}

const GXWChar* GXFontEntry::GetFileName () const
{
	return fileName;
}

GXUShort GXFontEntry::GetSize () const
{
	return size;
}

GXVoid GXFontEntry::AddRef ()
{
	refs++;
}

GXVoid GXFontEntry::Release ()
{
	refs--;
	if ( refs <= 0 )
		delete this;
}

GXFontEntry::~GXFontEntry ()
{
	delete font;
	GXSafeFree ( fileName );

	if ( gx_FontHead == this )
		gx_FontHead = gx_FontHead->next;
	else
		prev->next = next;

	if ( next )
		next->prev = prev;
}

//---------------------------------------------------------------------------------------------------

struct GXGlyph
{
	GXVec2		min;
	GXVec2		max;
	GXFloat		offsetY;
	GXUShort	advance;
	GXByte		atlasID;
};

struct GXFontParameters
{
	GXGlyph			glyphs[ 0x7FFF ];
	GXTexture2D*	atlases;
	GXByte			atlasID;
	GXUShort		left;
	GXUShort		top;
	GXUShort		bottom;
	GXUShort		spaceAdvance;
	GXVoid*			mappedFile;
};

//---------------------------------------------------------------------------------------------------

GXFont::GXFont ()
{
	face = nullptr;
	size = 0;
	parameters = nullptr;
}

GXFont::~GXFont ()
{
	if ( parameters == nullptr ) return;

	if ( GXFtDoneFace ( face ) )
		GXLogA ( "GXFont::~GXFont::Error - GXFtDoneFace выполнилась с ошибкой\n" );

	if ( parameters->atlasID != ATLAS_UNDEFINED )
	{
		for ( GXByte i = 0; i <= parameters->atlasID; i++ )
			parameters->atlases[ i ].FreeResources ();

		free ( parameters->atlases );
	}

	free ( parameters->mappedFile );
	
	delete parameters;
}

GXBool GXFont::GetGlyph ( GXUInt symbol, GXGlyphInfo &info ) const
{
	if ( symbol > 0x7FFF )
		return GX_FALSE;

	if ( parameters->glyphs[ symbol ].atlasID != ATLAS_UNDEFINED )
	{
		info.atlas = &parameters->atlases[ parameters->glyphs[ symbol ].atlasID ];
		memcpy ( &info.min, &parameters->glyphs[ symbol ].min, sizeof ( GXVec2 ) );
		memcpy ( &info.max, &parameters->glyphs[ symbol ].max, sizeof ( GXVec2 ) );
		info.offsetY = parameters->glyphs[ symbol ].offsetY;
		info.width = ( parameters->glyphs[ symbol ].max.x - parameters->glyphs[ symbol ].min.x ) * ATLAS_RESOLUTION;
		info.height = ( parameters->glyphs[ symbol ].max.y - parameters->glyphs[ symbol ].min.y ) * ATLAS_RESOLUTION;
		info.advance = parameters->glyphs[ symbol ].advance;

		return GX_TRUE;
	}

	RenderGlyph ( symbol );

	info.atlas = &parameters->atlases[ parameters->glyphs[ symbol ].atlasID ];
	memcpy ( &info.min, &parameters->glyphs[ symbol ].min, sizeof ( GXVec2 ) );
	memcpy ( &info.max, &parameters->glyphs[ symbol ].max, sizeof ( GXVec2 ) );
	info.offsetY = parameters->glyphs[ symbol ].offsetY;
	info.width = ( parameters->glyphs[ symbol ].max.x - parameters->glyphs[ symbol ].min.x ) * ATLAS_RESOLUTION;
	info.height = ( parameters->glyphs[ symbol ].max.y - parameters->glyphs[ symbol ].min.y ) * ATLAS_RESOLUTION;
	info.advance = parameters->glyphs[ symbol ].advance;

	return GX_TRUE;
}

GXInt GXFont::GetKerning ( GXUInt symbol, GXUInt prevSymbol ) const
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

GXUShort GXFont::GetSpaceAdvance () const
{
	if ( !parameters ) return 0;
	return parameters->spaceAdvance;
}

GXUShort GXFont::GetSize () const
{
	return size;
}

GXTexture2D* GXFont::GetAtlasTexture ( GXByte atlasID )
{
	if ( atlasID > this->parameters->atlasID )
	{
		GXLogW ( L"GXFont::GetAtlasTexture::Error - Wrong atlas ID\n" );
		return nullptr;
	}

	return &parameters->atlases[ atlasID ];
}

GXUInt GXCDECLCALL GXFont::GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, ... ) const
{
	GXInt penX = 0;

	GXWChar* text = nullptr;

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

GXFont& GXCALL GXFont::GetFont ( const GXWChar* fileName, GXUShort size )
{
	for ( GXFontEntry* p = gx_FontHead; p; p = p->next )
	{
		if ( GXWcscmp ( p->GetFileName (), fileName ) == 0 && p->GetSize () == size )
		{
			p->AddRef ();
			return p->GetFont ();
		}
	}

	GXFont* font = new GXFont ( fileName, size );
	new GXFontEntry ( *font, fileName, size );
	return *font;
}

GXVoid GXCALL GXFont::RemoveFont ( GXFont &font )
{
	for ( GXFontEntry* p = gx_FontHead; p; p = p->next )
	{
		if ( font == *p )
		{
			p->Release ();
			font = GXFont ();
			return;
		}
	}
}

GXUInt GXCALL GXFont::GetTotalLoadedFonts ( const GXWChar** lastFont, GXUShort &lastSize )
{
	GXUInt total = 0;
	for ( GXFontEntry* p = gx_FontHead; p; p = p->next )
		total++;

	if ( total > 0 )
	{
		*lastFont = gx_FontHead->GetFileName ();
		lastSize = gx_FontHead->GetSize ();
	}
	else
	{
		*lastFont = nullptr;
		lastSize = 0;
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
	GXFreeTypeInit = (PFNGXFREETYPEINIT)GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXFreeTypeInit" );
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

GXVoid GXFont::operator = ( const GXFont &other )
{
	memcpy ( this, &other, sizeof ( GXFont ) );
}

GXFont::GXFont ( const GXWChar* fileName, GXUShort size )
{
	parameters = new GXFontParameters ();

	this->size = size;

	for ( GXUShort i = 0; i < 0x7FFF; i++ )
		parameters->glyphs[ i ].atlasID = ATLAS_UNDEFINED;

	parameters->atlases = nullptr;
	parameters->atlasID = ATLAS_UNDEFINED;

	parameters->left = 0;
	parameters->bottom = 0;
	parameters->top = 0;

	GXUShort temp = (GXUShort)( size * 0.5f );
	parameters->spaceAdvance = temp == 0 ? 1 : temp;
	GXUPointer totalSize = 0;

	if ( !GXLoadFile ( fileName, &parameters->mappedFile, totalSize, GX_TRUE ) )
	{
		GXLogA ( "GXFont::GXFont::Error - не могу загрузить файл шрифта %s\n", fileName );
		face = nullptr;
		GXSafeDelete ( parameters );
	}
	else if ( GXFtNewMemoryFace ( gx_ft_Library, (FT_Byte*)parameters->mappedFile, (FT_Long)totalSize, 0, &face ) )
	{
		GXLogW ( L"GXFontUnit::GXFontUnit::Error - GXFtNewMemoryFace выполнилась с ошибкой для шрифта %s failed\n", fileName );
		free ( parameters->mappedFile );
		GXSafeDelete ( parameters );
		face = nullptr;
	}
}

GXVoid GXFont::RenderGlyph ( GXUInt symbol ) const
{
	if ( GXFtSetPixelSizes ( face, size, size ) )
		GXLogA ( "GXFont::RenderGlyph::Error - Can't set font size %i\n", size );

	FT_UInt glyphIndex = GXFtGetCharIndex ( face, symbol );

	if ( GXFtLoadGlyph ( face, glyphIndex, FT_LOAD_RENDER ) )
		GXLogA ( "GXFont::RenderGlyph::Error - FT_Load_Glyph failed at symbol #%i\n", symbol );

	if ( GXFtRenderGlyph ( face->glyph, FT_RENDER_MODE_NORMAL ) )
		GXLogA ( "GXFont::RenderGlyph::Error - FT_Render_Glyph failed at symbol #%i\n", symbol );

	parameters->glyphs[ symbol ].offsetY = (GXFloat)( ( face->glyph->metrics.horiBearingY - face->glyph->metrics.height ) >> 6 );
	parameters->glyphs[ symbol ].advance = (GXUShort)( face->glyph->advance.x >> 6 );

	FT_Bitmap bitmap = face->glyph->bitmap;

	switch ( CheckAtlas ( bitmap.width, bitmap.rows ) )
	{
		case ATLAS_NEW_LINE:
			parameters->left = 0;
			parameters->bottom = parameters->top;
			parameters->top = parameters->bottom;
		break;

		case ATLAS_NEW_TEXTURE:
			CreateAtlas ();

			parameters->left = 0;
			parameters->bottom = 0;
			parameters->top = 0;
		break;
	}

	parameters->glyphs[ symbol ].min = GXCreateVec2 ( parameters->left * ATLAS_ONE_PIXEL, parameters->bottom * ATLAS_ONE_PIXEL );
	parameters->glyphs[ symbol ].max = GXCreateVec2 ( ( parameters->left + bitmap.width + 1 ) * ATLAS_ONE_PIXEL, ( parameters->bottom + bitmap.rows ) * ATLAS_ONE_PIXEL );

	parameters->glyphs[ symbol ].atlasID = parameters->atlasID;

	GXUByte* buffer = (GXUByte*)malloc ( bitmap.width * bitmap.rows );

	for ( GXUInt h = 0; h < (GXUInt)bitmap.rows; h++ )
	{
		for ( GXUInt w = 0; w < (GXUInt)bitmap.width; w++ )
			buffer[ h * bitmap.width + w ] = bitmap.buffer[ ( bitmap.rows - 1 - h ) * bitmap.width + w ];
	}

	parameters->atlases[ parameters->atlasID ].FillRegionPixelData ( parameters->left, parameters->bottom, (GXUShort)bitmap.width, (GXUShort)bitmap.rows, buffer );

	free ( buffer );

	parameters->left += bitmap.width + ATLAS_SPACING;

	if ( parameters->top < ( parameters->bottom + bitmap.rows + ATLAS_SPACING ) )
		parameters->top = parameters->bottom + bitmap.rows + ATLAS_SPACING;
}

GXVoid GXFont::CreateAtlas () const
{
	if ( parameters->atlasID == ATLAS_UNDEFINED )
	{
		parameters->atlasID = 0;
		parameters->atlases = (GXTexture2D*)malloc ( sizeof ( GXTexture2D ) );
	}
	else
	{
		GXTexture2D* temp = (GXTexture2D*)malloc ( ( parameters->atlasID + 2 ) * sizeof ( GXTexture2D ) );
		for ( GXUShort i = 0; i < parameters->atlasID; i++ )
			temp[ i ] = parameters->atlases[ i ];

		parameters->atlasID++;
		free ( parameters->atlases );
		parameters->atlases = temp;
	}

	GXUInt size = ATLAS_RESOLUTION * ATLAS_RESOLUTION;
	GXUByte* data = (GXUByte*)malloc ( size );

	memset ( data, 0, size );

	parameters->atlases[ parameters->atlasID ].InitResources ( ATLAS_RESOLUTION, ATLAS_RESOLUTION, GL_R8, GX_FALSE, GL_CLAMP_TO_EDGE );
	parameters->atlases[ parameters->atlasID ].FillWholePixelData ( data );

	free ( data );
}

GXUByte GXFont::CheckAtlas ( GXUInt width, GXUInt height ) const
{
	if ( parameters->atlasID == ATLAS_UNDEFINED )
		return ATLAS_NEW_TEXTURE;

	if ( ( parameters->left + width ) >= ATLAS_RESOLUTION )
	{
		if ( ( parameters->bottom + height ) >= ATLAS_RESOLUTION )
			return ATLAS_NEW_TEXTURE;
		else
			return ATLAS_NEW_LINE;
	}

	if ( ( parameters->bottom + height ) >= ATLAS_RESOLUTION )
		return ATLAS_NEW_TEXTURE;

	return ATLAS_FILL;
}

GXBool GXFont::operator == ( const GXFontEntry &other ) const
{
	return face == other.GetFont ().face;
}
