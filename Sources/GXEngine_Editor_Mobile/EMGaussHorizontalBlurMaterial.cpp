#include <GXEngine_Editor_Mobile/EMGaussHorizontalBlurMaterial.h>
#include <GXCommon/GXLogger.h>


#define VERTEX_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL         L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL       nullptr
#define FRAGMENT_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL       L"Shaders/Editor Mobile/GaussHorizontalBlurOneChannelFivePixelKernel_fs.txt"

#define IMAGE_SLOT                                          0u

//---------------------------------------------------------------------------------------------------------------------

EMGaussHorizontalBlurMaterial::EMGaussHorizontalBlurMaterial ( eEMGaussHorizontalBlurKernelType kernelType ):
    _kernelType ( kernelType ),
    _imageTexture ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f )
{
    constexpr GLchar* samplerNames[ 1u ] = { "imageSampler" };
    constexpr GLuint samplerLocations[ 1u ] = { IMAGE_SLOT };

    GXShaderProgramInfo si;
    si._samplers = 1u;
    si._samplerNames = samplerNames;
    si._samplerLocations = samplerLocations;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    switch ( kernelType )
    {
        case eEMGaussHorizontalBlurKernelType::OneChannelFivePixel:
            si._vertexShader = VERTEX_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL;
            si._geometryShader = GEOMETRY_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL;
            si._fragmentShader = FRAGMENT_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL;
        break;

        default:
            // NOTHING
        break;
    }

    _shaderProgram.Init ( si );
}

EMGaussHorizontalBlurMaterial::~EMGaussHorizontalBlurMaterial ()
{
    // NOTHING
}

GXVoid EMGaussHorizontalBlurMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_imageTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );

    _imageTexture->Bind ( IMAGE_SLOT );
    _sampler.Bind ( IMAGE_SLOT );
}

GXVoid EMGaussHorizontalBlurMaterial::Unbind ()
{
    if ( !_imageTexture ) return;

    _sampler.Unbind ( IMAGE_SLOT );
    _imageTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMGaussHorizontalBlurMaterial::SetImageTexture ( GXTexture2D &texture )
{
    switch ( _kernelType )
    {
        case eEMGaussHorizontalBlurKernelType::OneChannelFivePixel:
            if ( texture.GetChannelNumber () != 1u ) break;

            _imageTexture = &texture;
            return;
        break;

        default:
            // NOTHING
        break;
    }

    GXLogA ( "EMGaussHorizontalBlurMaterial::SetImageTexture::Error - �������� ��������� � ���������� ������� ��������\n" );
}
