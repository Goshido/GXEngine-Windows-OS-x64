// version 1.2

#ifndef GX_UNLIT_COLOR_MATERIAL
#define GX_UNLIT_COLOR_MATERIAL


#include "GXMaterial.h"


class GXUnlitColorMaterial final : public GXMaterial
{
	private:
		GXColorRGB	color;

		GLint		mod_view_proj_matLocation;
		GLint		colorLocation;

	public:
		GXUnlitColorMaterial ();
		~GXUnlitColorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		GXVoid SetColor ( const GXColorRGB &newColor );

	private:
		GXUnlitColorMaterial ( const GXUnlitColorMaterial &other ) = delete;
		GXUnlitColorMaterial& operator = ( const GXUnlitColorMaterial &other ) = delete;
};


#endif // GX_UNLIT_COLOR_MATERIAL
