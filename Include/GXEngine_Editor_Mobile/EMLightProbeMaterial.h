#ifndef EM_LIGHT_PROBE_MATERIAL
#define EM_LIGHT_PROBE_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>


class EMLightProbeMaterial : public GXMaterial
{
	private:
		GXTextureCubeMap*			diffuseIrradianceTexture;
		GXTextureCubeMap*			prefilteredEnvironmentMapTexture;
		GXTexture2D*				brdfIntegrationMapTexture;

		GXTexture2D*				albedoTexture;
		GXTexture2D*				normalTexture;
		GXTexture2D*				emissionTexture;
		GXTexture2D*				parameterTexture;
		GXTexture2D*				depthTexture;

		GLint						viewerLocationWorldLocation;
		GLint						prefilteredEnvironmentMapLODsLocation;
		GLint						inverseViewMatrixLocation;
		GLint						inverseViewProjectionMatrixLocation;

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
