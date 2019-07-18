#ifndef EM_CHECKER_GENERATOR_MATERIAL
#define EM_CHECKER_GENERATOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMCheckerGeneratorMaterial final : public GXMaterial
{
    private:
        GXColorRGB      _colorOne;
        GXColorRGB      _colorTwo;
        GXVec2          _elementSize;
        GXVec2          _doubleElementSize;

        GLint           _colorOneLocation;
        GLint           _colorTwoLocation;
        GLint           _elementSizeLocation;
        GLint           _doubleElementSizeLocation;

    public:
        EMCheckerGeneratorMaterial ();
        ~EMCheckerGeneratorMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetColorOne ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
        GXVoid SetColorTwo ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
        GXVoid SetElementSize ( GXUShort width, GXUShort height );

    private:
        EMCheckerGeneratorMaterial ( const EMCheckerGeneratorMaterial &other ) = delete;
        EMCheckerGeneratorMaterial& operator = ( const EMCheckerGeneratorMaterial &other ) = delete;
};


#endif // EM_CHECKER_GENERATOR_MATERIAL
