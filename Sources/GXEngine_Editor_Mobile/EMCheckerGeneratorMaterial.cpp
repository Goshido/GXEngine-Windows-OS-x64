#include <GXEngine_Editor_Mobile/EMCheckerGeneratorMaterial.h>


#define VERTEX_SHADER		L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/Checker_fs.txt"

#define DEFAULT_COLOR_ONE_R		0
#define DEFAULT_COLOR_ONE_G		0
#define DEFAULT_COLOR_ONE_B		0
#define DEFAULT_COLOR_ONE_A		255

#define DEFAULT_COLOR_TWO_R		255
#define DEFAULT_COLOR_TWO_G		255
#define DEFAULT_COLOR_TWO_B		255
#define DEFAULT_COLOR_TWO_A		255

#define DEFAULT_ELEMENT_WIDTH	4
#define DEFAULT_ELEMENT_HEIGHT	4


EMCheckerGeneratorMaterial::EMCheckerGeneratorMaterial ()
{
	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 0;
	si.samplerNames = nullptr;
	si.samplerLocations = nullptr;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	colorOneLocation = shaderProgram.GetUniform ( "colorOne" );
	colorTwoLocation = shaderProgram.GetUniform ( "colorTwo" );
	elementSizeLocation = shaderProgram.GetUniform ( "elementSize" );
	doubleElementSizeLocation = shaderProgram.GetUniform ( "doubleElementSize" );

	SetColorOne ( DEFAULT_COLOR_ONE_R, DEFAULT_COLOR_ONE_G, DEFAULT_COLOR_ONE_G, DEFAULT_COLOR_ONE_A );
	SetColorTwo ( DEFAULT_COLOR_TWO_R, DEFAULT_COLOR_TWO_G, DEFAULT_COLOR_TWO_B, DEFAULT_COLOR_TWO_A );

	SetElementSize ( DEFAULT_ELEMENT_WIDTH, DEFAULT_ELEMENT_HEIGHT );
}

EMCheckerGeneratorMaterial::~EMCheckerGeneratorMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMCheckerGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	glUseProgram ( shaderProgram.GetProgram () );

	glUniform4fv ( colorOneLocation, 1, colorOne.arr );
	glUniform4fv ( colorTwoLocation, 1, colorTwo.arr );
	glUniform2fv ( elementSizeLocation, 1, elementSize.arr );
	glUniform2fv ( doubleElementSizeLocation, 1, doubleElementSize.arr );
}

GXVoid EMCheckerGeneratorMaterial::Unbind ()
{
	glUseProgram ( 0 );
}

GXVoid EMCheckerGeneratorMaterial::SetColorOne ( GXUChar red, GXUChar green, GXUChar blue, GXUChar alpha )
{
	GXColorToVec4 ( colorOne, red, green, blue, alpha );
}

GXVoid EMCheckerGeneratorMaterial::SetColorTwo ( GXUChar red, GXUChar green, GXUChar blue, GXUChar alpha )
{
	GXColorToVec4 ( colorTwo, red, green, blue, alpha );
}

GXVoid EMCheckerGeneratorMaterial::SetElementSize ( GXUShort width, GXUShort height )
{
	elementSize.x = (GXFloat)width;
	elementSize.y = (GXFloat)height;

	doubleElementSize.x = elementSize.x * 2.0f;
	doubleElementSize.y = elementSize.y * 2.0f;
}
