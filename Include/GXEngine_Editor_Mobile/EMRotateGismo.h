#ifndef EM_ROTATE_GISMO
#define EM_ROTATE_GISMO


#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXTexture.h>


class EMRotateGismo : public GXRenderable
{
	private:
		GXBool				isDeleted;
		GXBool				isVisible;

		GXShaderProgram		shaderProgram;
		GLint				mod_view_proj_matLocation;

		GXMeshGeometry		meshGeometry;
		GXTexture			texture;
		GLuint				sampler;

	public:
		EMRotateGismo ();
		GXVoid Delete ();

		GXVoid Hide ();
		GXVoid Show ();

		GXVoid Render () override;

	protected:
		~EMRotateGismo () override;

		GXVoid InitGraphicResources () override;
		GXVoid UpdateBounds () override;
};


#endif //EM_ROTATE_GISMO
