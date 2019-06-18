#include <GXEngine_Editor_Mobile/EMCheckerGeneratorMaterial.h>


#define VERTEX_SHADER		L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/Checker_fs.txt"

#define DEFAULT_COLOR_ONE_R		0u
#define DEFAULT_COLOR_ONE_G		0u
#define DEFAULT_COLOR_ONE_B		0u
#define DEFAULT_COLOR_ONE_A		255u

#define DEFAULT_COLOR_TWO_R		255u
#define DEFAULT_COLOR_TWO_G		255u
#define DEFAULT_COLOR_TWO_B		255u
#define DEFAULT_COLOR_TWO_A		255u

#define DEFAULT_ELEMENT_WIDTH	4u
#define DEFAULT_ELEMENT_HEIGHT	4u

//---------------------------------------------------------------------------------------------------------------------

EMCheckerGeneratorMaterial::EMCheckerGeneratorMaterial ():
	colorOne ( static_cast<GXUByte> ( DEFAULT_COLOR_ONE_R ), static_cast<GXUByte> ( DEFAULT_COLOR_ONE_G ), static_cast<GXUByte> ( DEFAULT_COLOR_ONE_G ), static_cast<GXUByte> ( DEFAULT_COLOR_ONE_A ) ),
	colorTwo ( static_cast<GXUByte> ( DEFAULT_COLOR_TWO_R ), static_cast<GXUByte> ( DEFAULT_COLOR_TWO_G ), static_cast<GXUByte> ( DEFAULT_COLOR_TWO_B ), static_cast<GXUByte> ( DEFAULT_COLOR_TWO_A ) ),
	elementSize ( static_cast<GXFloat> ( DEFAULT_ELEMENT_WIDTH ), static_cast<GXFloat> ( DEFAULT_ELEMENT_HEIGHT ) ),
	doubleElementSize ( DEFAULT_ELEMENT_WIDTH * 2.0f, DEFAULT_ELEMENT_HEIGHT * 2.0f )
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

	colorOneLocation = _shaderProgram.GetUniform ( "colorOne" );
	colorTwoLocation = _shaderProgram.GetUniform ( "colorTwo" );
	elementSizeLocation = _shaderProgram.GetUniform ( "elementSize" );
	doubleElementSizeLocation = _shaderProgram.GetUniform ( "doubleElementSize" );
}

EMCheckerGeneratorMaterial::~EMCheckerGeneratorMaterial ()
{
	// NOTHING
}

GXVoid EMCheckerGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	glUseProgram ( _shaderProgram.GetProgram () );

	glUniform4fv ( colorOneLocation, 1, colorOne.data );
	glUniform4fv ( colorTwoLocation, 1, colorTwo.data );
	glUniform2fv ( elementSizeLocation, 1, elementSize.data );
	glUniform2fv ( doubleElementSizeLocation, 1, doubleElementSize.data );
}

GXVoid EMCheckerGeneratorMaterial::Unbind ()
{
	glUseProgram ( 0u );
}

GXVoid EMCheckerGeneratorMaterial::SetColorOne ( GXUChar red, GXUChar green, GXUChar blue, GXUChar alpha )
{
	colorOne.From ( red, green, blue, alpha );
}

GXVoid EMCheckerGeneratorMaterial::SetColorTwo ( GXUChar red, GXUChar green, GXUChar blue, GXUChar alpha )
{
	colorTwo.From ( red, green, blue, alpha );
}

GXVoid EMCheckerGeneratorMaterial::SetElementSize ( GXUShort width, GXUShort height )
{
	elementSize.Init ( (GXFloat)width, (GXFloat)height );
	doubleElementSize.Multiply ( elementSize, 2.0f );
}
