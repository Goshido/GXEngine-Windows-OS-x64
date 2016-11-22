//version 1.6

#include <GXEngine/GXTextureUtils.h>


GXGLTextureStruct::GXGLTextureStruct ()
{
	internalFormat = GL_RGB8;
	format = GL_RGB;
	type = GL_UNSIGNED_BYTE;
	width = 0;
	height = 0;
	wrap = GL_CLAMP_TO_EDGE;
	resampling = GX_TEXTURE_RESAMPLING_NONE;
	anisotropy = 1;
	data = 0;
}

//------------------------------------------------------------------------------------------------

GLuint GXCALL GXCreateTexture ( const GXGLTextureStruct &texture_info )
{
	GLuint texture;

	glGenTextures ( 1, &texture );
	glBindTexture ( GL_TEXTURE_2D, texture );
	
	switch ( texture_info.internalFormat )
	{
		case GL_R32I:
		case GL_R32UI:
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 4 );
		break;

		case GL_R8:
		case GL_R8I:
		case GL_R8UI:
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
		break;

		case GL_RG8:
		case GL_RG8I:
		case GL_RG8UI:
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 2 );
		break;

		case GL_RGB8:
		case GL_RGB8I:
		case GL_RGB8UI:
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
		break;

		case GL_RGBA8:
		case GL_RGBA8I:
		case GL_RGBA8UI:
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 4 );
		break;

		case GL_DEPTH24_STENCIL8:
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 4 );
		break;

		default:
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
		break;
	}

	glTexImage2D ( GL_TEXTURE_2D, 0, texture_info.internalFormat, texture_info.width, texture_info.height, 0, texture_info.format, texture_info.type, texture_info.data );
	glGetError ();

	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_info.wrap );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_info.wrap );

	switch ( texture_info.resampling )
	{
		case GX_TEXTURE_RESAMPLING_LINEAR:
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		break;

		case GX_TEXTURE_RESAMPLING_BILINEAR:
			glGenerateMipmap ( GL_TEXTURE_2D );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		break;

		case GX_TEXTURE_RESAMPLING_TRILINEAR:
			glGenerateMipmap ( GL_TEXTURE_2D );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		break;

		case GX_TEXTURE_RESAMPLING_NONE:
		default:
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		break;
	}

	GLfloat maxAnisotropy;
	glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
	GXFloat anisotropy = ( (GXFloat)texture_info.anisotropy > maxAnisotropy ) ? maxAnisotropy : (GXFloat)texture_info.anisotropy;
	glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy );

	glBindTexture ( GL_TEXTURE_2D, 0 );

	return texture;
}