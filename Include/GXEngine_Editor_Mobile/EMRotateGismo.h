#ifndef EM_ROTATE_GISMO
#define EM_ROTATE_GISMO


#include <GXEngine/GXMesh.h>
#include <GXEngine/GXTextureStorage.h>


class EMRotateGismo : public GXMesh
{
	private:
		GXBool		isDeleted;
		GXBool		isVisible;

		GLint		mod_view_proj_matLocation;

		GXTexture	texture;

	public:
		EMRotateGismo ();
		GXVoid Delete ();

		GXVoid Hide ();
		GXVoid Show ();

		virtual GXVoid Draw ();

	protected:
		~EMRotateGismo ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //EM_ROTATE_GISMO
