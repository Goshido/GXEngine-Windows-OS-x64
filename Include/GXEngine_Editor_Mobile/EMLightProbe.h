#ifndef EM_LIGHT_PROBE
#define EM_LIGHT_PROBE


#include "EMDiffuseIrradianceGeneratorMaterial.h"
#include "EMPrefilteredEnvironmentMapGeneratorMaterial.h"
#include "EMBRDFIntegratorMaterial.h"
#include <GXEngine/GXMeshGeometry.h>


class EMLightProbe final
{
    friend class EMRenderer;

    private:
        EMLightProbe*                                   next;
        EMLightProbe*                                   prev;

        GXVec3                                          locationWorld;
        GXAABB                                          boundsWorld;

        GXTextureCubeMap*                               environmentMap;

        GXTextureCubeMap                                diffuseIrradiance;
        EMDiffuseIrradianceGeneratorMaterial            diffuseIrradianceGeneratorMaterial;

        GXTextureCubeMap                                prefilteredEnvironmentMap;
        EMPrefilteredEnvironmentMapGeneratorMaterial    prefilteredEnvironmentMapGeneratorMaterial;

        static GXTexture2D                              brdfIntegrationMap;
        EMBRDFIntegratorMaterial                        brdfIntegratorMaterial;

        GXMeshGeometry                                  cube;
        GXMeshGeometry                                  screenQuad;

        static EMLightProbe*                            probes;

    public:
        EMLightProbe ();
        ~EMLightProbe ();

        GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
        GXVoid SetEnvironmentMap ( GXTextureCubeMap &cubeMap );
        GXVoid SetBoundLocal ( GXFloat xRange, GXFloat yRange, GXFloat zRange );

        // Less step - more precisely convolution result.
        // Returns total samples will be done.
        GXUInt SetDiffuseIrradianceConvolutionAngleStep ( GXFloat radians );

        // This is 2D square texture
        GXVoid SetDiffuseIrradianceResolution ( GXUShort resolution );

        // This texture is square 2D texture.
        GXVoid SetBRDFIntegrationMapResolution ( GXUShort length );

        GXVoid SetBRDFIntegrationMapSamplesPerPixel ( GXUShort samples );

        GXTextureCubeMap& GetDiffuseIrradiance ();
        GXTextureCubeMap& GetPrefilteredEnvironmentMap ();
        GXTexture2D& GetBRDFIntegrationMap ();

    private:
        GXVoid UpdateDiffuseIrradiance ();
        GXVoid UpdatePrefilteredEnvironmentMap ();
        GXVoid UpdateBRDFIntegrationMap ();

        static EMLightProbe* GetProbes ();

        EMLightProbe ( const EMLightProbe &other ) = delete;
        EMLightProbe& operator = ( const EMLightProbe &other ) = delete;
};


#endif // EM_LIGHT_PROBE
