// version 1.8

#ifndef GX_FONT
#define GX_FONT


#include <GXEngine/GXTexture2D.h>
#include <GXEngineDLL/GXEngineAPI.h>


struct GXGlyphInfo
{
	GXTexture2D*	atlas;
	GXVec2			min;
	GXVec2			max;
	GXFloat			offsetY;
	GXFloat			width;
	GXFloat			height;
	GXUShort		advance;
};

// Class handles lazy loading reference counting font resource creation.

class GXFontEntry;
class GXFont
{
	private:
		GXFontEntry*		fontEntry;

	public:
		explicit GXFont ( const GXWChar* fileName, GXUShort size );
		~GXFont ();

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
