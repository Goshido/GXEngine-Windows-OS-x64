#ifndef EM_HUE_CIRCLE_GENERATOR_MATERIAL
#define EM_HUE_CIRCLE_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMHueCircleGeneratorMaterial : public GXMaterial
{
	private:
		GXFloat		innerRadius;
		GLint		innerRadiusLocation;

		GXFloat		outerRadius;
		GLint		outerRadiusLocation;

		GXVec2		halfResolution;
		GLint		halfResolutionLocation;

	public:
		EMHueCircleGeneratorMaterial ();
		~EMHueCircleGeneratorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetInnerRadius ( GXFloat radius );
		GXVoid SetOuterRadius ( GXFloat radius );

		GXVoid SetResolution ( GXUShort width, GXUShort height );
};


#endif //EM_HUE_CIRCLE_GENERATOR_MATERIAL
