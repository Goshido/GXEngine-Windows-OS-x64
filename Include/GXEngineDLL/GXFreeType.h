//version 1.1

#ifndef GX_FREETYPE
#define GX_FREETYPE


#include <ft2build.h>
#include FT_FREETYPE_H
#include <GXCommon/GXLogger.h>


typedef GXVoid ( GXCALL* PFNGXFREETYPEDESTROY ) ();
typedef FT_Error ( *PFNFTNEWMEMORYFACE ) ( FT_Library library, const FT_Byte* file_base, FT_Long file_size, FT_Long face_index, FT_Face* aface );
typedef FT_Error ( *PFNFTDONEFACE ) ( FT_Face face );
typedef FT_Error ( *PFNFTSETCHARSIZE ) ( FT_Face face, FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution );
typedef FT_Error ( *PFNFTSETPIXELSIZES ) ( FT_Face face, FT_UInt pixel_width, FT_UInt pixel_height );
typedef FT_UInt ( *PFNFTGETCHARINDEX ) ( FT_Face face, FT_ULong  charcode );
typedef FT_Error ( *PFNFTGETKERNING ) ( FT_Face face, FT_UInt left_glyph, FT_UInt right_glyph, FT_UInt kern_mode, FT_Vector* akerning );
typedef FT_Error ( *PFNFTLOADGLYPH ) ( FT_Face face, FT_UInt glyph_index, FT_Int32 load_flags );
typedef FT_Error ( *PFNFTRENDERGLYPH ) ( FT_GlyphSlot slot, FT_Render_Mode render_mode );


struct GXFreeTypeFunctions
{
	PFNFTNEWMEMORYFACE*		FT_New_Memory_Face;
	PFNFTDONEFACE*			FT_Done_Face;

	PFNFTSETCHARSIZE*		FT_Set_Char_Size;
	PFNFTSETPIXELSIZES*		FT_Set_Pixel_Sizes;

	PFNFTGETCHARINDEX*		FT_Get_Char_Index;
	PFNFTGETKERNING*		FT_Get_Kerning;

	PFNFTLOADGLYPH*			FT_Load_Glyph;
	PFNFTRENDERGLYPH*		FT_Render_Glyph;

	PFNGXFREETYPEDESTROY*	GXFreeTypeDestroy;
};


#endif //GX_FREETYPE