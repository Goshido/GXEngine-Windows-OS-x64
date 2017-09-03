#include <GXEngine_Editor_Mobile/EMBRDFIntegratorMaterial.h>


#define VERTEX_SHADER				L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER				nullptr
#define FRAGMENT_SHADER				L"Shaders/Editor Mobile/BRDFIntegrator_fs.txt"

#define DEFAULT_SAMPLES_PER_PIXEL	1024


EMBRDFIntegratorMaterial::EMBRDFIntegratorMaterial ()
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

	samplesLocation = shaderProgram.GetUniform ( "samples" );
	inverseSamplesLocation = shaderProgram.GetUniform ( "inverseSamples" );

	SetSamplesPerPixel ( DEFAULT_SAMPLES_PER_PIXEL );
}

EMBRDFIntegratorMaterial::~EMBRDFIntegratorMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMBRDFIntegratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	glUseProgram ( shaderProgram.GetProgram () );
	glUniform1i ( samplesLocation, samples );
	glUniform1f ( inverseSamplesLocation, inverseSamples );
}

GXVoid EMBRDFIntegratorMaterial::Unbind ()
{
	glUseProgram ( 0 );
}

GXVoid EMBRDFIntegratorMaterial::SetSamplesPerPixel ( GXUShort samplesPerPixel )
{
	samples = (GXInt)samplesPerPixel;
	inverseSamples = 1.0f / (GXFloat)samplesPerPixel;
}
