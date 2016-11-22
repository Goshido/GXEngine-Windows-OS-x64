//version 1.6

#ifndef GX_TEXTURE
#define GX_TEXTURE


#include "GXOpenGL.h"


#define GX_TEXTURE_RESAMPLING_NONE			0
#define GX_TEXTURE_RESAMPLING_LINEAR		1
#define GX_TEXTURE_RESAMPLING_BILINEAR		2
#define GX_TEXTURE_RESAMPLING_TRILINEAR		3


struct GXGLTextureStruct
{
	GLint						internalFormat;
	GLenum						format;
	GLenum						type;
	GLsizei						width;
	GLsizei						height;
	GLint						wrap;
	GXUChar						resampling;
	GXUChar						anisotropy;
	GLvoid*						data;

	GXGLTextureStruct ();
};


GLuint GXCALL GXCreateTexture ( const GXGLTextureStruct &texture_info );


#endif //GX_TEXTURE