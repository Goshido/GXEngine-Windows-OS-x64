#include <GXEngine_Editor_Mobile/EMHueCircleGeneratorMaterial.h>


#define VERTEX_SHADER				L"Shaders/Editor Mobile/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER				nullptr
#define FRAGMENT_SHADER				L"Shaders/Editor Mobile/HueCircle_fs.txt"

#define DEFAULT_INNER_RADIUS		35.0f
#define DEFAULT_OUTER_RADIUS		50.0f

#define DEFAULT_RESOLUTION_WIDTH	1280
#define DEFAULT_RESOLUTION_HEIGHT	720


EMHueCircleGeneratorMaterial::EMHueCircleGeneratorMaterial ()
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

	innerRadiusLocation = shaderProgram.GetUniform ( "innerRadius" );
	outerRadiusLocation = shaderProgram.GetUniform ( "outerRadius" );
	halfResolutionLocation = shaderProgram.GetUniform ( "halfResolution" );

	SetInnerRadius ( DEFAULT_INNER_RADIUS );
	SetOuterRadius ( DEFAULT_OUTER_RADIUS );
	SetResolution ( DEFAULT_RESOLUTION_WIDTH, DEFAULT_RESOLUTION_HEIGHT );
}

EMHueCircleGeneratorMaterial::~EMHueCircleGeneratorMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMHueCircleGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	glUseProgram ( shaderProgram.GetProgram () );

	glUniform1f ( innerRadiusLocation, innerRadius );
	glUniform1f ( outerRadiusLocation, outerRadius );
	glUniform2fv ( halfResolutionLocation, 1, halfResolution.arr );
}

GXVoid EMHueCircleGeneratorMaterial::Unbind ()
{
	glUseProgram ( 0 );
}

GXVoid EMHueCircleGeneratorMaterial::SetInnerRadius ( GXFloat radius )
{
	innerRadius = radius;
}

GXVoid EMHueCircleGeneratorMaterial::SetOuterRadius ( GXFloat radius )
{
	outerRadius = radius;
}

GXVoid EMHueCircleGeneratorMaterial::SetResolution ( GXUShort width, GXUShort height )
{
	halfResolution.x = width * 0.5f;
	halfResolution.y = height * 0.5f;
}
