// version 1.2

#ifndef GX_SAMPLER_UTILS
#define GX_SAMPLER_UTILS


#include "GXOpenGL.h"


enum class eGXSamplerResampling : GXUByte
{
	None = 0,
	Linear = 1,
	Bilinear = 2,
	Trilinear = 3
};

struct GXGLSamplerInfo
{
	GLint						wrap;
	eGXSamplerResampling		resampling;
	GXFloat						anisotropy;

	GXGLSamplerInfo ();
	~GXGLSamplerInfo ();
};


GLuint GXCALL GXCreateSampler ( const GXGLSamplerInfo &samplerInfo );


#endif // GX_SAMPLER_UTILS
