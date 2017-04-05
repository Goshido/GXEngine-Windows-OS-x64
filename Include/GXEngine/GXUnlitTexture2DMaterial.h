//version 1.0

#ifndef GX_UNLIT_TEXTURE2D_MATERIAL
#define GX_UNLIT_TEXTURE2D_MATERIAL


#include "GXMaterial.h"
#include "GXTexture.h"


class GXUnlitTexture2DMaterial : public GXMaterial
{
	private:
		GLint		mod_view_proj_matLocation;
		GLint		uvScaleOffsetLocation;
		GLint		colorLocation;

		GXTexture*	texture;
		GXVec4		color;
		GXVec4		uvScaleOffset;

	public:
		GXUnlitTexture2DMaterial ();
		~GXUnlitTexture2DMaterial () override;

		GXVoid Bind ( const GXRenderable &activeRenderable ) const;
		GXVoid Unbind () const;

		GXVoid SetTexture ( GXTexture &texture );
		GXVoid SetTextureScale ( GXFloat x, GXFloat y );
		GXVoid SetTextureOffset ( GXFloat x, GXFloat y );

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		GXVoid SetColor ( const GXVec4 &color );
};


#endif //GX_UNLIT_TEXTURE2D_MATERIAL
