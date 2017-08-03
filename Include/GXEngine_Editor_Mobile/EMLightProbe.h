#ifndef EM_LIGHT_PROBE
#define EM_LIGHT_PROBE


#include "EMDiffuseIrradianceGeneratorMaterial.h"
#include "EMPrefilteredEnvironmentMapGeneratorMaterial.h"
#include "EMBRDFIntegratorMaterial.h"
#include <GXEngine/GXMeshGeometry.h>


class EMLightProbe
{
	private:
		GXVec3											locationWorld;
		GXAABB											boundsWorld;

		GXTextureCubeMap								environmentMap;

		GXTextureCubeMap								diffuseIrradiance;
		EMDiffuseIrradianceGeneratorMaterial			diffuseIrradianceGeneratorMaterial;

		GXTextureCubeMap								prefilteredEnvironmentMap;
		EMPrefilteredEnvironmentMapGeneratorMaterial	prefilteredEnvironmentMapGeneratorMaterial;

		GXTexture2D										brdfIntegrationMap;
		EMBRDFIntegratorMaterial						brdfIntegratorMaterial;

		GXMeshGeometry									cube;
		GXMeshGeometry									screenQuad;

	public:
		EMLightProbe ();
		~EMLightProbe ();

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetEnvironmentMap ( GXTextureCubeMap &cubeMap );
		GXVoid SetBoundLocal ( GXFloat xRange, GXFloat yRange, GXFloat zRange );

		// Less step - more precisely convolution result.
		// Returns total samples will be done.
		GXUInt SetDiffuseIrradianceConvolutionAngleStep ( GXFloat radians );

		// This texture is square 2D texture.
		GXVoid SetBRDFIntegrationMapResolution ( GXUShort length );

		GXVoid SetBRDFIntegrationMapSamples ( GXUShort samples );

		GXTextureCubeMap& GetDiffuseIrradiance ();
		GXTextureCubeMap& GetPrefilteredEnvironmentMap ();
		GXTexture2D& GetBRDFIntegrationMap ();

	private:
		GXVoid UpdateDiffuseIrradiance ();
		GXVoid UpdatePrefilteredEnvironmentMap ();
		GXVoid UpdateBRDFIntegrationMap ();
};


#endif //EM_LIGHT_PROBE
