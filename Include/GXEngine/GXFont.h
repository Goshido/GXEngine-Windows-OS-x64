//version 1.4

#ifndef GX_FONT
#define GX_FONT


#include "GXFont.h"
#include "GXOpenGL.h"
#include <GXEngineDLL/GXEngineAPI.h>
#include <GXEngine/GXTextureStorage.h>
#include <GXCommon/GXMath.h>


extern FT_Library				gx_ft_Library;

//-------------------------------------------------------------------------

extern PFNFTNEWMEMORYFACE		GXFtNewMemoryFace;
extern PFNFTDONEFACE			GXFtDoneFace;

extern PFNFTSETCHARSIZE			GXFtSetCharSize;
extern PFNFTSETPIXELSIZES		GXFtSetPixelSizes;

extern PFNFTGETCHARINDEX		GXFtGetCharIndex;
extern PFNFTGETKERNING			GXFtGetKerning;

extern PFNFTLOADGLYPH			GXFtLoadGlyph;
extern PFNFTRENDERGLYPH			GXFtRenderGlyph;

//-------------------------------------------------------------------------

GXBool GXCALL GXFontInit ();
GXBool GXCALL GXFontDestroy ();

//-------------------------------------------------------------------------

struct GXGlyph
{
	GXVec2		min;
	GXVec2		max;
	GXFloat		offsetY;
	GXUShort	advance;
	GXByte		atlasID;
};

struct GXGlyphInfo
{
	GLuint			atlas;
	GXVec2			min;
	GXVec2			max;
	GXFloat			offsetY;
	GXFloat			width;
	GXFloat			height;
	GXUShort		advance;
};

class GXFont
{
	private:
		FT_Face		face;
		GXUShort	size;
		GXGlyph		glyphs[ 0x7FFF ];
		GLuint*		atlases;
		GXByte		atlasID;
		GXUShort	left;
		GXUShort	top;
		GXUShort	bottom;
		GXUShort	spaceAdvance;

	public:
		GXFont ( FT_Face face, GXUShort size );
		~GXFont ();

		GXBool GetGlyph ( GXUInt symbol, GXGlyphInfo &info );
		GXInt GetKerning ( GXUInt symbol, GXUInt prevSymbol );
		GXUShort GetSpaceAdvance ();
		GXUShort GetSize ();
		GXVoid GetAtlasTexture ( GXByte atlasID, GXTexture &texture );
		GXUInt GXCDECLCALL GetTextLength ( GXUInt bufferNumSymbols, const GXWChar* format, ... );

	private:
		GXVoid RenderGlyph ( GXUInt symbol );
		GXVoid CreateAtlas ();
		GXUByte CheckAtlas ( GXUInt width, GXUInt height );
};


#endif //GX_FONT
