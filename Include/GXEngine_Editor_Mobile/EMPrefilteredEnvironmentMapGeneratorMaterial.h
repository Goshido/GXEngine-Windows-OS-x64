#ifndef EM_PREFILTERED_ENVIRONMENT_MAP_GENERATOR_MATERIAL
#define EM_PREFILTERED_ENVIRONMENT_MAP_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>


class EMPrefilteredEnvironmentMapGeneratorMaterial : public GXMaterial
{
	private:
		GXTextureCubeMap*		environmentMap;

		GXMat4					viewProjectionMatrices[ 6 ];
		GLint					viewProjectionMatricesLocation;

		GXFloat					roughness;
		GLint					roughnessLocation;

		GXUInt					totalSamples;
		GLint					totalSamplesLocation;

		GXFloat					inverseTotalSamples;
		GLint					inverseTotalSamplesLocation;

	public:
		EMPrefilteredEnvironmentMapGeneratorMaterial ();
		~EMPrefilteredEnvironmentMapGeneratorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetEnvironmentMap ( GXTextureCubeMap &cubeMap );

		// Remapping to [0.0f, 1.0f] and take square.
		GXVoid SetRoughness ( GXFloat newRoughness );

		GXVoid SetTotalSamples ( GXUShort samples );
};


#endif //EM_PREFILTERED_ENVIRONMENT_MAP_GENERATOR_MATERIAL
