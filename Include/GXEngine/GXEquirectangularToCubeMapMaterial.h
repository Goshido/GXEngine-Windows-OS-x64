// version 1.2

#ifndef GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL
#define GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"


class GXEquirectangularToCubeMapMaterial : public GXMaterial
{
	private:
		GXTexture2D*	equirectangularTexture;

		GXMat4			viewProjectionMatrices[ 6 ];
		GLint			viewProjectionMatricesLocation;

		GXFloat			gamma;
		GLint			gammaLocation;

	public:
		GXEquirectangularToCubeMapMaterial ();
		~GXEquirectangularToCubeMapMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetEquirectangularTexture ( GXTexture2D &texture );
		GXVoid EnableGammaCorrection ();
		GXVoid DisableGammaCorrection ();

	private:
		GXEquirectangularToCubeMapMaterial ( const GXEquirectangularToCubeMapMaterial &other ) = delete;
		GXEquirectangularToCubeMapMaterial& operator = ( const GXEquirectangularToCubeMapMaterial &other ) = delete;
};


#endif // GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL
