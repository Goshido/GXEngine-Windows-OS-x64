// version 1.5

#ifndef GX_FREETYPE
#define GX_FREETYPE


#include <GXCommon/GXLogger.h>

GX_DISABLE_COMMON_WARNINGS

#include <ft2build.h>
#include FT_FREETYPE_H

GX_RESTORE_WARNING_STATE


typedef GXVoid ( GXCALL* FTDestroy ) ();
typedef FT_Error ( *FTNewMemoryFace ) ( FT_Library library, const FT_Byte* file_base, FT_Long file_size, FT_Long face_index, FT_Face* aface );
typedef FT_Error ( *FTDoneFace ) ( FT_Face face );
typedef FT_Error ( *FTSetCharSize ) ( FT_Face face, FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution );
typedef FT_Error ( *FTSetPixelSizes ) ( FT_Face face, FT_UInt pixel_width, FT_UInt pixel_height );
typedef FT_UInt ( *FTGetCharIndex ) ( FT_Face face, FT_ULong  charcode );
typedef FT_Error ( *FTGetKerning ) ( FT_Face face, FT_UInt left_glyph, FT_UInt right_glyph, FT_UInt kern_mode, FT_Vector* akerning );
typedef FT_Error ( *FTLoadGlyph ) ( FT_Face face, FT_UInt glyph_index, FT_Int32 load_flags );
typedef FT_Error ( *FTRenderGlyph ) ( FT_GlyphSlot slot, FT_Render_Mode render_mode );


struct GXFreeTypeFunctions
{
    FTNewMemoryFace*    FT_New_Memory_Face;
    FTDoneFace*         FT_Done_Face;

    FTSetCharSize*      FT_Set_Char_Size;
    FTSetPixelSizes*    FT_Set_Pixel_Sizes;

    FTGetCharIndex*     FT_Get_Char_Index;
    FTGetKerning*       FT_Get_Kerning;

    FTLoadGlyph*        FT_Load_Glyph;
    FTRenderGlyph*      FT_Render_Glyph;

    FTDestroy*          GXFreeTypeDestroy;
};


#endif // GX_FREETYPE
