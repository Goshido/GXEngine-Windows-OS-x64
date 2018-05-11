#include <GXEngine_Editor_Mobile/EMGaussVerticalBlurMaterial.h>
#include <GXCommon/GXLogger.h>


#define VERTEX_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL		L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL	nullptr
#define FRAGMENT_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL	L"Shaders/Editor Mobile/GaussVerticalBlurOneChannelFivePixelKernel_fs.txt"

#define IMAGE_SLOT										0u

//---------------------------------------------------------------------------------------------------------------------

EMGaussVerticalBlurMaterial::EMGaussVerticalBlurMaterial ( eEMGaussVerticalBlurKernelType kernelType ):
	kernelType ( kernelType ),
	imageTexture ( nullptr ),
	sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f )
{
	static const GLchar* samplerNames[ 1 ] = { "imageSampler" };
	static const GLuint samplerLocations[ 1 ] = { IMAGE_SLOT };

	GXShaderProgramInfo si;
	si.samplers = 1u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;


	switch ( kernelType )
	{
		case eEMGaussVerticalBlurKernelType::OneChannelFivePixel:
			si.vertexShader = VERTEX_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL;
			si.geometryShader = GEOMETRY_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL;
			si.fragmentShader = FRAGMENT_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL;
		break;

		default:
			// NOTHING
		break;
	}

	shaderProgram.Init ( si );
}

EMGaussVerticalBlurMaterial::~EMGaussVerticalBlurMaterial ()
{
	// NOTHING
}

GXVoid EMGaussVerticalBlurMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !imageTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	imageTexture->Bind ( IMAGE_SLOT );
	sampler.Bind ( IMAGE_SLOT );
}

GXVoid EMGaussVerticalBlurMaterial::Unbind ()
{
	if ( !imageTexture ) return;

	sampler.Unbind ( IMAGE_SLOT );
	imageTexture->Unbind ();

	glUseProgram ( 0u );
}

GXVoid EMGaussVerticalBlurMaterial::SetImageTexture ( GXTexture2D &texture )
{
	switch ( kernelType )
	{
		case eEMGaussVerticalBlurKernelType::OneChannelFivePixel:
			if ( texture.GetChannelNumber () != 1u ) break;
			
			imageTexture = &texture;
			return;
		break;

		default:
			// NOTHING
		break;
	}

	GXLogW ( L"EMGaussVerticalBlurMaterial::SetImageTexture::Error - Конфликт материала и количества каналов текстуры\n" );
}
