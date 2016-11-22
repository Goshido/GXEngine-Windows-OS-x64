//version 1.1

#include <GXEngineDLL/GXEngineDLL.h>
#include <GXEngineDLL/GXFreeType.h>


FT_Library gx_ft_Library;
GXBool gx_tf_LibraryValid = GX_FALSE;


GXVoid GXCALL GXFreeTypeDestroy ()
{
	if ( !gx_tf_LibraryValid ) return;

	if ( FT_Done_FreeType ( gx_ft_Library ) )
	{
		GXLogW ( L"GXFreeTypeDestroy::Error - FT_Done_FreeType провалилась" );
		GXDebugBox ( L"FT_Done_FreeType провалилась" );
	}
}

GXDLLEXPORT GXBool GXCALL GXFreeTypeInit ( GXFreeTypeFunctions &out, FT_Library &library )
{
	if ( FT_Init_FreeType ( &gx_ft_Library ) )
	{
		gx_tf_LibraryValid = GX_FALSE;

		GXLogW ( L"GXFreeTypeInit::Error - FT_Init_FreeType провалилась" );
		GXDebugBox ( L"FT_Init_FreeType провалилась" );
	}
	else
		gx_tf_LibraryValid = GX_TRUE;

	*out.FT_Done_Face = &FT_Done_Face;
	*out.FT_Get_Char_Index = &FT_Get_Char_Index;
	*out.FT_Get_Kerning = &FT_Get_Kerning;
	*out.FT_Load_Glyph = &FT_Load_Glyph;
	*out.FT_New_Memory_Face = &FT_New_Memory_Face;
	*out.FT_Render_Glyph = &FT_Render_Glyph;
	*out.FT_Set_Char_Size = &FT_Set_Char_Size;
	*out.FT_Set_Pixel_Sizes = &FT_Set_Pixel_Sizes;
	*out.GXFreeTypeDestroy = &GXFreeTypeDestroy;

	library = gx_ft_Library;

	return gx_tf_LibraryValid;
}