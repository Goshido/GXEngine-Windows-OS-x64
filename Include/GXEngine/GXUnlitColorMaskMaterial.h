//version 1.0

#ifndef GX_UNLIT_COLOR_MASK_MATERIAL
#define GX_UNLIT_COLOR_MASK_MATERIAL

#include "GXMaterial.h"
#include "GXTexture.h"


class GXUnlitColorMaskMaterial : public GXMaterial
{
	private:
		GLint			mod_view_proj_matLocation;
		GLint			uvScaleOffsetLocation;
		GLint			colorLocation;

		GXTexture*		mask;
		GXVec4			uvScaleOffset;
		GXVec4			color;

	public:
		GXUnlitColorMaskMaterial ();
		~GXUnlitColorMaskMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) const override;
		GXVoid Unbind () const override;

		GXVoid SetMaskTexture ( GXTexture &texture );
		GXVoid SetMaskScale ( GXFloat x, GXFloat y );
		GXVoid SetMaskOffset ( GXFloat x, GXFloat y );

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		GXVoid SetColor ( const GXVec4 &color );
};


#endif //GX_UNLIT_COLOR_MASK_MATERIAL
