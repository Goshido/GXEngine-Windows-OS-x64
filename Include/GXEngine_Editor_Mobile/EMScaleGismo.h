#ifndef EM_SCALE_GISMO
#define EM_SCALE_GISMO


#include "EMMesh.h"
#include <GXEngine/GXTexture.h>
#include <GXEngine/GXUnlitTexture2DMaterial.h>


class EMScaleGismo : public GXRenderable
{
	private:
		GXBool						isVisible;
		EMMesh						mesh;
		GXTexture					texture;
		GXUnlitTexture2DMaterial	unlitMaterial;
		GLuint						sampler;

	public:
		EMScaleGismo ();
		~EMScaleGismo () override;

		GXVoid Hide ();
		GXVoid Show ();

		GXVoid Render () override;

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid UpdateBounds () override;
};


#endif //EM_SCALE_GISMO
