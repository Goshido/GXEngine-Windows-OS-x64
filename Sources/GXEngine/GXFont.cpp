// version 1.15

#include <GXEngine/GXFont.h>
#include <GXEngineDLL/GXEngineAPI.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXUPointerAtomic.h>


#define ATLAS_UNDEFINED         -1
#define ATLAS_NEW_LINE          0u
#define ATLAS_NEW_TEXTURE       1u
#define ATLAS_FILL              2u
#define ATLAS_RESOLUTION        512u
#define ATLAS_ONE_PIXEL         0.0019531f    // 1 pixel in uv coordinate system
#define ATLAS_SPACING           2u

#define MAXIMUM_GLYPHS          0x7FFFu

//---------------------------------------------------------------------------------------------------------------------

extern HMODULE              gx_GXEngineDLLModuleHandle;

static FT_Library           gx_ft_Library = nullptr;

static FTNewMemoryFace      GXFtNewMemoryFace = nullptr;
static FTDoneFace           GXFtDoneFace = nullptr;

static FTSetCharSize        GXFtSetCharSize = nullptr;
static FTSetPixelSizes      GXFtSetPixelSizes = nullptr;

static FTGetCharIndex       GXFtGetCharIndex = nullptr;
static FTGetKerning         GXFtGetKerning = nullptr;

static FTLoadGlyph          GXFtLoadGlyph = nullptr;
static FTRenderGlyph        GXFtRenderGlyph = nullptr;

static FTDestroy            GXFreeTypeDestroy = nullptr;

static GXFontEntry*         gx_FontEntries = nullptr;

//---------------------------------------------------------------------------------------------------------------------

struct GXGlyph final
{
    GXVec2      _min;
    GXVec2      _max;
    GXFloat     _offsetY;
    GXUShort    _advance;
    GXByte      _atlasID;
};

//---------------------------------------------------------------------------------------------------------------------

class GXFontEntry final : public GXMemoryInspector
{
    friend class GXFont;

    private:
        GXFontEntry*        _previous;
        GXFontEntry*        _next;
        GXUPointerAtomic    _references;

        GXTexture2D**       _atlases;
        GXByte              _lastAtlasID;
        GXUShort            _left;
        GXUShort            _top;
        GXUShort            _bottom;

        const GXUShort      _size;
        const GXString      _fileName;

        FT_Face             _face;
        GXUShort            _spaceAdvance;

        GXGlyph             _glyphs[ MAXIMUM_GLYPHS ];
        GXUByte*            _mappedFile;

    public:
        explicit GXFontEntry ( const GXWChar* fontFileName, GXUShort fontSize );

        const GXWChar* GetFileName () const;
        GXUShort GetSize () const;

        GXBool GetGlyph ( GXUInt symbol, GXGlyphInfo &info );
        GXInt GetKerning ( GXUInt symbol, GXUInt prevSymbol ) const;
        GXUShort GetSpaceAdvance () const;
        GXTexture2D* GetAtlasTexture ( GXByte atlasID );

        GXUInt GXCDECLCALL GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, va_list parameters );

        GXVoid AddReference ();
        GXVoid Release ();

    private:
        ~GXFontEntry () override;

        GXVoid RenderGlyph ( GXUInt symbol );
        GXUByte CheckAtlas ( GXUInt width, GXUInt heigth ) const;
        GXVoid CreateAtlas ();

        GXFontEntry () = delete;
        GXFontEntry ( const GXFontEntry &other ) = delete;
        GXFontEntry& operator = ( const GXFontEntry &other ) = delete;
};

