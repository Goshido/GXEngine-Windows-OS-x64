// version 1.4

#ifndef GX_TEXUTE_2D_GAMMA_CORRECTOR_MATERIAL
#define GX_TEXUTE_2D_GAMMA_CORRECTOR_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"
#include "GXSampler.h"


class GXTexture2DGammaCorrectorMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _sRGBTexture;
        GXSampler       _sampler;

        GLint           _gammaLocation;

    public:
        GXTexture2DGammaCorrectorMaterial ();
        ~GXTexture2DGammaCorrectorMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetSRGBTexture ( GXTexture2D &texture );

    private:
        GXTexture2DGammaCorrectorMaterial ( const GXTexture2DGammaCorrectorMaterial &other ) = delete;
        GXTexture2DGammaCorrectorMaterial& operator = ( const GXTexture2DGammaCorrectorMaterial &other ) = delete;
};


#endif // GX_TEXUTE_2D_GAMMA_CORRECTOR_MATERIAL
