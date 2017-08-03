//version 1.0

#ifndef GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL
#define GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"


class GXEquirectangularToCubeMapMaterial : public GXMaterial
{
	private:
		GXTexture2D*	texture;

		GXMat4			viewProjectionMatrices[ 6 ];
		GLint			viewProjectionMatricesLocation;

	public:
		GXEquirectangularToCubeMapMaterial ();
		~GXEquirectangularToCubeMapMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetEquirectangularTexture ( GXTexture2D &texture );
};


#endif //GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL
