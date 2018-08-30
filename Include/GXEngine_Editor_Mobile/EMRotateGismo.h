#ifndef EM_ROTATE_GISMO
#define EM_ROTATE_GISMO


#include "EMMesh.h"
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXUnlitTexture2DMaterial.h>


class EMRotateGismo final : public GXTransform
{
	private:
		GXBool						isVisible;
		EMMesh						mesh;
		GXUnlitTexture2DMaterial	unlitMaterial;
		GXTexture2D					texture;

	public:
		EMRotateGismo ();
		~EMRotateGismo () override;

		GXVoid Hide ();
		GXVoid Show ();

		GXVoid Render ();

	protected:
		GXVoid TransformUpdated () override;

	private:
		EMRotateGismo ( const EMRotateGismo &other ) = delete;
		EMRotateGismo& operator = ( const EMRotateGismo &other ) = delete;
};


#endif // EM_ROTATE_GISMO
