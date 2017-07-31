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

		GXVec2					inverseScreenResolution;
		GLint					inverseScreenResolutionLocation;

	public:
		EMEnvironmentMapMaterial ();
		~EMEnvironmentMapMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetEnvironmentMap ( GXTextureCubeMap &cubeMap );
		GXVoid SetDepthTexture ( GXTexture2D &texture );

		GXVoid SetScreenResolution ( GXUShort width, GXUShort height );
};


#endif //EM_ENVIRONMENT_MAP_MATERIAL
