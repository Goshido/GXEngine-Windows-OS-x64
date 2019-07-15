#ifndef EM_PREFILTERED_ENVIRONMENT_MAP_GENERATOR_MATERIAL
#define EM_PREFILTERED_ENVIRONMENT_MAP_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXSampler.h>


class EMPrefilteredEnvironmentMapGeneratorMaterial final : public GXMaterial
{
    private:
        GXTextureCubeMap*       _environmentMap;
        GXSampler               _sampler;
        GXFloat                 _roughness;

        GXInt                   _totalSamples;
        GXFloat                 _inverseTotalSamples;

        GXMat4                  _viewProjectionMatrices[ 6u ];

        GLint                   _viewProjectionMatricesLocation;
        GLint                   _roughnessLocation;
        GLint                   _totalSamplesLocation;
        GLint                   _inverseTotalSamplesLocation;

    public:
        EMPrefilteredEnvironmentMapGeneratorMaterial ();
        ~EMPrefilteredEnvironmentMapGeneratorMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetEnvironmentMap ( GXTextureCubeMap &cubeMap );

        // Remapping to [0.0f, 1.0f] and take square.
        GXVoid SetRoughness ( GXFloat newRoughness );

        GXVoid SetTotalSamples ( GXUShort samples );

    private:
        EMPrefilteredEnvironmentMapGeneratorMaterial ( const EMPrefilteredEnvironmentMapGeneratorMaterial &other ) = delete;
        EMPrefilteredEnvironmentMapGeneratorMaterial& operator = ( const EMPrefilteredEnvironmentMapGeneratorMaterial &other ) = delete;
};


#endif // EM_PREFILTERED_ENVIRONMENT_MAP_GENERATOR_MATERIAL
