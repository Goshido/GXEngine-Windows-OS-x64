// version 1.4

#ifndef GX_UNLIT_COLOR_MASK_MATERIAL
#define GX_UNLIT_COLOR_MASK_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"
#include "GXSampler.h"


class GXUnlitColorMaskMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _mask;
        GXSampler       _sampler;
        GXVec4          _uvScaleOffset;
        GXColorRGB      _color;

        GLint           _mod_view_proj_matLocation;
        GLint           _uvScaleOffsetLocation;
        GLint           _colorLocation;

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

    private:
        GXUnlitColorMaskMaterial ( const GXUnlitColorMaskMaterial &other ) = delete;
        GXUnlitColorMaskMaterial& operator = ( const GXUnlitColorMaskMaterial &other ) = delete;
};


#endif // GX_UNLIT_COLOR_MASK_MATERIAL
