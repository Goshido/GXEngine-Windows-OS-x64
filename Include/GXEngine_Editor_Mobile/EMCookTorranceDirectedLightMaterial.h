#ifndef EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL
#define EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


class EMCookTorranceDirectedLightMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _albedoTexture;
        GXTexture2D*    _normalTexture;
        GXTexture2D*    _emissionTexture;
        GXTexture2D*    _parameterTexture;
        GXTexture2D*    _depthTexture;

        GXSampler       _sampler;

        GXColorRGB      _hue;
        GXFloat         _intensity;
        GXColorRGB      _hdrColor;
        GXColorRGB      _ambientColor;
        GXVec3          _toLightDirectionView;

        GLint           _hueLocation;
        GLint           _intensityLocation;
        GLint           _hdrColorLocation;
        GLint           _ambientColorLocation;
        GLint           _toLightDirectionViewLocation;
        GLint           _inverseProjectionMatrixLocation;

    public:
        EMCookTorranceDirectedLightMaterial ();
        ~EMCookTorranceDirectedLightMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetAlbedoTexture ( GXTexture2D &texture );
        GXVoid SetNormalTexture ( GXTexture2D &texture );
        GXVoid SetEmissionTexture ( GXTexture2D &texture );
        GXVoid SetParameterTexture ( GXTexture2D &texture );
        GXVoid SetDepthTexture ( GXTexture2D &texture );

        GXVoid SetLightDirectionView ( const GXVec3 &direction );
        GXVoid SetHue ( GXUByte red, GXUByte green, GXUByte blue );
        GXVoid SetIntencity ( GXFloat intencity );
        GXVoid SetAmbientColor ( const GXColorRGB &color );

    private:
        EMCookTorranceDirectedLightMaterial ( const EMCookTorranceDirectedLightMaterial &other ) = delete;
        EMCookTorranceDirectedLightMaterial& operator = ( const EMCookTorranceDirectedLightMaterial &other ) = delete;
};


#endif // EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL
