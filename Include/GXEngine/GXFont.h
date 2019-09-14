// version 1.17

#ifndef GX_FONT
#define GX_FONT


#include <GXEngine/GXTexture2D.h>
#include <GXEngineDLL/GXEngineAPI.h>


struct GXGlyphInfo final
{
    GXTexture2D*    _atlas;
    GXVec2          _min;
    GXVec2          _max;
    GXFloat         _offsetY;
    GXFloat         _width;
    GXFloat         _height;
    GXUShort        _advance;
};

// Class handles lazy loading reference counting font resource creation.

class GXFontEntry;
class GXFont final : public GXMemoryInspector
{
    private:
        GXFontEntry*    _fontEntry;

    public:
        explicit GXFont ( const GXString &fileName, GXUShort size );
        ~GXFont () override;

        GXBool GetGlyph ( GXUInt symbol, GXGlyphInfo &info ) const;
        GXInt GetKerning ( GXUInt symbol, GXUInt prevSymbol ) const;
        GXUShort GetSpaceAdvance () const;
        GXUShort GetSize () const;
        GXTexture2D* GetAtlasTexture ( GXByte atlasID ) const;
        GXUInt GXCDECLCALL GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, ... ) const;

        static GXUInt GXCALL GetTotalLoadedFonts ( const GXWChar** lastFont, GXUShort &lastSize );

        static GXBool GXCALL InitFreeTypeLibrary ();
        static GXBool GXCALL DestroyFreeTypeLibrary ();

    private:
        GXFont () = delete;
        GXFont ( const GXFont &other ) = delete;
        GXFont& operator = ( const GXFont &other ) = delete;
};


#endif // GX_FONT
