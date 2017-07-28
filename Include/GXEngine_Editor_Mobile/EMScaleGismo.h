#ifndef EM_SCALE_GISMO
#define EM_SCALE_GISMO


#include "EMMesh.h"
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXUnlitTexture2DMaterial.h>


class EMScaleGismo : public GXTransform
{
	private:
		GXBool						isVisible;
		EMMesh						mesh;
		GXTexture2D					texture;
		GXUnlitTexture2DMaterial	unlitMaterial;

	public:
		EMScaleGismo ();
		~EMScaleGismo () override;

		GXVoid Hide ();
		GXVoid Show ();

		GXVoid Render ();

	protected:
		GXVoid TransformUpdated () override;
};


#endif //EM_SCALE_GISMO
