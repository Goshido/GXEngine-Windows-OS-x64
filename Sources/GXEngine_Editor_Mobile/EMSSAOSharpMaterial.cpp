#include <GXEngine_Editor_Mobile/EMSSAOSharpMaterial.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_MAX_CHECK_RADIUS            0.1f
#define DEFAULT_SAMPLES                     64
#define DEFAULT_NOISE_TEXTURE_RESOLUTION    128u
#define DEFAULT_MAX_DISTANCE                1000.0f

#define MINIMUM_KERNEL_SCALE                0.01f
#define NOISE_TEXTURE_BYTES_PER_PIXEL       2u
#define CORRECTED_NOISE_X                   1.0f

#define DEPTH_SLOT                          0u
#define NORMAL_SLOT                         1u
#define NOISE_SLOT                          2u

#define VERTEX_SHADER                       L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER                     nullptr
#define FRAGMENT_SHADER                     L"Shaders/Editor Mobile/SSAOSharp_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

class EMSSAOSharpMaterialResource: public GXMemoryInspector
{
    public:
        EMSSAOSharpMaterialResource ();
        ~EMSSAOSharpMaterialResource () override;

    private:
        EMSSAOSharpMaterialResource ( const EMSSAOSharpMaterialResource &other ) = delete;
        EMSSAOSharpMaterialResource& operator = ( const EMSSAOSharpMaterialResource &other ) = delete;
};

EMSSAOSharpMaterialResource::EMSSAOSharpMaterialResource ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "EMSSAOSharpMaterialResource" )
{
    // NOTHING
}

EMSSAOSharpMaterialResource::~EMSSAOSharpMaterialResource ()
{
    // NOTHING
}

static EMSSAOSharpMaterialResource em_SSAOSharpMaterialResource;

//---------------------------------------------------------------------------------------------------------------------

EMSSAOSharpMaterial::EMSSAOSharpMaterial ():
    _depthTexture ( nullptr ),
    _normalTexture ( nullptr ),
    _sampler ( GL_REPEAT, eGXResampling::None, 1.0f ),
    _maxDistance ( DEFAULT_MAX_DISTANCE ),
    _checkRadius ( DEFAULT_MAX_CHECK_RADIUS )
{
    constexpr GLchar* samplerNames[ 3u ] = { "depthSampler", "normalSampler", "noiseSampler" };
    constexpr GLuint samplerLocations[ 3u ] = { DEPTH_SLOT, NORMAL_SLOT, NOISE_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 3u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _kernelLocation = _shaderProgram.GetUniform ( "kernel" );
    _checkRadiusLocation = _shaderProgram.GetUniform ( "checkRadius" );
    _samplesLocation = _shaderProgram.GetUniform ( "samples" );
    _inverseSamplesLocation = _shaderProgram.GetUniform ( "inverseSamples" );
    _noiseScaleLocation = _shaderProgram.GetUniform ( "noiseScale" );
    _maxDistanceLocation = _shaderProgram.GetUniform ( "maxDistance" );
    _projectionMatrixLocation = _shaderProgram.GetUniform ( "projectionMatrix" );
    _inverseProjectionMatrixLocation = _shaderProgram.GetUniform ( "inverseProjectionMatrix" );

    SetSampleNumber ( DEFAULT_SAMPLES );
    SetNoiseTextureResolution ( DEFAULT_NOISE_TEXTURE_RESOLUTION );
}

EMSSAOSharpMaterial::~EMSSAOSharpMaterial ()
{
    _noiseTexture.FreeResources ();
}

GXVoid EMSSAOSharpMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_depthTexture || !_normalTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    GXCamera* camera = GXCamera::GetActiveCamera ();

    const GXMat4& projectionMatrix = camera->GetCurrentFrameProjectionMatrix ();
    const GXMat4& inverseProjectionMatrix = camera->GetCurrentFrameInverseProjectionMatrix ();

    glUniformMatrix4fv ( _projectionMatrixLocation, 1, GL_FALSE, projectionMatrix._data );
    glUniformMatrix4fv ( _inverseProjectionMatrixLocation, 1, GL_FALSE, inverseProjectionMatrix._data );
    glUniform3fv ( _kernelLocation, EM_MAX_SSAO_SAMPLES, reinterpret_cast<const GLfloat*> ( _kernel ) );
    glUniform1f ( _checkRadiusLocation, _checkRadius );
    glUniform1i ( _samplesLocation, _samples );
    glUniform1f ( _inverseSamplesLocation, _inverseSamples );
    glUniform2fv ( _noiseScaleLocation, 1, _noiseScale._data );
    glUniform1f ( _maxDistanceLocation, _maxDistance );

    _depthTexture->Bind ( DEPTH_SLOT );
    _sampler.Bind ( DEPTH_SLOT );

    _normalTexture->Bind ( NORMAL_SLOT );
    _sampler.Bind ( NORMAL_SLOT );

    _noiseTexture.Bind ( NOISE_SLOT );
    _sampler.Bind ( NOISE_SLOT );
}

