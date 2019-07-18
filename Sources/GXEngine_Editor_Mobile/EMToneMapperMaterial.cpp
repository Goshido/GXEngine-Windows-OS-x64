#include <GXEngine_Editor_Mobile/EMToneMapperMaterial.h>


#define DEFAULT_GAMMA                       2.2f
#define DEFAULT_ABSOLUTE_WHITE_INTENSITY    1.0e+5f

#define DEFAULT_AVERAGE_LUMINANCE           0.5f
#define DEFAULT_MINIMUM_LUMINANCE           0.5f
#define DEFAULT_MAXIMUM_LUMINANCE           0.5f

#define DEFAULT_EYE_SENSETIVITY             0.0072f

#define EPSILON                             1.0e-4f

#define LIMINANCE_RED_FACTOR                0.27f
#define LIMINANCE_GREEN_FACTOR              0.67f
#define LIMINANCE_BLUE_FACTOR               0.06f

#define TEXTURE_SLOT                        0u

#define VERTEX_SHADER                       L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER                     nullptr
#define FRAGMENT_SHADER                     L"Shaders/Editor Mobile/ToneMapper_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMToneMapperMaterial::EMToneMapperMaterial ():
    _linearSpaceTexture ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
    _gamma ( DEFAULT_GAMMA ),
    _inverseGamma ( 1.0f / DEFAULT_GAMMA ),
    _eyeSensitivity ( DEFAULT_EYE_SENSETIVITY )
{
    constexpr GLchar* samplerNames[ 1u ] = { "linearSpaceSampler" };
    constexpr GLuint samplerLocations[ 1u ] = { TEXTURE_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 1u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _inverseGammaLocation = _shaderProgram.GetUniform ( "inverseGamma" );
    _prescaleFactorLocation = _shaderProgram.GetUniform ( "prescaleFactor" );
    _inverseAbsoluteWhiteSquareIntensityLocation = _shaderProgram.GetUniform ( "inverseAbsoluteWhiteSquareIntensity" );

    SetLuminanceTriplet ( DEFAULT_AVERAGE_LUMINANCE, DEFAULT_MINIMUM_LUMINANCE, DEFAULT_MAXIMUM_LUMINANCE );
    SetAbsoluteWhiteIntensity ( DEFAULT_ABSOLUTE_WHITE_INTENSITY );
}

EMToneMapperMaterial::~EMToneMapperMaterial ()
{
    // NOTHING
}

GXVoid EMToneMapperMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_linearSpaceTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    glUniform1f ( _inverseGammaLocation, _inverseGamma );
    glUniform1f ( _prescaleFactorLocation, _prescaleFactor );
    glUniform1f ( _inverseAbsoluteWhiteSquareIntensityLocation, _inverseAbsoluteWhiteSquareIntensity );

    _linearSpaceTexture->Bind ( TEXTURE_SLOT );
    _sampler.Bind ( TEXTURE_SLOT );
}

GXVoid EMToneMapperMaterial::Unbind ()
{
    if ( !_linearSpaceTexture ) return;

    _sampler.Unbind ( TEXTURE_SLOT );
    _linearSpaceTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMToneMapperMaterial::SetLinearSpaceTexture ( GXTexture2D &texture )
{
    _linearSpaceTexture = &texture;
}

GXVoid EMToneMapperMaterial::SetLuminanceTriplet ( GXFloat averageLuminance, GXFloat minimumLuminance, GXFloat maximumLuminance )
{
    GXFloat alpha = ( maximumLuminance - averageLuminance );
    GXFloat betta = ( averageLuminance - minimumLuminance );
    GXFloat yotta = _eyeSensitivity * powf ( 2.0f, 2.0f * ( betta - alpha ) / ( alpha + betta + EPSILON ) );

    _prescaleFactor = yotta / ( averageLuminance + EPSILON );
}

GXVoid EMToneMapperMaterial::SetGamma ( GXFloat newGamma )
{
    _gamma = newGamma;
    _inverseGamma = 1.0f / _gamma;
}

GXFloat EMToneMapperMaterial::GetGamma () const
{
    return _gamma;
}

GXVoid EMToneMapperMaterial::SetEyeSensitivity ( GXFloat sensitivity )
{
    _eyeSensitivity = sensitivity;
}

GXFloat EMToneMapperMaterial::GetEyeSensitivity () const
{
    return _eyeSensitivity;
}

GXVoid EMToneMapperMaterial::SetAbsoluteWhiteIntensity ( GXFloat intensity )
{
    _absoluteWhiteIntensity = intensity;
    _inverseAbsoluteWhiteSquareIntensity = 1.0f / ( intensity * intensity + EPSILON );
}

GXFloat EMToneMapperMaterial::GetAbsoluteWhiteIntensity () const
{
    return _absoluteWhiteIntensity;
}
