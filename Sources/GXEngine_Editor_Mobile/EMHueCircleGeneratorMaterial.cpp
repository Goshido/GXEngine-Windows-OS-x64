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
	si._vertexShader = VERTEX_SHADER;
	si._geometryShader = GEOMETRY_SHADER;
	si._fragmentShader = FRAGMENT_SHADER;
	si._samplers = 0u;
	si._samplerNames = nullptr;
	si._samplerLocations = nullptr;
	si._transformFeedbackOutputs = 0;
	si._transformFeedbackOutputNames = nullptr;

	_shaderProgram.Init ( si );

	innerRadiusLocation = _shaderProgram.GetUniform ( "innerRadius" );
	outerRadiusLocation = _shaderProgram.GetUniform ( "outerRadius" );
	halfResolutionLocation = _shaderProgram.GetUniform ( "halfResolution" );
}

EMHueCircleGeneratorMaterial::~EMHueCircleGeneratorMaterial ()
{
	// NOTHING
}

GXVoid EMHueCircleGeneratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	glUseProgram ( _shaderProgram.GetProgram () );

	glUniform1f ( innerRadiusLocation, innerRadius );
	glUniform1f ( outerRadiusLocation, outerRadius );
	glUniform2fv ( halfResolutionLocation, 1, halfResolution._data );
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
	halfResolution._data[ 0 ] = width * 0.5f;
	halfResolution._data[ 1 ] = height * 0.5f;
}
