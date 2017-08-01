#ifndef EM_DIFFUSE_IRRADIANCE_GENERATOR_MATERIAL
#define EM_DIFFUSE_IRRADIANCE_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>


class EMDiffuseIrradianceGeneratorMaterial : public GXMaterial
{
	private:
		GXTextureCubeMap*		environmentMap;

		GXMat4					modelProjectionMatrices[ 6 ];
		GLint					modelProjectionMatricesLocation;

		GXFloat					angleStep;
		GLint					angleStepLocation;

	public:
		EMDiffuseIrradianceGeneratorMaterial ();
		~EMDiffuseIrradianceGeneratorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetEnvironmentMap ( GXTextureCubeMap &cubeMap );

		// Less step - more precisely result convolution.
		// Returns total samples will be done.
		GXUInt SetAngleStep ( GXFloat radians );
};


#endif //EM_DIFFUSE_IRRADIANCE_GENERATOR_MATERIAL
