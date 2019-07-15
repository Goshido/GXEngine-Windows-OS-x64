#include <GXEngine_Editor_Mobile/EMImportantAreaFilterMaterial.h>
#include <GXCommon/GXMemory.h>


#define IMAGE_SLOT                  0u
#define RETINA_FILTER_SLOT          1u

#define MINIMUM_U                   0.0
#define MAXIMUM_U                   1.0
#define MINIMUM_V                   0.0
#define MAXIMUM_V                   1.0
#define THREE_SIGMA_RULE_FACTOR     0.16666666666666666666666666666667
#define DOUBLE_PI                   6.283185307179586476925286766559

#define VERTEX_SHADER               L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER             nullptr
#define FRAGMENT_SHADER             L"Shaders/Editor Mobile/ImportantAreaFilter_fs.txt"

//---------------------------------------------------------------------------------------------

class EMImportAreaFilterMaterialResource : public GXMemoryInspector
{
    public:
        EMImportAreaFilterMaterialResource ();
        ~EMImportAreaFilterMaterialResource () override;

    private:
        EMImportAreaFilterMaterialResource ( const EMImportAreaFilterMaterialResource &other ) = delete;
        EMImportAreaFilterMaterialResource& operator = ( const EMImportAreaFilterMaterialResource &other ) = delete;
};

EMImportAreaFilterMaterialResource::EMImportAreaFilterMaterialResource ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "EMImportAreaFilterMaterialResource" )
{
    // NOTHING
}

EMImportAreaFilterMaterialResource::~EMImportAreaFilterMaterialResource ()
{
    // NOTHING
}

static EMImportAreaFilterMaterialResource em_ImportAreaFilterMaterialResource;

//---------------------------------------------------------------------------------------------

EMImportantAreaFilterMaterial::EMImportantAreaFilterMaterial ():
    _imageTexture ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f )
{
    constexpr GLchar* samplerNames[ 3u ] = { "imageSampler", "retinaFilterSampler" };
    constexpr GLuint samplerLocations[ 3u ] = { IMAGE_SLOT, RETINA_FILTER_SLOT };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 2u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );
}

EMImportantAreaFilterMaterial::~EMImportantAreaFilterMaterial ()
{
    _retinaFilterTexture.FreeResources ();
}

GXVoid EMImportantAreaFilterMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_imageTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    _imageTexture->Bind ( IMAGE_SLOT );
    _sampler.Bind ( IMAGE_SLOT );

    _retinaFilterTexture.Bind ( RETINA_FILTER_SLOT );
    _sampler.Bind ( RETINA_FILTER_SLOT );
}

GXVoid EMImportantAreaFilterMaterial::Unbind ()
{
    if ( !_imageTexture ) return;

    glUseProgram ( 0u );

    _sampler.Unbind ( IMAGE_SLOT );
    _imageTexture->Unbind ();

    _sampler.Unbind ( RETINA_FILTER_SLOT );
    _retinaFilterTexture.Unbind ();
}

GXVoid EMImportantAreaFilterMaterial::SetImageTexture ( GXTexture2D &texture )
{
    _imageTexture = &texture;

    GXUShort width = texture.GetHeight ();
    GXUShort height = texture.GetWidth ();

    GXUShort effectiveLength = height < width ? height : width;

    if ( _retinaFilterTexture.IsInitialized () && effectiveLength == _retinaFilterTexture.GetHeight () ) return;

    GenerateRetinaFilterTexture ( effectiveLength );
}

GXVoid EMImportantAreaFilterMaterial::GenerateRetinaFilterTexture ( GXUShort effectiveLength )
{
    const GXDouble step = ( MAXIMUM_V - MINIMUM_V ) / static_cast<GXDouble> ( effectiveLength - 1u );

    const GXDouble differenceU = MAXIMUM_U - MINIMUM_U;
    const GXDouble differenceV = MAXIMUM_V - MINIMUM_V;
    const GXDouble meanU = MINIMUM_U + differenceU * 0.5;
    const GXDouble meanV = MINIMUM_V + differenceV * 0.5;
    const GXDouble standartDeviationU = differenceU * THREE_SIGMA_RULE_FACTOR;
    const GXDouble standartDeviationV = differenceV * THREE_SIGMA_RULE_FACTOR;

    GXDouble alpha = step * 0.5;
    const GXDouble limitU = MAXIMUM_U + alpha;
    const GXDouble limitV = MAXIMUM_V + alpha;

    alpha = 1.0 / ( standartDeviationU * standartDeviationU );
    const GXDouble omega = 1.0 / ( standartDeviationV * standartDeviationV );
    const GXDouble yotta = 1.0 / ( DOUBLE_PI * standartDeviationU * standartDeviationV );

    GXDouble totalGaussSum = 0.0;
    GXDouble v = MINIMUM_V;

    const GXUInt samples = static_cast<GXUInt> ( effectiveLength * effectiveLength );
    GXUPointer sampleOffset = 0u;
    GXDouble* probabilityDensitySamples = static_cast<GXDouble*> ( em_ImportAreaFilterMaterialResource.Malloc ( samples * sizeof ( GXDouble ) ) );

    while ( v < limitV )
    {
        const GXDouble gamma = v - meanV;
        const GXDouble phi = omega * gamma * gamma;

        GXDouble u = MINIMUM_U;

        while ( u < limitU )
        {
            const GXDouble zetta = u - meanU;
            
            const GXDouble sample = yotta * exp ( -0.5 * ( alpha * zetta * zetta + phi ) );
            totalGaussSum += sample;
            probabilityDensitySamples[ sampleOffset ] = sample;

            ++sampleOffset;
            u += step;
        }

        v += step;
    }

    const GXDouble hardwareReducingCompensationFactor = static_cast<GXDouble> ( samples ) / totalGaussSum;
    GXFloat* retinaFilterSamples = static_cast<GXFloat*> ( em_ImportAreaFilterMaterialResource.Malloc ( samples * sizeof ( GXFloat ) ) );

    for ( sampleOffset = 0u; sampleOffset < samples; ++sampleOffset )
        retinaFilterSamples[ sampleOffset ] = static_cast<GXFloat> ( hardwareReducingCompensationFactor * probabilityDensitySamples[ sampleOffset ] );

    em_ImportAreaFilterMaterialResource.Free ( probabilityDensitySamples );

    if ( _retinaFilterTexture.IsInitialized () )
        _retinaFilterTexture.FreeResources ();

    _retinaFilterTexture.InitResources ( effectiveLength, effectiveLength, GL_R32F, GX_FALSE );
    _retinaFilterTexture.FillWholePixelData ( retinaFilterSamples );

    em_ImportAreaFilterMaterialResource.Free ( retinaFilterSamples );
}
