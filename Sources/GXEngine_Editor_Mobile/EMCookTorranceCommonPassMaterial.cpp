#include <GXEngine_Editor_Mobile/EMCookTorranceCommonPassMaterial.h>
#include <GXEngine/GXCamera.h>


#define ALBEDO_SLOT                             0u

#define DEFAULT_ALBEDO_TEXTURE_SCALE_U          1.0f
#define DEFAULT_ALBEDO_TEXTURE_SCALE_V          1.0f

#define DEFAULT_ALBEDO_TEXTURE_OFFSET_U         0.0f
#define DEFAULT_ALBEDO_TEXTURE_OFFSET_V         0.0f

#define DEFAULT_ALBEDO_COLOR_R                  255u
#define DEFAULT_ALBEDO_COLOR_G                  255u
#define DEFAULT_ALBEDO_COLOR_B                  255u
#define DEFAULT_ALBEDO_COLOR_A                  255u

#define NORMAL_SLOT                             1u

#define DEFAULT_NORMAL_TEXTURE_SCALE_U          1.0f
#define DEFAULT_NORMAL_TEXTURE_SCALE_V          1.0f

#define DEFAULT_NORMAL_TEXTURE_OFFSET_U         0.0f
#define DEFAULT_NORMAL_TEXTURE_OFFSET_V         0.0f

#define EMISSION_SLOT                           2u

#define DEFAULT_EMISSION_TEXTURE_SCALE_U        1.0f
#define DEFAULT_EMISSION_TEXTURE_SCALE_V        1.0f

#define DEFAULT_EMISSION_TEXTURE_OFFSET_U       0.0f
#define DEFAULT_EMISSION_TEXTURE_OFFSET_V       0.0f

#define DEFAULT_EMISSION_COLOR_R                255u
#define DEFAULT_EMISSION_COLOR_G                255u
#define DEFAULT_EMISSION_COLOR_B                255u

#define DEFAULT_EMISSION_SCALE                  1.0f

#define PARAMETER_SLOT                          3u

#define DEFAULT_PARAMETER_TEXTURE_SCALE_U       1.0f
#define DEFAULT_PARAMETER_TEXTURE_SCALE_V       1.0f

#define DEFAULT_PARAMETER_TEXTURE_OFFSET_U      0.0f
#define DEFAULT_PARAMETER_TEXTURE_OFFSET_V      0.0f

#define DEFAULT_ROUGHNESS_SCALE                 1.0f
#define DEFAULT_INDEX_OF_REFRACTION_SCALE       1.0f
#define DEFAULT_SPECULAR_INTECITY_SCALE         1.0f
#define DEFAULT_METALLIC_SCALE                  1.0f

#define DEFAULT_DELTA_TIME                      0.1667f
#define DEFAULT_EXPLOSURE                       0.01667f

#define DEFAULT_SCREEN_RESOLUTION_WIDTH         1280.0f
#define DEFAULT_SCREEN_RESOLUTION_HEIGHT        720.0f

#define DEFAULT_MAXIMUM_BLUR_SAMPLES            15u

#define ANY_ALPHA                               255u
#define UBYTE_COLOR_TO_FLOAT                    0.0039215f

