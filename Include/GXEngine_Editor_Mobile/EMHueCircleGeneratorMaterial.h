#ifndef EM_HUE_CIRCLE_GENERATOR_MATERIAL
#define EM_HUE_CIRCLE_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMHueCircleGeneratorMaterial : public GXMaterial
{
	private:
		GXFloat		innerRadius;
		GXFloat		outerRadius;
		GXVec2		halfResolution;

		GLint		innerRadiusLocation;
		GLint		outerRadiusLocation;
		GLint		halfResolutionLocation;

	public:
		EMHueCircleGeneratorMaterial ();
		~EMHueCircleGeneratorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetInnerRadius ( GXFloat radius );
		GXVoid SetOuterRadius ( GXFloat radius );

		GXVoid SetResolution ( GXUShort width, GXUShort height );

	private:
		EMHueCircleGeneratorMaterial ( const EMHueCircleGeneratorMaterial &other ) = delete;
		EMHueCircleGeneratorMaterial& operator = ( const EMHueCircleGeneratorMaterial &other ) = delete;
};


#endif // EM_HUE_CIRCLE_GENERATOR_MATERIAL
