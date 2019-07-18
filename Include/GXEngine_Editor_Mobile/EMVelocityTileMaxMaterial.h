#ifndef EM_VELOCITY_TILE_MAX_MATERIAL
#define EM_VELOCITY_TILE_MAX_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


class EMVelocityTileMaxMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _velocityBlurTexture;
        GXSampler       _sampler;

        GXInt           _maxBlurSamples;
        GXVec2          _inverseScreenResolution;

        GLint           _maxBlurSamplesLocation;
        GLint           _inverseScreenResolutionLocation;

    public:
        EMVelocityTileMaxMaterial ();
        ~EMVelocityTileMaxMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetVelocityBlurTexture ( GXTexture2D &texture );
        GXVoid SetMaxBlurSamples ( GXUByte maxSamples );
        GXVoid SetScreenResolution ( GXUShort width, GXUShort height );

    private:
        EMVelocityTileMaxMaterial ( const EMVelocityTileMaxMaterial &other ) = delete;
        EMVelocityTileMaxMaterial& operator = ( const EMVelocityTileMaxMaterial &other ) = delete;
};


#endif // EM_VELOCITY_TILE_MAX_MATERIAL
