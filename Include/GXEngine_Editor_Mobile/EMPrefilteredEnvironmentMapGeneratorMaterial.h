#ifndef EM_PREFILTERED_ENVIRONMENT_MAP_GENERATOR_MATERIAL
#define EM_PREFILTERED_ENVIRONMENT_MAP_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXSampler.h>


class EMPrefilteredEnvironmentMapGeneratorMaterial final : public GXMaterial
{
	private:
		GXTextureCubeMap*		environmentMap;
		GXSampler				sampler;
		GXFloat					roughness;

		GXInt					totalSamples;
		GXFloat					inverseTotalSamples;

		GXMat4					viewProjectionMatrices[ 6 ];

		GLint					viewProjectionMatricesLocation;
		GLint					roughnessLocation;
		GLint					totalSamplesLocation;
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

	private:
		EMPrefilteredEnvironmentMapGeneratorMaterial ( const EMPrefilteredEnvironmentMapGeneratorMaterial &other ) = delete;
		EMPrefilteredEnvironmentMapGeneratorMaterial& operator = ( const EMPrefilteredEnvironmentMapGeneratorMaterial &other ) = delete;
};


#endif // EM_PREFILTERED_ENVIRONMENT_MAP_GENERATOR_MATERIAL
