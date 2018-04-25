// version 1.7

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

class GXFontEntry;
struct GXFontParameters;
class GXFont
{
	private:
		FT_Face				face;
		GXUShort			size;
		GXFontParameters*	parameters;

	public:
		GXFont ();
		~GXFont ();

		GXBool GetGlyph ( GXUInt symbol, GXGlyphInfo &info ) const;
		GXInt GetKerning ( GXUInt symbol, GXUInt prevSymbol ) const;
		GXUShort GetSpaceAdvance () const;
		GXUShort GetSize () const;
		GXTexture2D* GetAtlasTexture ( GXByte atlasID );
		GXUInt GXCDECLCALL GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, ... ) const;

		static GXFont& GXCALL GetFont ( const GXWChar* fileName, GXUShort size );
		static GXVoid GXCALL RemoveFont ( GXFont &font );
		static GXUInt GXCALL GetTotalLoadedFonts ( const GXWChar** lastFont, GXUShort &lastSize );

		static GXBool GXCALL InitFreeTypeLibrary ();
		static GXBool GXCALL DestroyFreeTypeLibrary ();

		GXVoid operator = ( const GXFont &other );

	private:
		explicit GXFont ( const GXWChar* fileName, GXUShort size );

		GXVoid RenderGlyph ( GXUInt symbol ) const;
		GXVoid CreateAtlas () const;
		GXUByte CheckAtlas ( GXUInt width, GXUInt height ) const;

		GXBool operator == ( const GXFontEntry &other ) const;
};


#endif // GX_FONT
