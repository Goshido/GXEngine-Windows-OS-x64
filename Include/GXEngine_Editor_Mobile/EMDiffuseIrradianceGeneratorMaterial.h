#ifndef EM_DIFFUSE_IRRADIANCE_GENERATOR_MATERIAL
#define EM_DIFFUSE_IRRADIANCE_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>


class EMDiffuseIrradianceGeneratorMaterial : public GXMaterial
{
	private:
		GXTextureCubeMap*		environmentMap;
		GXFloat					angleStep;

		GXMat4					viewProjectionMatrices[ 6 ];

		GLint					angleStepLocation;
		GLint					viewProjectionMatricesLocation;

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
