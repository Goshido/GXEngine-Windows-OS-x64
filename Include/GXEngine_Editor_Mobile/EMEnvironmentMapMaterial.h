#ifndef EM_ENVIRONMENT_MAP_MATERIAL
#define EM_ENVIRONMENT_MAP_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXTexture2D.h>


class EMEnvironmentMapMaterial : public GXMaterial
{
	private:
		GXTextureCubeMap*		environmentTexture;
		GXTexture2D*			depthTexture;

		GLint					modelViewProjectionMatrixLocation;

		GXVec2					screenResolution;
		GXVec2					inverseScreenResolution;
		GLint					inverseScreenResolutionLocation;

		GLint					velocityBlurLocation;

		GXFloat					inverseDeltaTime;
		GXFloat					environmentQuasiDistance;

	public:
		EMEnvironmentMapMaterial ();
		~EMEnvironmentMapMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetEnvironmentMap ( GXTextureCubeMap &cubeMap );
		GXVoid SetDepthTexture ( GXTexture2D &texture );

		GXVoid SetScreenResolution ( GXUShort width, GXUShort height );
		GXVoid SetDeltaTime ( GXFloat seconds );

		// Distance to environment map border to calculate positions and then velocity blur.
		GXVoid SetEnvironmentQuasiDistance ( GXFloat meters );
};


#endif // EM_ENVIRONMENT_MAP_MATERIAL
