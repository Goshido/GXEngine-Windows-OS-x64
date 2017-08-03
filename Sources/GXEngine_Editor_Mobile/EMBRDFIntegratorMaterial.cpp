#include <GXEngine_Editor_Mobile/EMBRDFIntegratorMaterial.h>


#define VERTEX_SHADER			L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/Editor Mobile/BRDFIntegrator_fs.txt"

#define DEFAULT_TOTAL_SAMPLES	1024


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

	totalSamplesLocation = shaderProgram.GetUniform ( "totalSamples" );
	inverseTotalSamplesLocation = shaderProgram.GetUniform ( "inverseTotalSamples" );

	SetTotalSamples ( DEFAULT_TOTAL_SAMPLES );
}

EMBRDFIntegratorMaterial::~EMBRDFIntegratorMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMBRDFIntegratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	glUseProgram ( shaderProgram.GetProgram () );
	glUniform1i ( totalSamplesLocation, totalSamples );
	glUniform1f ( inverseTotalSamplesLocation, inverseTotalSamples );
}

GXVoid EMBRDFIntegratorMaterial::Unbind ()
{
	glUseProgram ( 0 );
}

GXVoid EMBRDFIntegratorMaterial::SetTotalSamples ( GXUShort samples )
{
	totalSamples = (GXInt)samples;
	inverseTotalSamples = 1.0f / (GXFloat)samples;
}
