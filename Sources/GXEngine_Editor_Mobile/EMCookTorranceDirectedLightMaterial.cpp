#include <GXEngine_Editor_Mobile/EMCookTorranceDirectedLightMaterial.h>
#include <GXEngine/GXCamera.h>


#define DEFAULT_LIGHT_DIRECTION_VIEW_X      0.0f
#define DEFAULT_LIGHT_DIRECTION_VIEW_Y      -1.0f
#define DEFAULT_LIGHT_DIRECTION_VIEW_Z      0.0f

#define DEFAULT_HUE_R                       1.0f
#define DEFAULT_HUE_G                       1.0f
#define DEFAULT_HUE_B                       1.0f
#define DEFAULT_HUE_A                       1.0f

#define DEFAULT_INTENSITY                   0.9f

#define DEFAULT_AMBIENT_COLOR_R             0.1f
#define DEFAULT_AMBIENT_COLOR_B             0.1f
#define DEFAULT_AMBIENT_COLOR_G             0.1f
#define DEFAULT_AMBIENT_COLOR_A             1.0f

#define ALBEDO_SLOT                         0u
#define NORMAL_SLOT                         1u
#define EMISSION_SLOT                       2u
#define PARAMETER_SLOT                      3u
#define DEPTH_SLOT                          4u

#define ANY_ALPHA                           255u

#define VERTEX_SHADER                       L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER                     nullptr
#define FRAGMENT_SHADER                     L"Shaders/Editor Mobile/CookTorranceDirectedLight_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMCookTorranceDirectedLightMaterial::EMCookTorranceDirectedLightMaterial ():
    _albedoTexture ( nullptr ),
    _normalTexture ( nullptr ),
    _emissionTexture ( nullptr ),
    _parameterTexture ( nullptr ),
    _depthTexture ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
    _hue ( DEFAULT_HUE_R, DEFAULT_HUE_G, DEFAULT_HUE_B, DEFAULT_HUE_A ),
    _intensity ( DEFAULT_INTENSITY ),
    _hdrColor ( DEFAULT_HUE_R * DEFAULT_INTENSITY, DEFAULT_HUE_G * DEFAULT_INTENSITY, DEFAULT_HUE_B * DEFAULT_INTENSITY, DEFAULT_HUE_A ),
    _ambientColor ( DEFAULT_AMBIENT_COLOR_R, DEFAULT_AMBIENT_COLOR_B, DEFAULT_AMBIENT_COLOR_G, DEFAULT_AMBIENT_COLOR_A ),
    _toLightDirectionView ( -DEFAULT_LIGHT_DIRECTION_VIEW_X, -DEFAULT_LIGHT_DIRECTION_VIEW_Y, -DEFAULT_LIGHT_DIRECTION_VIEW_Z )
{
    constexpr GLchar* samplerNames[ 5u ] = { "albedoSampler", "normalSampler", "emissionSampler", "parameterSampler", "depthSampler" };
    constexpr GLuint samplerLocations[ 5u ] = { ALBEDO_SLOT, NORMAL_SLOT, EMISSION_SLOT, PARAMETER_SLOT, DEPTH_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 5u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _hueLocation = _shaderProgram.GetUniform ( "hue" );
    _intensityLocation = _shaderProgram.GetUniform ( "intensity" );
    _hdrColorLocation = _shaderProgram.GetUniform ( "hrdColor" );
    _ambientColorLocation = _shaderProgram.GetUniform ( "ambientColor" );
    _toLightDirectionViewLocation = _shaderProgram.GetUniform ( "toLightDirectionView" );
    _inverseProjectionMatrixLocation = _shaderProgram.GetUniform ( "inverseProjectionMatrix" );
}

EMCookTorranceDirectedLightMaterial::~EMCookTorranceDirectedLightMaterial ()
{
    // NOTHING
}

GXVoid EMCookTorranceDirectedLightMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_albedoTexture || !_normalTexture || !_emissionTexture || !_parameterTexture || !_depthTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    const GXMat4& inverseProjectionMatrix = GXCamera::GetActiveCamera ()->GetCurrentFrameInverseProjectionMatrix ();
    glUniformMatrix4fv ( _inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix._data );

    glUniform3fv ( _hueLocation, 1, _hue._data );
    glUniform1f ( _intensityLocation, _intensity );
    glUniform3fv ( _hdrColorLocation, 1, _hdrColor._data );
    glUniform3fv ( _ambientColorLocation, 1, _ambientColor._data );
    glUniform3fv ( _toLightDirectionViewLocation, 1, _toLightDirectionView._data );

    _albedoTexture->Bind ( ALBEDO_SLOT );
    _sampler.Bind ( ALBEDO_SLOT );

    _normalTexture->Bind ( NORMAL_SLOT );
    _sampler.Bind ( NORMAL_SLOT );

    _emissionTexture->Bind ( EMISSION_SLOT );
    _sampler.Bind ( EMISSION_SLOT );

    _parameterTexture->Bind ( PARAMETER_SLOT );
    _sampler.Bind ( PARAMETER_SLOT );

    _depthTexture->Bind ( DEPTH_SLOT );
    _sampler.Bind ( DEPTH_SLOT );
}

GXVoid EMCookTorranceDirectedLightMaterial::Unbind ()
{
    if ( !_albedoTexture || !_normalTexture || !_emissionTexture || !_parameterTexture || !_depthTexture ) return;

    _sampler.Unbind ( ALBEDO_SLOT );
    _albedoTexture->Unbind ();

    _sampler.Unbind ( NORMAL_SLOT );
    _normalTexture->Unbind ();

    _sampler.Unbind ( EMISSION_SLOT );
    _emissionTexture->Unbind ();

    _sampler.Unbind ( PARAMETER_SLOT );
    _parameterTexture->Unbind ();

    _sampler.Unbind ( DEPTH_SLOT );
    _depthTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMCookTorranceDirectedLightMaterial::SetAlbedoTexture ( GXTexture2D &texture )
{
    _albedoTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetNormalTexture ( GXTexture2D &texture )
{
    _normalTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetEmissionTexture ( GXTexture2D &texture )
{
    _emissionTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetParameterTexture ( GXTexture2D &texture )
{
    _parameterTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetDepthTexture ( GXTexture2D &texture )
{
    _depthTexture = &texture;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetLightDirectionView ( const GXVec3 &direction )
{
    _toLightDirectionView = direction;
    _toLightDirectionView.Reverse ();
}

GXVoid EMCookTorranceDirectedLightMaterial::SetHue ( GXUByte red, GXUByte green, GXUByte blue )
{
    _hue.From ( red, green, blue, ANY_ALPHA );
    _hdrColor._data[ 0u ] = _hue._data[ 0u ] * _intensity;
    _hdrColor._data[ 1u ] = _hue._data[ 1u ] * _intensity;
    _hdrColor._data[ 2u ] = _hue._data[ 2u ] * _intensity;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetIntencity ( GXFloat intencity )
{
    this->_intensity = intencity;
    _hdrColor._data[ 0u ] = _hue._data[ 0u ] * _intensity;
    _hdrColor._data[ 1u ] = _hue._data[ 1u ] * _intensity;
    _hdrColor._data[ 2u ] = _hue._data[ 2u ] * _intensity;
}

GXVoid EMCookTorranceDirectedLightMaterial::SetAmbientColor ( const GXColorRGB &color )
{
    _ambientColor = color;
}
