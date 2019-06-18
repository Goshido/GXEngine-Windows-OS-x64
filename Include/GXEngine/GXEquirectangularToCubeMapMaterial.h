// version 1.4

#ifndef GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL
#define GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"
#include "GXSampler.h"


class GXEquirectangularToCubeMapMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _equirectangularTexture;
        GXSampler       _sampler;

        GXMat4          _viewProjectionMatrices[ 6u ];
        GLint           _viewProjectionMatricesLocation;

        GXFloat         _gamma;
        GLint           _gammaLocation;

    public:
        GXEquirectangularToCubeMapMaterial ();
        ~GXEquirectangularToCubeMapMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetEquirectangularTexture ( GXTexture2D &texture );
        GXVoid EnableGammaCorrection ();
        GXVoid DisableGammaCorrection ();

    private:
        GXEquirectangularToCubeMapMaterial ( const GXEquirectangularToCubeMapMaterial &other ) = delete;
        GXEquirectangularToCubeMapMaterial& operator = ( const GXEquirectangularToCubeMapMaterial &other ) = delete;
};


#endif // GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL
