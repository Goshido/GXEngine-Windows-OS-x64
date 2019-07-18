#ifndef EM_DIFFUSE_IRRADIANCE_GENERATOR_MATERIAL
#define EM_DIFFUSE_IRRADIANCE_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXSampler.h>


class EMDiffuseIrradianceGeneratorMaterial final : public GXMaterial
{
    private:
        GXTextureCubeMap*       _environmentMap;
        GXSampler               _sampler;
        GXFloat                 _angleStep;

        GXMat4                  _viewProjectionMatrices[ 6u ];

        GLint                   _angleStepLocation;
        GLint                   _viewProjectionMatricesLocation;

    public:
        EMDiffuseIrradianceGeneratorMaterial ();
        ~EMDiffuseIrradianceGeneratorMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetEnvironmentMap ( GXTextureCubeMap &cubeMap );

        // Less step - more precisely convolution result.
        // Returns total samples will be done.
        GXUInt SetAngleStep ( GXFloat radians );

    private:
        EMDiffuseIrradianceGeneratorMaterial ( const EMDiffuseIrradianceGeneratorMaterial &other ) = delete;
        EMDiffuseIrradianceGeneratorMaterial& operator = ( const EMDiffuseIrradianceGeneratorMaterial &other ) = delete;
};


#endif // EM_DIFFUSE_IRRADIANCE_GENERATOR_MATERIAL
