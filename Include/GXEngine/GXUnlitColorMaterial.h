// version 1.0

#ifndef GX_UNLIT_COLOR_MATERIAL
#define GX_UNLIT_COLOR_MATERIAL


#include "GXMaterial.h"


class GXUnlitColorMaterial : public GXMaterial
{
	private:
		GLint		mod_view_proj_matLocation;
		GLint		colorLocation;

		GXColorRGB	color;

	public:
		GXUnlitColorMaterial ();
		~GXUnlitColorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		GXVoid SetColor ( const GXColorRGB &newColor );
};


#endif // GX_UNLIT_COLOR_MATERIAL