#define VERTEX_SHADER                           L"Shaders/Editor Mobile/CookTorranceCommonPass_vs.txt"
#define GEOMETRY_SHADER                         nullptr
#define FRAGMENT_SHADER                         L"Shaders/Editor Mobile/CookTorranceCommonPass_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMCookTorranceCommonPassMaterial::EMCookTorranceCommonPassMaterial ():
    _albedoTexture ( nullptr ),
    _normalTexture ( nullptr ),
    _emissionTexture ( nullptr ),
    _parameterTexture ( nullptr ),
    _sampler ( GL_REPEAT, eGXResampling::Trilinear, 16.0f ),
    _albedoColor ( static_cast<GXUByte> ( DEFAULT_ALBEDO_COLOR_R ), static_cast<GXUByte> ( DEFAULT_ALBEDO_COLOR_G ), static_cast<GXUByte> ( DEFAULT_ALBEDO_COLOR_B ), static_cast<GXUByte> ( DEFAULT_ALBEDO_COLOR_A ) ),
    _albedoTextureScaleOffset ( DEFAULT_ALBEDO_TEXTURE_SCALE_U, DEFAULT_ALBEDO_TEXTURE_SCALE_V, DEFAULT_ALBEDO_TEXTURE_OFFSET_U, DEFAULT_ALBEDO_TEXTURE_OFFSET_V ),
    _normalTextureScaleOffset ( DEFAULT_NORMAL_TEXTURE_SCALE_U, DEFAULT_NORMAL_TEXTURE_SCALE_V, DEFAULT_NORMAL_TEXTURE_OFFSET_U, DEFAULT_NORMAL_TEXTURE_OFFSET_V ),
    _emissionColor ( static_cast<GXUByte> ( DEFAULT_EMISSION_COLOR_R ), static_cast<GXUByte> ( DEFAULT_EMISSION_COLOR_G ), static_cast<GXUByte> ( DEFAULT_EMISSION_COLOR_B ), static_cast<GXUByte> ( ANY_ALPHA ) ),
    _emissionColorScale ( DEFAULT_EMISSION_SCALE ),
    _emissionScaledColor ( DEFAULT_EMISSION_SCALE * DEFAULT_EMISSION_COLOR_R * UBYTE_COLOR_TO_FLOAT, DEFAULT_EMISSION_SCALE * DEFAULT_EMISSION_COLOR_G * UBYTE_COLOR_TO_FLOAT, DEFAULT_EMISSION_SCALE * DEFAULT_EMISSION_COLOR_B * UBYTE_COLOR_TO_FLOAT, ANY_ALPHA ),
    _emissionTextureScaleOffset ( DEFAULT_EMISSION_TEXTURE_SCALE_U, DEFAULT_EMISSION_TEXTURE_SCALE_V, DEFAULT_EMISSION_TEXTURE_OFFSET_U, DEFAULT_EMISSION_TEXTURE_OFFSET_V ),
    _parameterScale ( DEFAULT_ROUGHNESS_SCALE, DEFAULT_INDEX_OF_REFRACTION_SCALE, DEFAULT_SPECULAR_INTECITY_SCALE, DEFAULT_METALLIC_SCALE ),
    _parameterTextureScaleOffset ( DEFAULT_PARAMETER_TEXTURE_SCALE_U, DEFAULT_PARAMETER_TEXTURE_SCALE_V, DEFAULT_PARAMETER_TEXTURE_OFFSET_U, DEFAULT_PARAMETER_TEXTURE_OFFSET_V ),
    _inverseDeltaTime ( 1.0f / DEFAULT_DELTA_TIME ),
    _exposure ( DEFAULT_EXPLOSURE ),
    _screenResolution ( DEFAULT_SCREEN_RESOLUTION_WIDTH, DEFAULT_SCREEN_RESOLUTION_HEIGHT ),
    _inverseMaximumBlurSamples ( 1.0f / static_cast<GXFloat> ( DEFAULT_MAXIMUM_BLUR_SAMPLES ) )
{
    constexpr GLchar* samplerNames[ 4u ] = { "albedoSampler", "normalSampler", "emissionSampler", "parameterSampler" };
    constexpr GLuint samplerLocations[ 4u ] = { ALBEDO_SLOT, NORMAL_SLOT, EMISSION_SLOT, PARAMETER_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 4u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _albedoTextureScaleOffsetLocation = _shaderProgram.GetUniform ( "albedoTextureScaleOffset" );
    _albedoColorLocation = _shaderProgram.GetUniform ( "albedoColor" );
    _normalTextureScaleOffsetLocation = _shaderProgram.GetUniform ( "normalTextureScaleOffset" );
    _emissionTextureScaleOffsetLocation = _shaderProgram.GetUniform ( "emissionTextureScaleOffset" );
    _emissionScaledColorLocation = _shaderProgram.GetUniform ( "emissionScaledColor" );
    _parameterTextureScaleOffsetLocation = _shaderProgram.GetUniform ( "parameterTextureScaleOffset" );
    _parameterScaleLocation = _shaderProgram.GetUniform ( "parameterScale" );
    _inverseDeltaTimeLocation = _shaderProgram.GetUniform ( "inverseDeltaTime" );
    _exposureLocation = _shaderProgram.GetUniform ( "exposure" );
    _screenResolutionLocation = _shaderProgram.GetUniform ( "screenResolution" );
    _maximumBlurSamplesLocation = _shaderProgram.GetUniform ( "maximumBlurSamples" );
    _inverseMaximumBlurSamplesLocation = _shaderProgram.GetUniform ( "inverseMaximumBlurSamples" );
    _currentFrameModelViewProjectionMatrixLocation = _shaderProgram.GetUniform ( "currentFrameModelViewProjectionMatrix" );
    _currentFrameRotationViewMatrixLocation = _shaderProgram.GetUniform ( "currentFrameRotationViewMatrix" );
    _lastFrameModelViewProjectionMatrixLocation = _shaderProgram.GetUniform ( "lastFrameModelViewProjectionMatrix" );
}

EMCookTorranceCommonPassMaterial::~EMCookTorranceCommonPassMaterial ()
{
    // NOTHING
}

GXVoid EMCookTorranceCommonPassMaterial::Bind ( const GXTransform &transform )
{
    if ( !_albedoTexture || !_normalTexture || !_emissionTexture || !_parameterTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    GXCamera* camera = GXCamera::GetActiveCamera ();

    GXMat4 currentFrameModelViewProjectionMatrix;
    GXMat3 currentFrameRotationViewMatrix;
    GXMat4 lastFrameModelViewProjectionMatrix;

    const GXMat4& currentFrameModelMatrix = transform.GetCurrentFrameModelMatrix ();
    currentFrameModelViewProjectionMatrix.Multiply ( currentFrameModelMatrix, camera->GetCurrentFrameViewProjectionMatrix () );

    GXMat4 currentFrameRotationMatrix;
    transform.GetRotation ( currentFrameRotationMatrix );
    const GXMat4& currentFrameViewMatrix = camera->GetCurrentFrameViewMatrix ();

    GXVec3 tmp;
    GXMat3 r;
    currentFrameRotationMatrix.GetX ( tmp );
    r.SetX ( tmp );

    currentFrameRotationMatrix.GetY ( tmp );
    r.SetY ( tmp );

    currentFrameRotationMatrix.GetZ ( tmp );
    r.SetZ ( tmp );

    GXMat3 v;
    currentFrameViewMatrix.GetX ( tmp );
    v.SetX ( tmp );

    currentFrameViewMatrix.GetY ( tmp );
    v.SetY ( tmp );

    currentFrameViewMatrix.GetZ ( tmp );
    v.SetZ ( tmp );

    currentFrameRotationViewMatrix.Multiply ( r, v );
    lastFrameModelViewProjectionMatrix.Multiply ( transform.GetLastFrameModelMatrix (), camera->GetLastFrameViewProjectionMatrix () );

    glUniformMatrix4fv ( _currentFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, currentFrameModelViewProjectionMatrix._data );
    glUniformMatrix3fv ( _currentFrameRotationViewMatrixLocation, 1, GL_FALSE, currentFrameRotationViewMatrix._data );
    glUniformMatrix4fv ( _lastFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, lastFrameModelViewProjectionMatrix._data );

    glUniform1f ( _inverseDeltaTimeLocation, _inverseDeltaTime );
    glUniform1f ( _exposureLocation, _exposure );
    glUniform2fv ( _screenResolutionLocation, 1, _screenResolution._data );
    glUniform1f ( _maximumBlurSamplesLocation, _maximumBlurSamples );
    glUniform1f ( _inverseMaximumBlurSamplesLocation, _inverseMaximumBlurSamples );

    _albedoTexture->Bind ( ALBEDO_SLOT );
    _sampler.Bind ( ALBEDO_SLOT );
    glUniform4fv ( _albedoTextureScaleOffsetLocation, 1, _albedoTextureScaleOffset._data );
    glUniform4fv ( _albedoColorLocation, 1, _albedoColor._data );

    _normalTexture->Bind ( NORMAL_SLOT );
    _sampler.Bind ( NORMAL_SLOT );
    glUniform4fv ( _normalTextureScaleOffsetLocation, 1, _normalTextureScaleOffset._data );

    _emissionTexture->Bind ( EMISSION_SLOT );
    _sampler.Bind ( EMISSION_SLOT );
    glUniform4fv ( _emissionTextureScaleOffsetLocation, 1, _emissionTextureScaleOffset._data );
    glUniform3fv ( _emissionScaledColorLocation, 1, _emissionScaledColor._data );

    _parameterTexture->Bind ( PARAMETER_SLOT );
    _sampler.Bind ( PARAMETER_SLOT );
    glUniform4fv ( _parameterTextureScaleOffsetLocation, 1, _parameterTextureScaleOffset._data );
    glUniform4fv ( _parameterScaleLocation, 1, _parameterScale._data );
}

GXVoid EMCookTorranceCommonPassMaterial::Unbind ()
{
    if ( !_albedoTexture || !_normalTexture || !_emissionTexture || !_parameterTexture ) return;

    glUseProgram ( 0u );

    _sampler.Unbind ( ALBEDO_SLOT );
    _albedoTexture->Unbind ();

    _sampler.Unbind ( NORMAL_SLOT );
    _normalTexture->Unbind ();

    _sampler.Unbind ( EMISSION_SLOT );
    _emissionTexture->Unbind ();

    _sampler.Unbind ( PARAMETER_SLOT );
    _parameterTexture->Unbind ();
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoTexture ( GXTexture2D &texture )
{
    _albedoTexture = &texture;
}

GXTexture2D* EMCookTorranceCommonPassMaterial::GetAlbedoTexture ()
{
    return _albedoTexture;
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoTextureScale ( GXFloat scaleU, GXFloat scaleV )
{
    _albedoTextureScaleOffset._data[ 0u ] = scaleU;
    _albedoTextureScaleOffset._data[ 1u ] = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetAlbedoTextureScale ( GXVec2 &scaleUV ) const
{
    scaleUV.Init ( _albedoTextureScaleOffset._data[ 0 ], _albedoTextureScaleOffset._data[ 1 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
    _albedoTextureScaleOffset._data[ 2u ] = offsetU;
    _albedoTextureScaleOffset._data[ 3u ] = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetAlbedoTextureOffset ( GXVec2 &offsetUV ) const
{
    offsetUV.Init ( _albedoTextureScaleOffset._data[ 2 ], _albedoTextureScaleOffset._data[ 3 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetAlbedoColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    _albedoColor.From ( red, green, blue, alpha );
}

GXVoid EMCookTorranceCommonPassMaterial::GetAlbedoColor ( GXUByte &red, GXUByte &green, GXUByte &blue, GXUByte &alpha ) const
{
    _albedoColor.ConvertToUByte ( red, green, blue, alpha );
}

GXVoid EMCookTorranceCommonPassMaterial::SetNormalTexture ( GXTexture2D &texture )
{
    _normalTexture = &texture;
}

GXTexture2D* EMCookTorranceCommonPassMaterial::GetNormalTexture ()
{
    return _normalTexture;
}

GXVoid EMCookTorranceCommonPassMaterial::SetNormalTextureScale ( GXFloat scaleU, GXFloat scaleV )
{
    _normalTextureScaleOffset._data[ 0u ] = scaleU;
    _normalTextureScaleOffset._data[ 1u ] = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetNormalTextureScale ( GXVec2 &scaleUV ) const
{
    scaleUV.Init ( _normalTextureScaleOffset._data[ 0 ], _normalTextureScaleOffset._data[ 1 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetNormalTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
    _normalTextureScaleOffset._data[ 2u ] = offsetU;
    _normalTextureScaleOffset._data[ 3u ] = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetNormalTextureOffset ( GXVec2 &offsetUV ) const
{
    offsetUV.Init ( _normalTextureScaleOffset._data[ 2u ], _normalTextureScaleOffset._data[ 3u ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionTexture ( GXTexture2D &texture )
{
    _emissionTexture = &texture;
}

GXTexture2D* EMCookTorranceCommonPassMaterial::GetEmissionTexture ()
{
    return _emissionTexture;
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionTextureScale ( GXFloat scaleU, GXFloat scaleV )
{
    _emissionTextureScaleOffset._data[ 0u ] = scaleU;
    _emissionTextureScaleOffset._data[ 1u ] = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetEmissionTextureScale ( GXVec2 &scaleUV ) const
{
    scaleUV.Init ( _emissionTextureScaleOffset._data[ 0 ], _emissionTextureScaleOffset._data[ 1 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
    _emissionTextureScaleOffset._data[ 2u ] = offsetU;
    _emissionTextureScaleOffset._data[ 3u ] = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetEmissionTextureOffset ( GXVec2 &offsetUV ) const
{
    offsetUV.Init ( _emissionTextureScaleOffset._data[ 2 ], _emissionTextureScaleOffset._data[ 3 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionColor ( GXUByte red, GXUByte green, GXUByte blue )
{
    _emissionColor.From ( red, green, blue, ANY_ALPHA );
    _emissionScaledColor.SetRed ( _emissionColor.GetRed () * _emissionColorScale );
    _emissionScaledColor.SetGreen ( _emissionColor.GetGreen () * _emissionColorScale );
    _emissionScaledColor.SetBlue ( _emissionColor.GetBlue () * _emissionColorScale );
}

GXVoid EMCookTorranceCommonPassMaterial::GetEmissionColor ( GXUByte &red, GXUByte &green, GXUByte &blue ) const
{
    GXUByte anyAlpha;
    _emissionColor.ConvertToUByte ( red, green, blue, anyAlpha );
}

GXVoid EMCookTorranceCommonPassMaterial::SetEmissionColorScale ( GXFloat scale )
{
    _emissionColorScale = scale;
    _emissionScaledColor.SetRed ( _emissionColor.GetRed () * _emissionColorScale );
    _emissionScaledColor.SetGreen ( _emissionColor.GetGreen () * _emissionColorScale );
    _emissionScaledColor.SetBlue ( _emissionColor.GetBlue () * _emissionColorScale );
}

GXFloat EMCookTorranceCommonPassMaterial::GetEmissionColorScale () const
{
    return _emissionColorScale;
}

GXVoid EMCookTorranceCommonPassMaterial::SetParameterTexture ( GXTexture2D &texture )
{
    _parameterTexture = &texture;
}

GXTexture2D* EMCookTorranceCommonPassMaterial::GetParameterTexture ()
{
    return _parameterTexture;
}

GXVoid EMCookTorranceCommonPassMaterial::SetParameterTextureScale ( GXFloat scaleU, GXFloat scaleV )
{
    _parameterTextureScaleOffset._data[ 0u ] = scaleU;
    _parameterTextureScaleOffset._data[ 1u ] = scaleV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetParameterTextureScale ( GXVec2 &scaleUV ) const
{
    scaleUV.Init ( _parameterTextureScaleOffset._data[ 0 ], _parameterTextureScaleOffset._data[ 1 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetParameterTextureOffset ( GXFloat offsetU, GXFloat offsetV )
{
    _parameterTextureScaleOffset._data[ 2u ] = offsetU;
    _parameterTextureScaleOffset._data[ 3u ] = offsetV;
}

GXVoid EMCookTorranceCommonPassMaterial::GetParameterTextureOffset ( GXVec2 &offsetUV ) const
{
    offsetUV.Init ( _parameterTextureScaleOffset._data[ 2 ], _parameterTextureScaleOffset._data[ 3 ] );
}

GXVoid EMCookTorranceCommonPassMaterial::SetRoughnessScale ( GXFloat scale )
{
    _parameterScale._data[ 0u ] = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetRoughnessScale () const
{
    return _parameterScale._data[ 0u ];
}

GXVoid EMCookTorranceCommonPassMaterial::SetIndexOfRefractionScale ( GXFloat scale )
{
    _parameterScale._data[ 1u ] = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetIndexOfRefractionScale () const
{
    return _parameterScale._data[ 1u ];
}

GXVoid EMCookTorranceCommonPassMaterial::SetSpecularIntensityScale ( GXFloat scale )
{
    _parameterScale._data[ 2u ] = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetSpecularIntensityScale () const
{
    return _parameterScale._data[ 2u ];
}

GXVoid EMCookTorranceCommonPassMaterial::SetMetallicScale ( GXFloat scale )
{
    _parameterScale._data[ 3u ] = scale;
}

GXFloat EMCookTorranceCommonPassMaterial::GetMetallicScale () const
{
    return _parameterScale._data[ 3u ];
}

GXVoid EMCookTorranceCommonPassMaterial::SetDeltaTime ( GXFloat seconds )
{
    _inverseDeltaTime = 1.0f / seconds;
}

GXVoid EMCookTorranceCommonPassMaterial::SetExposure ( GXFloat seconds )
{
    _exposure = seconds;
}

GXVoid EMCookTorranceCommonPassMaterial::SetScreenResolution ( GXUShort width, GXUShort height )
{
    _screenResolution.Init ( static_cast<GXFloat> ( width ), static_cast<GXFloat> ( height ) );
}

GXVoid EMCookTorranceCommonPassMaterial::SetMaximumBlurSamples ( GXUByte samples )
{
    _maximumBlurSamples = static_cast<GXFloat> ( samples );
    _inverseMaximumBlurSamples = 1.0f / _maximumBlurSamples;
}
