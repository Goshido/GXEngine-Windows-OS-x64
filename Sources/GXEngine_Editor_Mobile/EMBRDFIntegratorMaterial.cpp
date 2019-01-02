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
    si.vertexShader = VERTEX_SHADER;
    si.geometryShader = GEOMETRY_SHADER;
    si.fragmentShader = FRAGMENT_SHADER;
    si.samplers = 0u;
    si.samplerNames = nullptr;
    si.samplerLocations = nullptr;
    si.transformFeedbackOutputs = 0;
    si.transformFeedbackOutputNames = nullptr;

    shaderProgram.Init ( si );

    samplesLocation = shaderProgram.GetUniform ( "samples" );
    inverseSamplesLocation = shaderProgram.GetUniform ( "inverseSamples" );
}

EMBRDFIntegratorMaterial::~EMBRDFIntegratorMaterial ()
{
    // NOTHING
}

GXVoid EMBRDFIntegratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
    glUseProgram ( shaderProgram.GetProgram () );
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
