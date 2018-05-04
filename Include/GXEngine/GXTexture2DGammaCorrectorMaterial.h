// version 1.1

#ifndef GX_TEXUTE_2D_GAMMA_CORRECTOR_MATERIAL
#define GX_TEXUTE_2D_GAMMA_CORRECTOR_MATERIAL


#include "GXMaterial.h"
#include "GXTexture2D.h"


class GXTexture2DGammaCorrectorMaterial : public GXMaterial
{
	private:
		GXTexture2D*		sRGBTexture;
		GLint				gammaLocation;

	public:
		GXTexture2DGammaCorrectorMaterial ();
		~GXTexture2DGammaCorrectorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetsRGBTexture ( GXTexture2D &texture );

	private:
		GXTexture2DGammaCorrectorMaterial ( const GXTexture2DGammaCorrectorMaterial &other ) = delete;
		GXTexture2DGammaCorrectorMaterial& operator = ( const GXTexture2DGammaCorrectorMaterial &other ) = delete;
};


#endif // GX_TEXUTE_2D_GAMMA_CORRECTOR_MATERIAL