GXFontEntry::GXFontEntry ( const GXWChar* fontFileName, GXUShort fontSize )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXFontEntry" )
    _previous ( nullptr ),
    _next ( gx_FontEntries ),
    _references ( 1u ),
    _atlases ( nullptr ),
    _lastAtlasID ( ATLAS_UNDEFINED ),
    _left ( 0u ),
    _bottom ( 0u ),
    _top ( 0u ),
    _size ( fontSize ),
    _fileName ( fontFileName )
{
    if ( gx_FontEntries )
        gx_FontEntries->_previous = this;

    gx_FontEntries = this;

    for ( GXUShort i = 0u; i < MAXIMUM_GLYPHS; ++i )
        _glyphs[ i ]._atlasID = ATLAS_UNDEFINED;

    GXUShort temp = static_cast<GXUShort> ( _size * 0.5f );
    _spaceAdvance = temp == 0u ? 1u : temp;
    GXUPointer totalSize = 0u;

    GXFile file ( _fileName );

    if ( !file.LoadContent ( _mappedFile, totalSize, eGXFileContentOwner::User, GX_TRUE ) )
    {
        GXLogA ( "GXFontEntry::GXFontEntry::Error - Не могу загрузить файл шрифта %S\n", _fileName );
        _face = nullptr;

        return;
    }

    if ( !GXFtNewMemoryFace ( gx_ft_Library, static_cast<FT_Byte*> ( _mappedFile ), static_cast<FT_Long> ( totalSize ), 0, &_face ) ) return;

    GXLogA ( "GXFontEntry::GXFontEntry::Error - GXFtNewMemoryFace выполнилась с ошибкой для шрифта %S failed\n", _fileName );
    SafeFree ( reinterpret_cast<GXVoid**> ( &_mappedFile ) );
    _face = nullptr;
}

const GXWChar* GXFontEntry::GetFileName () const
{
    return _fileName;
}

GXUShort GXFontEntry::GetSize () const
{
    return _size;
}

GXBool GXFontEntry::GetGlyph ( GXUInt symbol, GXGlyphInfo &info )
{
    if ( symbol > MAXIMUM_GLYPHS ) return GX_FALSE;

    const GXGlyph& glyph = _glyphs[ symbol ];

    if ( _glyphs[ symbol ]._atlasID != ATLAS_UNDEFINED )
    {
        info._atlas = _atlases[ glyph._atlasID ];
        info._min = glyph._min;
        info._max = glyph._max;
        info._offsetY = glyph._offsetY;
        info._width = ( glyph._max.GetX () - glyph._min.GetX () ) * ATLAS_RESOLUTION;
        info._height = ( glyph._max.GetY () - glyph._min.GetY () ) * ATLAS_RESOLUTION;
        info._advance = glyph._advance;

        return GX_TRUE;
    }

    RenderGlyph ( symbol );

    info._atlas = _atlases[ glyph._atlasID ];
    info._min = glyph._min;
    info._max = glyph._max;
    info._offsetY = glyph._offsetY;
    info._width = ( glyph._max.GetX () - glyph._min.GetX () ) * ATLAS_RESOLUTION;
    info._height = ( glyph._max.GetY () - glyph._min.GetY () ) * ATLAS_RESOLUTION;
    info._advance = glyph._advance;

    return GX_TRUE;
}

GXInt GXFontEntry::GetKerning ( GXUInt symbol, GXUInt prevSymbol ) const
{
    FT_UInt glyphIndex = GXFtGetCharIndex ( _face, symbol );
    FT_UInt prevGlyphIndex = GXFtGetCharIndex ( _face, prevSymbol );

    if ( !FT_HAS_KERNING ( _face ) ) return 0;

    FT_Vector delta;
    GXFtGetKerning ( _face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta );
    return static_cast<GXInt> ( delta.x >> 6 );
}

GXUShort GXFontEntry::GetSpaceAdvance () const
{
    return _spaceAdvance;
}

GXTexture2D* GXFontEntry::GetAtlasTexture ( GXByte atlasID )
{
    if ( atlasID > atlasID )
    {
        GXLogA ( "GXFontEntry::GetAtlasTexture::Error - Wrong atlas ID\n" );
        return nullptr;
    }

    return _atlases[ atlasID ];
}

GXUInt GXCDECLCALL GXFontEntry::GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, va_list parameters )
{
    GXInt penX = 0;

    GXWChar* text = nullptr;

    if ( bufferNumSymbols )
    {
        GXWChar* temp = static_cast<GXWChar*> ( Malloc ( bufferNumSymbols * sizeof ( GXWChar ) ) );
        vswprintf_s ( temp, bufferNumSymbols, format, parameters );

        text = temp;
    }
    else
    {
        text = const_cast<GXWChar*> ( format );
    }

    GXUPointer len = GXWcslen ( text );

    GXUInt prevSymbol = 0u;
    GXGlyphInfo info;

    for ( GXUPointer i = 0u; i < len; ++i )
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

        penX += info._advance;
    }

    if ( bufferNumSymbols )
        Free ( text );

    return static_cast<GXUInt> ( penX );
}

