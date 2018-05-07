// version 1.1

#ifndef GX_UNLIT_TEXTURE2D_MATERIAL
#define GX_UNLIT_TEXTURE2D_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"


class GXUnlitTexture2DMaterial : public GXMaterial
{
	private:
		GXTexture2D*	texture;
		GXColorRGB		color;
		GXVec4			uvScaleOffset;

		GLint			mod_view_proj_matLocation;
		GLint			uvScaleOffsetLocation;
		GLint			colorLocation;

	public:
		GXUnlitTexture2DMaterial ();
		~GXUnlitTexture2DMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetTexture ( GXTexture2D &textureObject );
		GXVoid SetTextureScale ( GXFloat x, GXFloat y );
		GXVoid SetTextureOffset ( GXFloat x, GXFloat y );

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		GXVoid SetColor ( const GXColorRGB &newColor );

	private:
		GXUnlitTexture2DMaterial ( const GXUnlitTexture2DMaterial &other ) = delete;
		GXUnlitTexture2DMaterial& operator = ( const GXUnlitTexture2DMaterial &other ) = delete;
};


#endif // GX_UNLIT_TEXTURE2D_MATERIAL
