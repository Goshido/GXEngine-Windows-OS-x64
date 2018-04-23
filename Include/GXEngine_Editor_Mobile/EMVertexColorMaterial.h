#ifndef EM_VERTEX_COLOR_MATERIAL
#define EM_VERTEX_COLOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMVertexColorMaterial : public GXMaterial
{
	private:
		GLint		currentFrameModelViewProjectionMatrixLocation;

	public:
		EMVertexColorMaterial ();
		~EMVertexColorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;
};


#endif // EM_VERTEX_COLOR_MATERIAL
