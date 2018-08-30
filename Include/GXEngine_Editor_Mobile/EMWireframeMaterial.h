#ifndef EM_WIREFRAME_MATERIAL
#define EM_WIREFRAME_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMWireframeMaterial final : public GXMaterial
{
	private:
		GXColorRGB	color;

		GLint		currentFrameModelViewProjectionMatrixLocation;
		GLint		colorLocation;

	public:
		EMWireframeMaterial ();
		~EMWireframeMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );

	private:
		EMWireframeMaterial ( const EMWireframeMaterial &other ) = delete;
		EMWireframeMaterial& operator = ( const EMWireframeMaterial &other ) = delete;
};


#endif // EM_WIREFRAME_MATERIAL
