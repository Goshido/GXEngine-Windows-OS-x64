//version 1.0

#ifndef GX_UNLIT_COLOR_MATERIAL
#define GX_UNLIT_COLOR_MATERIAL


#include "GXMaterial.h"


class GXUnlitColorMaterial : public GXMaterial
{
	private:
		GLuint		mod_view_proj_matLocation;
		GLuint		colorLocation;

		GXVec4		color;

	public:
		GXUnlitColorMaterial ();
		~GXUnlitColorMaterial () override;

		GXVoid Bind ( const GXRenderable &activeRenderable ) const;
		GXVoid Unbind () const;

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
};


#endif //GX_UNLIT_COLOR_MATERIAL
