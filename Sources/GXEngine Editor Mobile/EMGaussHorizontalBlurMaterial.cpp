#include <GXEngine_Editor_Mobile/EMGaussHorizontalBlurMaterial.h>
#include <GXCommon/GXLogger.h>


#define VERTEX_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL		L"Shaders/Editor Mobile/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL	nullptr
#define FRAGMENT_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL	L"Shaders/Editor Mobile/GaussHorizontalBlurOneChannelFivePixelKernel_fs.txt"

#define IMAGE_SLOT										0


EMGaussHorizontalBlurMaterial::EMGaussHorizontalBlurMaterial ( eEMGaussHorizontalBlurKernelType kernelType )
{
	imageTexture = nullptr;

	static const GLchar* samplerNames[ 1 ] = { "imageSampler" };
	static const GLuint samplerLocations[ 1 ] = { IMAGE_SLOT };

	GXShaderProgramInfo si;
	si.numSamplers = 1;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	this->kernelType = kernelType;

	switch ( kernelType )
	{
		case eEMGaussHorizontalBlurKernelType::ONE_CHANNEL_FIVE_PIXEL_KERNEL:
			si.vs = VERTEX_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL;
			si.gs = GEOMETRY_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL;
			si.fs = FRAGMENT_SHADER_ONE_CHANNEL_FIVE_PIXEL_KERNEL;
		break;

		default:
			//NOTHING
		break;
	}

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );
}

EMGaussHorizontalBlurMaterial::~EMGaussHorizontalBlurMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMGaussHorizontalBlurMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !imageTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );
}

GXVoid EMGaussHorizontalBlurMaterial::Unbind ()
{
	if ( !imageTexture ) return;

	glUseProgram ( 0 );
	imageTexture->Unbind ();
}

GXVoid EMGaussHorizontalBlurMaterial::SetImageTexture ( GXTexture &texture )
{
	switch ( kernelType )
	{
		case eEMGaussHorizontalBlurKernelType::ONE_CHANNEL_FIVE_PIXEL_KERNEL:
			if ( texture.GetChannelNumber () == 1 )
			{
				imageTexture = &texture;
				return;
			}
		break;

		default:
			//NOTHING
		break;
	}

	GXLogW ( L"EMGaussHorizontalBlurMaterial::SetImageTexture::Error - Конфликт материала и количества каналов текстуры\n" );
}
