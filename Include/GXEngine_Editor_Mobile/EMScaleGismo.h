#ifndef EM_SCALE_GISMO
#define EM_SCALE_GISMO


#include <GXEngine/GXMesh.h>
#include <GXEngine/GXTextureStorage.h>


class EMScaleGismo : public GXMesh
{
	private:
		GXBool		isDeleted;
		GXBool		isVisible;

		GLint		mod_view_proj_matLocation;

		GXTexture	texture;

	public:
		EMScaleGismo ();
		GXVoid Delete ();

		GXVoid Hide ();
		GXVoid Show ();

		virtual GXVoid Draw ();

	protected:
		~EMScaleGismo ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //EM_SCALE_GISMO
