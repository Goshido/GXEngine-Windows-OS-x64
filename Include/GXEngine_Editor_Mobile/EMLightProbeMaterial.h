#ifndef EM_LIGHT_PROBE_MATERIAL
#define EM_LIGHT_PROBE_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXSampler.h>


class EMLightProbeMaterial final : public GXMaterial
{
    private:
        GXTextureCubeMap*       _diffuseIrradianceTexture;
        GXTextureCubeMap*       _prefilteredEnvironmentMapTexture;
        GXTexture2D*            _brdfIntegrationMapTexture;

        GXTexture2D*            _albedoTexture;
        GXTexture2D*            _normalTexture;
        GXTexture2D*            _emissionTexture;
        GXTexture2D*            _parameterTexture;
        GXTexture2D*            _depthTexture;

        GXSampler               _diffuseIrradianceSampler;
        GXSampler               _prefilteredEnvironmentSampler;
        GXSampler               _brdfIntegrationSampler;
        GXSampler               _gBufferSampler;

        GLint                   _viewerLocationWorldLocation;
        GLint                   _prefilteredEnvironmentMapLODsLocation;
        GLint                   _inverseViewMatrixLocation;
        GLint                   _inverseViewProjectionMatrixLocation;

    public:
        EMLightProbeMaterial ();
        ~EMLightProbeMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetDiffuseIrradianceTexture ( GXTextureCubeMap &cubeMap );
        GXVoid SetPrefilteredEnvironmentMapTexture ( GXTextureCubeMap &cubeMap );
        GXVoid SetBRDFIntegrationMapTexture ( GXTexture2D &texture );
        GXVoid SetAlbedoTexture ( GXTexture2D &texture );
        GXVoid SetNormalTexture ( GXTexture2D &texture );
        GXVoid SetEmissionTexture ( GXTexture2D &texture );
        GXVoid SetParameterTexture ( GXTexture2D &texture );
        GXVoid SetDepthTexture ( GXTexture2D &texture );

    private:
        EMLightProbeMaterial ( const EMLightProbeMaterial &other ) = delete;
        EMLightProbeMaterial& operator = ( const EMLightProbeMaterial &other ) = delete;
};


#endif // EM_LIGHT_PROBE_MATERIAL
