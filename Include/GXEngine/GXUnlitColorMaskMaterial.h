// version 1.0

#ifndef GX_UNLIT_COLOR_MASK_MATERIAL
#define GX_UNLIT_COLOR_MASK_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"


class GXUnlitColorMaskMaterial : public GXMaterial
{
	private:
		GLint			mod_view_proj_matLocation;
		GLint			uvScaleOffsetLocation;
		GLint			colorLocation;

		GXTexture2D*	mask;
		GXVec4			uvScaleOffset;
		GXColorRGB		color;

	public:
		GXUnlitColorMaskMaterial ();
		~GXUnlitColorMaskMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetMaskTexture ( GXTexture2D &texture );
		GXVoid SetMaskScale ( GXFloat x, GXFloat y );
		GXVoid SetMaskOffset ( GXFloat x, GXFloat y );

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		GXVoid SetColor ( const GXColorRGB &newColor );
};


#endif // GX_UNLIT_COLOR_MASK_MATERIAL
