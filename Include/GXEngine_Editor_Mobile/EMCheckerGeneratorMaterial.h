#ifndef EM_CHECKER_GENERATOR_MATERIAL
#define EM_CHECKER_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMCheckerGeneratorMaterial : public GXMaterial
{
	private:
		GXColorRGB	colorOne;
		GXColorRGB	colorTwo;
		GXVec2		elementSize;
		GXVec2		doubleElementSize;

		GLint		colorOneLocation;
		GLint		colorTwoLocation;
		GLint		elementSizeLocation;
		GLint		doubleElementSizeLocation;

	public:
		EMCheckerGeneratorMaterial ();
		~EMCheckerGeneratorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetColorOne ( GXUChar red, GXUChar green, GXUChar blue, GXUChar alpha );
		GXVoid SetColorTwo ( GXUChar red, GXUChar green, GXUChar blue, GXUChar alpha );
		GXVoid SetElementSize ( GXUShort width, GXUShort height );

	private:
		EMCheckerGeneratorMaterial ( const EMCheckerGeneratorMaterial &other ) = delete;
		EMCheckerGeneratorMaterial& operator = ( const EMCheckerGeneratorMaterial &other ) = delete;
};


#endif // EM_CHECKER_GENERATOR_MATERIAL
