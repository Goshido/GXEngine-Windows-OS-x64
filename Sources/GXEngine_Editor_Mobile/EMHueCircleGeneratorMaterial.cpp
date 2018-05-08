#include <GXEngine_Editor_Mobile/EMHueCircleGeneratorMaterial.h>


#define DEFAULT_INNER_RADIUS		35.0f
#define DEFAULT_OUTER_RADIUS		50.0f

#define DEFAULT_RESOLUTION_WIDTH	1280.0f
#define DEFAULT_RESOLUTION_HEIGHT	720.0f

#define VERTEX_SHADER				L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER				nullptr
#define FRAGMENT_SHADER				L"Shaders/Editor Mobile/HueCircle_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMHueCircleGeneratorMaterial::EMHueCircleGeneratorMaterial ():
	innerRadius ( DEFAULT_INNER_RADIUS ),
	outerRadius ( DEFAULT_OUTER_RADIUS ),
	halfResolution ( DEFAULT_RESOLUTION_WIDTH * 0.5f, DEFAULT_RESOLUTION_HEIGHT * 0.5f )
{
	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 0u;
	si.samplerNames = nullptr;
	si.samplerLocations = nullptr;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	innerRadiusLocation = shaderProgram.GetUniform ( "innerRadius" );
	outerRadiusLocation = shaderProgram.GetUniform ( "outerRadius" );
	halfResolutionLocation = shaderProgram.GetUniform ( "halfResolution" );
}

EMHueCircleGeneratorMaterial::~EMHueCircleGeneratorMaterial ()
{
	// NOTHING
}

GXVoid EMHueCircleGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	glUseProgram ( shaderProgram.GetProgram () );

	glUniform1f ( innerRadiusLocation, innerRadius );
	glUniform1f ( outerRadiusLocation, outerRadius );
	glUniform2fv ( halfResolutionLocation, 1, halfResolution.data );
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
	halfResolution.data[ 0 ] = width * 0.5f;
	halfResolution.data[ 1 ] = height * 0.5f;
}
