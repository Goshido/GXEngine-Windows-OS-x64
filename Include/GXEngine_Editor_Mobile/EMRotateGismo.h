#ifndef EM_ROTATE_GISMO
#define EM_ROTATE_GISMO


#include "EMMesh.h"
#include <GXEngine/GXTexture.h>
#include <GXEngine/GXUnlitTexture2DMaterial.h>


class EMRotateGismo : public GXRenderable
{
	private:
		GXBool						isVisible;
		EMMesh						mesh;
		GXUnlitTexture2DMaterial	unlitMaterial;
		GXTexture					texture;
		GLuint						sampler;

	public:
		EMRotateGismo ();
		~EMRotateGismo () override;

		GXVoid Hide ();
		GXVoid Show ();

		GXVoid Render () override;

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid UpdateBounds () override;
};


#endif //EM_ROTATE_GISMO
