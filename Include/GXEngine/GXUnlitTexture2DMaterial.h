// version 1.4

#ifndef GX_UNLIT_TEXTURE2D_MATERIAL
#define GX_UNLIT_TEXTURE2D_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"
#include "GXSampler.h"


class GXUnlitTexture2DMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _texture;
        GXSampler       _sampler;
        GXColorRGB      _color;
        GXVec4          _uvScaleOffset;

        GLint           _mod_view_proj_matLocation;
        GLint           _uvScaleOffsetLocation;
        GLint           _colorLocation;

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
