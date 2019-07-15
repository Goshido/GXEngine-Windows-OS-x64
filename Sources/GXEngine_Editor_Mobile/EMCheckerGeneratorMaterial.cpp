#include <GXEngine_Editor_Mobile/EMCheckerGeneratorMaterial.h>


#define VERTEX_SHADER               L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER             nullptr
#define FRAGMENT_SHADER             L"Shaders/Editor Mobile/Checker_fs.txt"

#define DEFAULT_COLOR_ONE_R         0u
#define DEFAULT_COLOR_ONE_G         0u
#define DEFAULT_COLOR_ONE_B         0u
#define DEFAULT_COLOR_ONE_A         255u

#define DEFAULT_COLOR_TWO_R         255u
#define DEFAULT_COLOR_TWO_G         255u
#define DEFAULT_COLOR_TWO_B         255u
#define DEFAULT_COLOR_TWO_A         255u

#define DEFAULT_ELEMENT_WIDTH       4u
#define DEFAULT_ELEMENT_HEIGHT      4u

//---------------------------------------------------------------------------------------------------------------------

EMCheckerGeneratorMaterial::EMCheckerGeneratorMaterial ():
    _colorOne ( static_cast<GXUByte> ( DEFAULT_COLOR_ONE_R ), static_cast<GXUByte> ( DEFAULT_COLOR_ONE_G ), static_cast<GXUByte> ( DEFAULT_COLOR_ONE_G ), static_cast<GXUByte> ( DEFAULT_COLOR_ONE_A ) ),
    _colorTwo ( static_cast<GXUByte> ( DEFAULT_COLOR_TWO_R ), static_cast<GXUByte> ( DEFAULT_COLOR_TWO_G ), static_cast<GXUByte> ( DEFAULT_COLOR_TWO_B ), static_cast<GXUByte> ( DEFAULT_COLOR_TWO_A ) ),
    _elementSize ( static_cast<GXFloat> ( DEFAULT_ELEMENT_WIDTH ), static_cast<GXFloat> ( DEFAULT_ELEMENT_HEIGHT ) ),
    _doubleElementSize ( DEFAULT_ELEMENT_WIDTH * 2.0f, DEFAULT_ELEMENT_HEIGHT * 2.0f )
{
    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 0u;
    si._samplerNames = nullptr;
    si._samplerLocations = nullptr;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _colorOneLocation = _shaderProgram.GetUniform ( "colorOne" );
    _colorTwoLocation = _shaderProgram.GetUniform ( "colorTwo" );
    _elementSizeLocation = _shaderProgram.GetUniform ( "elementSize" );
    _doubleElementSizeLocation = _shaderProgram.GetUniform ( "doubleElementSize" );
}

EMCheckerGeneratorMaterial::~EMCheckerGeneratorMaterial ()
{
    // NOTHING
}

GXVoid EMCheckerGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
    glUseProgram ( _shaderProgram.GetProgram () );

    glUniform4fv ( _colorOneLocation, 1, _colorOne._data );
    glUniform4fv ( _colorTwoLocation, 1, _colorTwo._data );
    glUniform2fv ( _elementSizeLocation, 1, _elementSize._data );
    glUniform2fv ( _doubleElementSizeLocation, 1, _doubleElementSize._data );
}

GXVoid EMCheckerGeneratorMaterial::Unbind ()
{
    glUseProgram ( 0u );
}

GXVoid EMCheckerGeneratorMaterial::SetColorOne ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    _colorOne.From ( red, green, blue, alpha );
}

GXVoid EMCheckerGeneratorMaterial::SetColorTwo ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    _colorTwo.From ( red, green, blue, alpha );
}

GXVoid EMCheckerGeneratorMaterial::SetElementSize ( GXUShort width, GXUShort height )
{
    _elementSize.Init ( (GXFloat)width, (GXFloat)height );
    _doubleElementSize.Multiply ( _elementSize, 2.0f );
}
