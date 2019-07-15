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
        EMLightProbe*                                   _next;
        EMLightProbe*                                   _previous;

        GXVec3                                          _locationWorld;
        GXAABB                                          _boundsWorld;

        GXTextureCubeMap*                               _environmentMap;

        GXTextureCubeMap                                _diffuseIrradiance;
        EMDiffuseIrradianceGeneratorMaterial            _diffuseIrradianceGeneratorMaterial;

        GXTextureCubeMap                                _prefilteredEnvironmentMap;
        EMPrefilteredEnvironmentMapGeneratorMaterial    _prefilteredEnvironmentMapGeneratorMaterial;

        static GXTexture2D                              _brdfIntegrationMap;
        EMBRDFIntegratorMaterial                        _brdfIntegratorMaterial;

        GXMeshGeometry                                  _cube;
        GXMeshGeometry                                  _screenQuad;

        static EMLightProbe*                            _probes;

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
