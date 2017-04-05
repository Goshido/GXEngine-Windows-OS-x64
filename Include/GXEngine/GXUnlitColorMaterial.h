//version 1.0

#ifndef GX_UNLIT_COLOR_MATERIAL
#define GX_UNLIT_COLOR_MATERIAL


#include "GXMaterial.h"


class GXUnlitColorMaterial : public GXMaterial
{
	private:
		GLint		mod_view_proj_matLocation;
		GLint		colorLocation;

		GXVec4		color;

	public:
		GXUnlitColorMaterial ();
		~GXUnlitColorMaterial () override;

		GXVoid Bind ( const GXRenderable &activeRenderable ) const;
		GXVoid Unbind () const;

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		GXVoid SetColor ( const GXVec4 &color );
};


#endif //GX_UNLIT_COLOR_MATERIAL
