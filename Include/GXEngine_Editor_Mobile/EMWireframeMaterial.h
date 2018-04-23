#ifndef EM_WIREFRAME_MATERIAL
#define EM_WIREFRAME_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMWireframeMaterial : public GXMaterial
{
	private:
		GLint		currentFrameModelViewProjectionMatrixLocation;

		GXColorRGB	color;
		GLint		colorLocation;

	public:
		EMWireframeMaterial ();
		~EMWireframeMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
};


#endif // EM_WIREFRAME_MATERIAL