GXVoid EMSSAOSharpMaterial::Unbind ()
{
    if ( !_depthTexture || !_normalTexture ) return;

    _sampler.Unbind ( DEPTH_SLOT );
    _depthTexture->Unbind ();

    _sampler.Unbind ( NORMAL_SLOT );
    _normalTexture->Unbind ();

    _sampler.Unbind ( NOISE_SLOT );
    _noiseTexture.Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMSSAOSharpMaterial::SetDepthTexture ( GXTexture2D &texture )
{
    _depthTexture = &texture;
}

GXVoid EMSSAOSharpMaterial::SetNormalTexture ( GXTexture2D &texture )
{
    _normalTexture = &texture;
}

GXVoid EMSSAOSharpMaterial::SetCheckRadius ( GXFloat meters )
{
    if ( meters <= 0.0f ) return;

    GXRandomize ();

    for ( GXInt i = 0; i < _samples; ++i )
    {
        GXFloat scale = i * _inverseSamples;
        scale *= scale;
        scale = MINIMUM_KERNEL_SCALE + scale * ( 1.0f - MINIMUM_KERNEL_SCALE );

        GXVec3 v;
        v.SetX ( GXRandomBetween ( -1.0f, 1.0f ) );
        v.SetY ( GXRandomBetween ( -1.0f, 1.0f ) );
        v.SetZ ( GXRandomNormalize () );

        if ( v.Length () == 0.0f )
            v.SetZ ( 1.0f );
        else
            v.Normalize ();

        _kernel[ i ].Multiply ( v, scale * meters );
    }

    _checkRadius = meters;
}

GXFloat EMSSAOSharpMaterial::GetCheckRadius () const
{
    return _checkRadius;
}

GXVoid EMSSAOSharpMaterial::SetSampleNumber ( GXUByte samplesPerPixel )
{
    if ( samplesPerPixel > EM_MAX_SSAO_SAMPLES )
    {
        GXLogA ( "EMSSAOSharpMaterial::SetSampleNumber::Warning - Ќе могу обработать столько выборок (указано %hu). Ѕудет использовано количество выборок равное %i.\n", _samples, EM_MAX_SSAO_SAMPLES );
        _samples = EM_MAX_SSAO_SAMPLES;
    }
    else
    {
        _samples = static_cast<GXInt> ( samplesPerPixel );
    }

    _inverseSamples = 1.0f / static_cast<const GXFloat> ( _samples );

    SetCheckRadius ( _checkRadius );
}

GXUByte EMSSAOSharpMaterial::GetSampleNumber () const
{
    return static_cast<GXUByte> ( _samples );
}

GXVoid EMSSAOSharpMaterial::SetNoiseTextureResolution ( GXUShort resolution )
{
    if ( resolution == 0u )
    {
        GXLogA ( "EMSSAOSharpMaterial::SetNoiseTextureResolution::Error - ”казано разрешение в 0 пикселей!\n" );
        return;
    }

    _noiseTexture.FreeResources ();
    
    const GXUInt totalPixels = static_cast<const GXUInt> ( resolution * resolution );
    GXUByte* noiseData = static_cast<GXUByte*> ( em_SSAOSharpMaterialResource.Malloc ( totalPixels * NOISE_TEXTURE_BYTES_PER_PIXEL ) );

    GXRandomize ();

    GXUInt offset = 0u;

    for ( GXUInt i = 0u; i < totalPixels; ++i )
    {
        GXVec2 v;
        v.SetX ( GXRandomBetween ( -1.0f, 1.0f ) );
        v.SetY ( GXRandomBetween ( -1.0f, 1.0f ) );

        if ( v.Length () == 0.0f )
            v.SetX ( CORRECTED_NOISE_X );
        else
            v.Normalize ();

        noiseData[ offset ] = static_cast<GXUByte> ( v._data[ 0u ] * 128.0f + 127.0f );
        ++offset;

        noiseData[ offset ] = static_cast<GXUByte> ( v._data[ 1u ] * 128.0f + 127.0f );
        ++offset;
    }

    _noiseTexture.InitResources ( resolution, resolution, GL_RG8, GL_FALSE );
    _noiseTexture.FillWholePixelData ( noiseData );

    em_SSAOSharpMaterialResource.Free ( noiseData );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    const GXFloat inverseResolution = 1.0f / static_cast<GXFloat> ( resolution );

    _noiseScale._data[ 0u ] = renderer.GetWidth () * inverseResolution;
    _noiseScale._data[ 1u ] = renderer.GetHeight () * inverseResolution;
}

GXUShort EMSSAOSharpMaterial::GetNoiseTextureResolution () const
{
    return _noiseTexture.GetWidth ();
}

GXVoid EMSSAOSharpMaterial::SetMaximumDistance ( GXFloat meters )
{
    _maxDistance = meters;
}

GXFloat EMSSAOSharpMaterial::GetMaximumDistance () const
{
    return _maxDistance;
}
