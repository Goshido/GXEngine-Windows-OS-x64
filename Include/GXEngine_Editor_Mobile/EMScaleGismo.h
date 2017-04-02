#ifndef EM_SCALE_GISMO
#define EM_SCALE_GISMO


#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXTexture.h>


class EMScaleGismo : public GXRenderable
{
	private:
		GXBool				isDeleted;
		GXBool				isVisible;

		GLint				mod_view_proj_matLocation;
		GXShaderProgram		shaderProgram;

		GXMeshGeometry		meshGeometry;
		GXTexture			texture;

	public:
		EMScaleGismo ();
		GXVoid Delete ();

		GXVoid Hide ();
		GXVoid Show ();

		GXVoid Render () override;

	protected:
		~EMScaleGismo () override;

		GXVoid InitGraphicResources () override;
		GXVoid UpdateBounds () override;
};


#endif //EM_SCALE_GISMO
