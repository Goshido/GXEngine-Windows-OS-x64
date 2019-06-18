#include <GXEngine_Editor_Mobile/EMBRDFIntegratorMaterial.h>


#define VERTEX_SHADER                   L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER                 nullptr
#define FRAGMENT_SHADER                 L"Shaders/Editor Mobile/BRDFIntegrator_fs.txt"

#define DEFAULT_SAMPLES_PER_PIXEL       1024

//---------------------------------------------------------------------------------------------------------------------

EMBRDFIntegratorMaterial::EMBRDFIntegratorMaterial ():
    samples ( DEFAULT_SAMPLES_PER_PIXEL ),
    inverseSamples ( 1.0f / static_cast<GXFloat> ( DEFAULT_SAMPLES_PER_PIXEL ) )
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

    samplesLocation = _shaderProgram.GetUniform ( "samples" );
    inverseSamplesLocation = _shaderProgram.GetUniform ( "inverseSamples" );
}

EMBRDFIntegratorMaterial::~EMBRDFIntegratorMaterial ()
{
    // NOTHING
}

GXVoid EMBRDFIntegratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
    glUseProgram ( _shaderProgram.GetProgram () );
    glUniform1i ( samplesLocation, samples );
    glUniform1f ( inverseSamplesLocation, inverseSamples );
}

GXVoid EMBRDFIntegratorMaterial::Unbind ()
{
    glUseProgram ( 0u );
}

GXVoid EMBRDFIntegratorMaterial::SetSamplesPerPixel ( GXUShort samplesPerPixel )
{
    samples = static_cast<GXInt> ( samplesPerPixel );
    inverseSamples = 1.0f / static_cast<GXFloat> ( samplesPerPixel );
}
