//version 1.0

#include <GXEngine/GXSamplerUtils.h>


GXGLSamplerStruct::GXGLSamplerStruct ()
{
	wrap = GL_CLAMP_TO_EDGE;
	resampling = GX_SAMPLER_RESAMPLING_NONE;
	anisotropy = 1.0f;
}

//----------------------------------------------------------------------------------

GLuint GXCALL GXCreateSampler ( const GXGLSamplerStruct &samplerInfo )
{
	GLuint sampler;
	glGenSamplers ( 1, &sampler );

	switch ( samplerInfo.resampling )
	{
		case GX_SAMPLER_RESAMPLING_LINEAR:
			glSamplerParameteri ( sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glSamplerParameteri ( sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		break;

		case GX_SAMPLER_RESAMPLING_BILINEAR:
			glSamplerParameteri ( sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glSamplerParameteri ( sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		break;

		case GX_SAMPLER_RESAMPLING_TRILINEAR:
			glSamplerParameteri ( sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glSamplerParameteri ( sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		break;

		case GX_SAMPLER_RESAMPLING_NONE:
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