GXVoid GXFontEntry::AddReference ()
{
    ++_references;
}

GXVoid GXFontEntry::Release ()
{
    --_references;

    if ( _references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXFontEntry::~GXFontEntry ()
{
    if ( GXFtDoneFace ( _face ) )
        GXLogA ( "GXFontEntry::~GXFontEntry::Error - GXFtDoneFace выполнилась с ошибкой\n" );

    if ( _lastAtlasID != ATLAS_UNDEFINED )
    {
        for ( GXByte i = 0; i <= _lastAtlasID; ++i )
            delete _atlases[ i ];

        Free ( _atlases );
    }

    Free ( _mappedFile );

    if ( gx_FontEntries == this )
        gx_FontEntries = gx_FontEntries->_next;
    else
        _previous->_next = _next;

    if ( !_next ) return;

    _next->_previous = _previous;
}

GXVoid GXFontEntry::RenderGlyph ( GXUInt symbol )
{
    if ( GXFtSetPixelSizes ( _face, _size, _size ) )
        GXLogA ( "GXFontEntry::RenderGlyph::Error - Can't set font size %i\n", _size );

    FT_UInt glyphIndex = GXFtGetCharIndex ( _face, symbol );

    if ( GXFtLoadGlyph ( _face, glyphIndex, FT_LOAD_RENDER ) )
        GXLogA ( "GXFontEntry::RenderGlyph::Error - FT_Load_Glyph failed at symbol #%i\n", symbol );

    if ( GXFtRenderGlyph ( _face->glyph, FT_RENDER_MODE_NORMAL ) )
        GXLogA ( "GXFontEntry::RenderGlyph::Error - FT_Render_Glyph failed at symbol #%i\n", symbol );

    GXGlyph& glyph = _glyphs[ symbol ];

    glyph._offsetY = static_cast<GXFloat> ( ( _face->glyph->metrics.horiBearingY - _face->glyph->metrics.height ) >> 6 );
    glyph._advance = static_cast<GXUShort> ( _face->glyph->advance.x >> 6 );

    FT_Bitmap bitmap = _face->glyph->bitmap;

    switch ( CheckAtlas ( static_cast<GXUInt> ( bitmap.width ), static_cast<GXUInt> ( bitmap.rows ) ) )
    {
        case ATLAS_NEW_LINE:
            _left = 0u;
            _bottom = _top;
            _top = _bottom;
        break;

        case ATLAS_NEW_TEXTURE:
            CreateAtlas ();

            _left = 0u;
            _bottom = 0u;
            _top = 0u;
        break;

        default:
            // NOTHING
        break;
    }

    glyph._min.Init ( _left * ATLAS_ONE_PIXEL, _bottom * ATLAS_ONE_PIXEL );
    glyph._max.Init ( ( _left + bitmap.width + 1 ) * ATLAS_ONE_PIXEL, ( _bottom + bitmap.rows ) * ATLAS_ONE_PIXEL );

    glyph._atlasID = _lastAtlasID;

    GXUByte* buffer = static_cast<GXUByte*> ( Malloc ( static_cast<GXUPointer> ( bitmap.width * bitmap.rows ) ) );

    for ( GXUInt h = 0u; h < static_cast<GXUInt> ( bitmap.rows ); ++h )
    {
        for ( GXUInt w = 0u; w < static_cast<GXUInt> ( bitmap.width ); ++w )
        {
            buffer[ h * bitmap.width + w ] = bitmap.buffer[ ( bitmap.rows - 1 - h ) * bitmap.width + w ];
        }
    }

    _atlases[ _lastAtlasID ]->FillRegionPixelData ( _left, _bottom, static_cast<GXUShort> ( bitmap.width ), static_cast<GXUShort> ( bitmap.rows ), buffer );

    Free ( buffer );

    _left += static_cast<GXUShort> ( bitmap.width + ATLAS_SPACING );

    if ( _top >= ( _bottom + bitmap.rows + ATLAS_SPACING ) ) return;

    _top = static_cast<GXUShort> ( _bottom + static_cast<GXUShort> ( bitmap.rows ) + ATLAS_SPACING );
}

GXUByte GXFontEntry::CheckAtlas ( GXUInt width, GXUInt height ) const
{
    if ( _lastAtlasID == ATLAS_UNDEFINED )
        return ATLAS_NEW_TEXTURE;

    if ( ( _left + width ) >= ATLAS_RESOLUTION )
    {
        if ( ( _bottom + height ) >= ATLAS_RESOLUTION )
            return ATLAS_NEW_TEXTURE;
        else
            return ATLAS_NEW_LINE;
    }

    if ( ( _bottom + height ) >= ATLAS_RESOLUTION )
        return ATLAS_NEW_TEXTURE;

    return ATLAS_FILL;
}

GXVoid GXFontEntry::CreateAtlas ()
{
    if ( _lastAtlasID == ATLAS_UNDEFINED )
    {
        _lastAtlasID = 0;
        _atlases = static_cast<GXTexture2D**> ( Malloc ( sizeof ( GXTexture2D* ) ) );
    }
    else
    {
        GXTexture2D** temp = static_cast<GXTexture2D**> ( Malloc ( ( _lastAtlasID + 2 ) * sizeof ( GXTexture2D* ) ) );

        for ( GXByte i = 0u; i < _lastAtlasID; ++i )
            temp[ i ] = _atlases[ i ];

        ++_lastAtlasID;
        Free ( _atlases );
        _atlases = temp;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2D" );
    GXTexture2D* atlas = new GXTexture2D ( ATLAS_RESOLUTION, ATLAS_RESOLUTION, GL_R8, GX_FALSE );
    atlas->FillWholePixelData ( nullptr );
    _atlases[ _lastAtlasID ] = atlas;
}

//---------------------------------------------------------------------------------------------------------------------

GXFont::GXFont ( const GXWChar* fileName, GXUShort size )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXFont" )
{
    for ( GXFontEntry* p = gx_FontEntries; p; p = p->_next )
    {
        if ( GXWcscmp ( p->GetFileName (), fileName ) != 0 || p->GetSize () != size ) continue;

        p->AddReference ();
        _fontEntry = p;

        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXFontEntry" );
    _fontEntry = new GXFontEntry ( fileName, size );
}

GXFont::~GXFont ()
{
    if ( !_fontEntry ) return;

    _fontEntry->Release ();
}

GXBool GXFont::GetGlyph ( GXUInt symbol, GXGlyphInfo &info ) const
{
    return _fontEntry->GetGlyph ( symbol, info );
}

GXInt GXFont::GetKerning ( GXUInt symbol, GXUInt prevSymbol ) const
{
    return _fontEntry->GetKerning ( symbol, prevSymbol );
}

GXUShort GXFont::GetSpaceAdvance () const
{
    return _fontEntry->GetSpaceAdvance ();
}

GXUShort GXFont::GetSize () const
{
    return _fontEntry->GetSize ();
}

GXTexture2D* GXFont::GetAtlasTexture ( GXByte atlasID ) const
{
    return _fontEntry->GetAtlasTexture ( atlasID );
}

GXUInt GXCDECLCALL GXFont::GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, ... ) const
{
    va_list parameters;

    va_start ( parameters, format );
    GXUInt result = _fontEntry->GetTextLength ( bufferNumSymbols, format, parameters );
    va_end ( parameters );

    return result;
}

GXUInt GXCALL GXFont::GetTotalLoadedFonts ( const GXWChar** lastFont, GXUShort &lastSize )
{
    GXUInt total = 0u;

    for ( GXFontEntry* p = gx_FontEntries; p; p = p->_next )
        ++total;

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
        GXLogA ( "GXFont::InitFreeType::Error - Не удалось загрузить GXEngine.dll\n" );
        return GX_FALSE;
    }

    GXFreeTypeInitFunc GXFreeTypeInit;
    GXFreeTypeInit = reinterpret_cast<GXFreeTypeInitFunc> ( reinterpret_cast<GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXFreeTypeInit" ) ) );

    if ( !GXFreeTypeInit )
    {
        GXLogA ( "GXFont::InitFreeType::Error - Не удалось найти функцию GXFreeTypeInit\n" );
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
        GXLogA ( "GXFont::DestroyFreeTypeLibrary::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll\n" );
        return GX_FALSE;
    }

    GXFreeTypeDestroy ();

    if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
    {
        GXLogA ( "GXFont::DestroyFreeTypeLibrary::Error - Не удалось выгрузить библиотеку GXEngine.dll\n" );
        return GX_FALSE;
    }

    return GX_TRUE;
}
