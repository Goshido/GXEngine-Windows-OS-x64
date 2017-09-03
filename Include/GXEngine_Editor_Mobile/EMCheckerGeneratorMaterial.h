#ifndef EM_CHECKER_GENERATOR_MATERIAL
#define EM_CHECKER_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMCheckerGeneratorMaterial : public GXMaterial
{
	private:
		GXVec4		colorOne;
		GLint		colorOneLocation;

		GXVec4		colorTwo;
		GLint		colorTwoLocation;

		GXVec2		elementSize;
		GLint		elementSizeLocation;

		GXVec2		doubleElementSize;
		GLint		doubleElementSizeLocation;

	public:
		EMCheckerGeneratorMaterial ();
		~EMCheckerGeneratorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetColorOne ( GXUChar red, GXUChar green, GXUChar blue, GXUChar alpha );
		GXVoid SetColorTwo ( GXUChar red, GXUChar green, GXUChar blue, GXUChar alpha );
		GXVoid SetElementSize ( GXUShort width, GXUShort height );
};


#endif //EM_CHECKER_GENERATOR_MATERIAL
