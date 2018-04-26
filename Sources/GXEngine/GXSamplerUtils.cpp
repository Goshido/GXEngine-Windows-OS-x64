// version 1.2

#include <GXEngine/GXSamplerUtils.h>


GXGLSamplerInfo::GXGLSamplerInfo ():
	wrap ( GL_CLAMP_TO_EDGE ),
	resampling ( eGXSamplerResampling::None ),
	anisotropy ( 1.0f )
{
	// NOTHING
}

GXGLSamplerInfo::~GXGLSamplerInfo ()
{
	// NOTHING
}

//----------------------------------------------------------------------------------

GLuint GXCALL GXCreateSampler ( const GXGLSamplerInfo &samplerInfo )
{
	GLuint sampler;
	glGenSamplers ( 1, &sampler );

	switch ( samplerInfo.resampling )
	{
		case eGXSamplerResampling::Linear:
			glSamplerParameteri ( sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glSamplerParameteri ( sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		break;

		case eGXSamplerResampling::Bilinear:
			glSamplerParameteri ( sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glSamplerParameteri ( sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		break;

		case eGXSamplerResampling::Trilinear:
			glSamplerParameteri ( sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glSamplerParameteri ( sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		break;

		case eGXSamplerResampling::None:
		default:
			glSamplerParameteri ( sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glSamplerParameteri ( sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		break;
	}

	glSamplerParameteri ( sampler, GL_TEXTURE_WRAP_S, samplerInfo.wrap );
	glSamplerParameteri ( sampler, GL_TEXTURE_WRAP_T, samplerInfo.wrap );
	glSamplerParameteri ( sampler, GL_TEXTURE_WRAP_R, samplerInfo.wrap );

	GLfloat maxAnisotropy = 1.0f;
	glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
	GXFloat supportedAnisotropy = ( samplerInfo.anisotropy > maxAnisotropy ) ? maxAnisotropy : samplerInfo.anisotropy;
	glSamplerParameterf ( sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, supportedAnisotropy );

	return sampler;
}
