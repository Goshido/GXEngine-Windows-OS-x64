//version 1.0

#ifndef GX_SAMPLER_UTILS
#define GX_SAMPLER_UTILS


#include "GXOpenGL.h"


enum eGXSamplerResampling : GXUByte
{
	GX_SAMPLER_RESAMPLING_NONE = 0,
	GX_SAMPLER_RESAMPLING_LINEAR = 1,
	GX_SAMPLER_RESAMPLING_BILINEAR = 2,
	GX_SAMPLER_RESAMPLING_TRILINEAR = 3
};

struct GXGLSamplerStruct
{
	GLint						wrap;
	eGXSamplerResampling		resampling;
	GXFloat						anisotropy;

	GXGLSamplerStruct ();
};


GLuint GXCALL GXCreateSampler ( const GXGLSamplerStruct &samplerInfo );


#endif //GX_SAMPLER_UTILS
