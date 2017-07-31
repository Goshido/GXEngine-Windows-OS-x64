//version 1.0

#ifndef GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL
#define GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"


enum class eGXCubeMapFace : GXInt
{
	PositiveX = 0,
	NegativeX = 1,
	PositiveY = 2,
	NegativeY = 3,
	PositiveZ = 4,
	NegativeZ = 5
};


class GXEquirectangularToCubeMapMaterial : public GXMaterial
{
	private:
		GXTexture2D*	texture;

		GXMat4			viewProjectionMatrix;
		GLint			viewProjectionMatrixLocation;

	public:
		GXEquirectangularToCubeMapMaterial ();
		~GXEquirectangularToCubeMapMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetEquirectangularTexture ( GXTexture2D &texture );
		GXVoid SetSide ( eGXCubeMapFace side );
};


#endif //GX_EQUIRECTANGULAR_TO_CUBE_MAP_MATERIAL
