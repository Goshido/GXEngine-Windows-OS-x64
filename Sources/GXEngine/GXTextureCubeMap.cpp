#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXSamplerUtils.h>


#define INVALID_INTERNAL_FORMAT		0
#define INVALID_UNPACK_ALIGNMENT	0x7FFFFFFF
#define INVALID_FORMAT				0
#define INVALID_TYPE				0
#define INVALID_TEXTURE_UNIT		0xFF
#define INVALID_CHANNEL_NUMBER		0xFF


GXUInt GXTextureCubeMap::refs = 0;


GXTextureCubeMap::GXTextureCubeMap ()
{
	width = height = 0;
	numChannels = INVALID_CHANNEL_NUMBER;
	internalFormat = INVALID_INTERNAL_FORMAT;
	unpackAlignment = INVALID_UNPACK_ALIGNMENT;
	format = INVALID_FORMAT;
	type = INVALID_TYPE;
	textureUnit = INVALID_TEXTURE_UNIT;
	textureObject = 0;
	isGenerateMipmap = GX_FALSE;
	sampler = 0;

	refs++;
}

GXTextureCubeMap::GXTextureCubeMap ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap )
{
	InitResources ( width, height, internalFormat, isGenerateMipmap );
}

GXTextureCubeMap::~GXTextureCubeMap ()
{
	FreeResources ();
	refs--;
}

GXUShort GXTextureCubeMap::GetWidth () const
{
	return width;
}

GXUShort GXTextureCubeMap::GetHeight () const
{
	return height;
}

GXUByte GXTextureCubeMap::GetChannelNumber () const
{
	return numChannels;
}

GLuint GXTextureCubeMap::GetTotalLoadedTextures ()
{
	return refs;
}

GXVoid GXTextureCubeMap::FillWholePixelData ( const GXVoid* data, GLenum target )
{
	if ( textureObject == 0 ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_CUBE_MAP, textureObject );

	glPixelStorei ( GL_UNPACK_ALIGNMENT, unpackAlignment );
	glTexImage2D ( target, 0, internalFormat, width, height, 0, format, type, data );
	glGetError ();

	if ( isGenerateMipmap )
	{
		glGenerateMipmap ( GL_TEXTURE_CUBE_MAP );
		glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

		GLfloat maxAnisotropy;
		glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
		glTexParameterf ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );
	}

	glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );
}

GXVoid GXTextureCubeMap::UpdateMipmaps ()
{
	if ( textureObject == 0 || !isGenerateMipmap ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_CUBE_MAP, textureObject );

	glGenerateMipmap ( GL_TEXTURE_CUBE_MAP );
	glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	GLfloat maxAnisotropy;
	glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
	glTexParameterf ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );

	glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );
}

GXVoid GXTextureCubeMap::Bind ( GXUByte textureUnit )
{
	this->textureUnit = textureUnit;

	glBindSampler ( textureUnit, sampler );
	glActiveTexture ( GL_TEXTURE0 + textureUnit );
	glBindTexture ( GL_TEXTURE_CUBE_MAP, textureObject );
}

GXVoid GXTextureCubeMap::Unbind ()
{
	glActiveTexture ( GL_TEXTURE0 + textureUnit );
	glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );
	glBindSampler ( textureUnit, 0 );
}

GLuint GXTextureCubeMap::GetTextureObject () const
{
	return textureObject;
}

GXVoid GXTextureCubeMap::InitResources ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap )
{
	this->width = width;
	this->height = height;
	this->internalFormat = internalFormat;
	this->isGenerateMipmap = isGenerateMipmap;

	textureUnit = INVALID_TEXTURE_UNIT;

	glActiveTexture ( GL_TEXTURE0 );
	glGenTextures ( 1, &textureObject );
	glBindTexture ( GL_TEXTURE_CUBE_MAP, textureObject );

	switch ( internalFormat )
	{
		case GL_R32I:
		case GL_R32UI:
		{
			unpackAlignment = 4;
			format = GL_RED;
			type = GL_UNSIGNED_INT;
			numChannels = 1;
		}
		break;

		case GL_R8:
		case GL_R8I:
		case GL_R8UI:
		{
			unpackAlignment = 1;
			format = GL_RED;
			type = GL_UNSIGNED_BYTE;
			numChannels = 1;
		}
		break;

		case GL_RG8:
		case GL_RG8I:
		case GL_RG8UI:
		{
			unpackAlignment = 2;
			format = GL_RG;
			type = GL_UNSIGNED_BYTE;
			numChannels = 2;
		}
		break;

		case GL_RGB8:
		case GL_RGB8I:
		case GL_RGB8UI:
		{
			unpackAlignment = 1;
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			numChannels = 3;
		}
		break;

		case GL_RGB16:
		{
			unpackAlignment = 2;
			format = GL_RGB;
			type = GL_UNSIGNED_SHORT;
			numChannels = 3;
		}
		break;

		case GL_RGB16F:
		{
			unpackAlignment = 2;
			format = GL_RGB;
			type = GL_FLOAT;
			numChannels = 3;
		}
		break;

		case GL_RGBA8:
		case GL_RGBA8I:
		case GL_RGBA8UI:
		{
			unpackAlignment = 4;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			numChannels = 4;
		}
		break;

		case GL_RGBA16F:
		{
			unpackAlignment = 4;
			format = GL_RGBA;
			type = GL_FLOAT;
			numChannels = 4;
		}
		break;

		case GL_DEPTH24_STENCIL8:
		{
			unpackAlignment = 4;
			format = GL_DEPTH_STENCIL;
			type = GL_UNSIGNED_INT_24_8;
			numChannels = 1;
		}
		break;

		default:
		// NOTHING
		break;
	}

	GXGLSamplerInfo samplerInfo;

	if ( isGenerateMipmap )
	{
		samplerInfo.anisotropy = 16.0f;
		samplerInfo.resampling = eGXSamplerResampling::Trilinear;
	}
	else
	{
		glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0 );
		samplerInfo.anisotropy = 1.0f;
		samplerInfo.resampling = eGXSamplerResampling::None;
	}

	samplerInfo.wrap = GL_CLAMP_TO_EDGE;
	sampler = GXCreateSampler ( samplerInfo );

	glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );

	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );
}

GXVoid GXTextureCubeMap::FreeResources ()
{
	if ( textureObject == 0 ) return;

	glDeleteTextures ( 1, &textureObject );
	textureObject = 0;

	width = height = 0;
	internalFormat = INVALID_INTERNAL_FORMAT;
	format = INVALID_TYPE;
	type = INVALID_TYPE;
	isGenerateMipmap = GX_FALSE;
	textureUnit = INVALID_TEXTURE_UNIT;

	glDeleteSamplers ( 1, &sampler );
	sampler = 0;
}
