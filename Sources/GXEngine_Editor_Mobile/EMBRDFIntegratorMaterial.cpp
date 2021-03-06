#include <GXEngine_Editor_Mobile/EMBRDFIntegratorMaterial.h>


#define VERTEX_SHADER                   L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER                 nullptr
#define FRAGMENT_SHADER                 L"Shaders/Editor Mobile/BRDFIntegrator_fs.txt"

#define DEFAULT_SAMPLES_PER_PIXEL       1024

//---------------------------------------------------------------------------------------------------------------------

EMBRDFIntegratorMaterial::EMBRDFIntegratorMaterial ():
    _samples ( DEFAULT_SAMPLES_PER_PIXEL ),
    _inverseSamples ( 1.0f / static_cast<GXFloat> ( DEFAULT_SAMPLES_PER_PIXEL ) )
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

    _samplesLocation = _shaderProgram.GetUniform ( "samples" );
    _inverseSamplesLocation = _shaderProgram.GetUniform ( "inverseSamples" );
}

EMBRDFIntegratorMaterial::~EMBRDFIntegratorMaterial ()
{
    // NOTHING
}

GXVoid EMBRDFIntegratorMaterial::Bind ( const GXTransform& /*transform*/ )
{
    glUseProgram ( _shaderProgram.GetProgram () );
    glUniform1i ( _samplesLocation, _samples );
    glUniform1f ( _inverseSamplesLocation, _inverseSamples );
}

GXVoid EMBRDFIntegratorMaterial::Unbind ()
{
    glUseProgram ( 0u );
}

GXVoid EMBRDFIntegratorMaterial::SetSamplesPerPixel ( GXUShort samplesPerPixel )
{
    _samples = static_cast<GXInt> ( samplesPerPixel );
    _inverseSamples = 1.0f / static_cast<GXFloat> ( samplesPerPixel );
}
