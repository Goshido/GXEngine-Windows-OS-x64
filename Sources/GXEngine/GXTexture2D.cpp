//version 1.0

#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXFileSystem.h>


#define INVALID_INTERNAL_FORMAT		0
#define INVALID_UNPACK_ALIGNMENT	0x7FFFFFFF
#define INVALID_FORMAT				0
#define INVALID_TYPE				0
#define INVALID_TEXTURE_UNIT		0xFF
#define INVALID_CHANNEL_NUMBER		0xFF

#define CACHE_DIRECTORY_NAME	L"Cache"
#define CACHE_FILE_EXTENSION	L"cache"


static GXTexture2DEntry* gx_TextureHead = nullptr;

class GXTexture2DEntry
{
	public:
		GXTexture2DEntry*		next;
		GXTexture2DEntry*		prev;

	private:
		GXWChar*				fileName;
		GXTexture2D*			texture;

		GXInt					refs;

	public:
		explicit GXTexture2DEntry ( GXTexture2D &texture, const GXWChar* fileName );

		const GXWChar* GetFileName () const;
		GXTexture2D& GetTexture () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXTexture2DEntry ();
};

GXTexture2DEntry::GXTexture2DEntry ( GXTexture2D &texture, const GXWChar* fileName )
{
	GXWcsclone ( &this->fileName, fileName );
	this->texture = &texture;

	refs = 1;

	prev = nullptr;

	if ( gx_TextureHead )
		gx_TextureHead->prev = this;

	next = gx_TextureHead;
	gx_TextureHead = this;
}

const GXWChar* GXTexture2DEntry::GetFileName () const
{
	return fileName;
}

GXTexture2D& GXTexture2DEntry::GetTexture () const
{
	return *texture;
}

GXVoid GXTexture2DEntry::AddRef ()
{
	refs++;
}

GXVoid GXTexture2DEntry::Release ()
{
	refs--;

	if ( refs <= 0 )
		delete this;
}

GXTexture2DEntry::~GXTexture2DEntry ()
{
	GXSafeFree ( fileName );
	delete texture;

	if ( gx_TextureHead == this )
		gx_TextureHead = gx_TextureHead->next;
	else
		prev->next = next;

	if ( next )
		next->prev = prev;
}

//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

struct GXTexture2DCacheHeader
{
	GXUByte		numChannels;
	GXUShort	width;
	GXUShort	height;
};

#pragma pack(pop)

//----------------------------------------------------------------------

GXTexture2D::GXTexture2D ()
{
	width = height = 0;
	numChannels = INVALID_CHANNEL_NUMBER;
	internalFormat = INVALID_INTERNAL_FORMAT;
	unpackAlignment = INVALID_UNPACK_ALIGNMENT;
	format = INVALID_TYPE;
	type = INVALID_TYPE;
	textureUnit = INVALID_TEXTURE_UNIT;
	textureObject = 0;
	isGenerateMipmap = GX_FALSE;
	wrapMode = GL_CLAMP_TO_EDGE;
	sampler = 0;
}

GXTexture2D::GXTexture2D ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap, GLint wrapMode )
{
	InitResources ( width, height, internalFormat, isGenerateMipmap, wrapMode );
}

GXTexture2D::~GXTexture2D ()
{
	FreeResources ();
}

GXUShort GXTexture2D::GetWidth () const
{
	return width;
}

GXUShort GXTexture2D::GetHeight () const
{
	return height;
}

GXUByte GXTexture2D::GetChannelNumber () const
{
	return numChannels;
}

GXTexture2D& GXCALL GXTexture2D::LoadTexture ( const GXWChar* fileName, GXBool isGenerateMipmap, GLint wrapMode )
{
	for ( GXTexture2DEntry* p = gx_TextureHead; p; p = p->next )
	{
		if ( GXWcscmp ( p->GetFileName (), fileName ) == 0 )
		{
			p->AddRef ();
			return p->GetTexture ();
		}
	}

	GXWChar* path = nullptr;
	GXGetFileDirectoryPath ( &path, fileName );
	GXUPointer size = GXWcslen ( path ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar );		//L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		//L'/' symbol

	GXWChar* baseFileName = nullptr;
	GXGetBaseFileName ( &baseFileName, fileName );
	size += GXWcslen ( baseFileName ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar );		//L'.' symbol
	size += GXWcslen ( CACHE_FILE_EXTENSION ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		//L'\0' symbol

	GXWChar* cacheFileName = (GXWChar*)malloc ( size );
	wsprintfW ( cacheFileName, L"%s/%s/%s.%s", path, CACHE_DIRECTORY_NAME, baseFileName, CACHE_FILE_EXTENSION );

	GXUByte* data = nullptr;

	if ( GXLoadFile ( cacheFileName, (GXVoid**)&data, size, GX_FALSE ) )
	{
		free ( cacheFileName );
		free ( path );
		free ( baseFileName );

		const GXTexture2DCacheHeader* cacheHeader = (const GXTexture2DCacheHeader*)data;
		const GXUByte* pixelData = (const GXUByte*)( data + sizeof ( GXTexture2DCacheHeader ) );
		GLuint internalFormat = INVALID_INTERNAL_FORMAT;

		switch ( cacheHeader->numChannels )
		{
			case 1:
				internalFormat = GL_R8;
			break;

			case 2:
				internalFormat = GL_RG8;
			break;

			case 3:
				internalFormat = GL_RGB8;
			break;

			case 4:
				internalFormat = GL_RGBA8;
			break;

			default:
				// NOTHING
			break;
		}

		GXTexture2D* texture = new GXTexture2D ( cacheHeader->width, cacheHeader->height, internalFormat, isGenerateMipmap, wrapMode );
		texture->FillWholePixelData ( pixelData );

		free ( data );

		new GXTexture2DEntry ( *texture, fileName );

		return *texture;
	}

	GXTexture2DCacheHeader cacheHeader;
	GXUInt width = 0;
	GXUInt height = 0;

	if ( !GXLoadLDRImage ( fileName, width, height, cacheHeader.numChannels, &data ) )
	{
		free ( cacheFileName );
		free ( path );
		free ( baseFileName );

		GXTexture2D* texture = new GXTexture2D ();
		new GXTexture2DEntry ( *texture, fileName );
		return *texture;
	}

	size = GXWcslen ( path ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		//L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		//L'\0' symbol

	GXWChar* cacheDirectory = (GXWChar*)malloc ( size );
	wsprintfW ( cacheDirectory, L"%s/%s", path, CACHE_DIRECTORY_NAME );

	if ( !GXDoesDirectoryExist ( cacheDirectory ) )
		GXCreateDirectory ( cacheDirectory );

	free ( cacheDirectory );

	cacheHeader.width = (GXUShort)width;
	cacheHeader.height = (GXUShort)height;
	GLuint internalFormat = INVALID_INTERNAL_FORMAT;

	GXWriteFileStream cacheFile ( cacheFileName );
	cacheFile.Write ( &cacheHeader, sizeof ( GXTexture2DCacheHeader ) );
	cacheFile.Write ( data, cacheHeader.numChannels * width * height );
	cacheFile.Close ();

	switch ( cacheHeader.numChannels )
	{
		case 1:
			internalFormat = GL_R8;
		break;

		case 2:
			internalFormat = GL_RG8;
		break;

		case 3:
			internalFormat = GL_RGB8;
		break;

		case 4:
			internalFormat = GL_RGBA8;
		break;

		default:
			// NOTHING
		break;
	}

	GXTexture2D* texture = new GXTexture2D ( cacheHeader.width, cacheHeader.height, internalFormat, isGenerateMipmap, wrapMode );
	texture->FillWholePixelData ( data );

	free ( data );
	free ( cacheFileName );
	free ( path );
	free ( baseFileName );

	new GXTexture2DEntry ( *texture, fileName );

	return *texture;
}

GXVoid GXCALL GXTexture2D::RemoveTexture ( GXTexture2D& texture )
{
	for ( GXTexture2DEntry* p = gx_TextureHead; p; p = p->next )
	{
		if ( texture == *p )
		{
			p->Release ();
			texture = GXTexture2D ();
			return;
		}
	}
}

GXUInt GXCALL GXTexture2D::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
	GXUInt total = 0;
	for ( GXTexture2DEntry* p = gx_TextureHead; p; p = p->next )
		total++;

	if ( total > 0 )
		*lastTexture = gx_TextureHead->GetFileName ();
	else
		*lastTexture = nullptr;

	return total;
}

GXVoid GXTexture2D::FillWholePixelData ( const GXVoid* data )
{
	if ( textureObject == 0 ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, textureObject );

	glPixelStorei ( GL_UNPACK_ALIGNMENT, unpackAlignment );
	glTexImage2D ( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data );
	glGetError ();

	if ( isGenerateMipmap )
	{
		glGenerateMipmap ( GL_TEXTURE_2D );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

		GLfloat maxAnisotropy;
		glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
		glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );
	}

	glBindTexture ( GL_TEXTURE_2D, 0 );
}

GXVoid GXTexture2D::FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort width, GXUShort height, const GXVoid* data )
{
	if ( textureObject == 0 ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, textureObject );

	glPixelStorei ( GL_UNPACK_ALIGNMENT, unpackAlignment );
	glTexSubImage2D ( GL_TEXTURE_2D, 0, (GLint)left, (GLint)bottom, (GLint)width, (GLint)height, format, type, data );
	glGetError ();

	if ( isGenerateMipmap )
	{
		glGenerateMipmap ( GL_TEXTURE_2D );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

		GLfloat maxAnisotropy;
		glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
		glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );
	}

	glBindTexture ( GL_TEXTURE_2D, 0 );
}

GXVoid GXTexture2D::UpdateMipmaps ()
{
	if ( textureObject == 0 || !isGenerateMipmap ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, textureObject );

	glGenerateMipmap ( GL_TEXTURE_2D );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	GLfloat maxAnisotropy;
	glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
	glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );

	glBindTexture ( GL_TEXTURE_2D, 0 );
}

GXVoid GXTexture2D::Bind ( GXUByte textureUnit )
{
	this->textureUnit = textureUnit;

	glBindSampler ( textureUnit, sampler );
	glActiveTexture ( GL_TEXTURE0 + textureUnit );
	glBindTexture ( GL_TEXTURE_2D, textureObject );
}

GXVoid GXTexture2D::Unbind ()
{
	glActiveTexture ( GL_TEXTURE0 + textureUnit );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( textureUnit, 0 );
}

GLuint GXTexture2D::GetTextureObject () const
{
	return textureObject;
}

GXVoid GXTexture2D::InitResources ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap, GLint wrapMode )
{
	this->width = width;
	this->height = height;
	this->internalFormat = internalFormat;
	this->isGenerateMipmap = isGenerateMipmap;
	this->wrapMode = wrapMode;

	textureUnit = INVALID_TEXTURE_UNIT;

	glActiveTexture ( GL_TEXTURE0 );
	glGenTextures ( 1, &textureObject );
	glBindTexture ( GL_TEXTURE_2D, textureObject );

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
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
		samplerInfo.anisotropy = 1.0f;
		samplerInfo.resampling = eGXSamplerResampling::None;
	}

	samplerInfo.wrap = wrapMode;
	sampler = GXCreateSampler ( samplerInfo );

	glBindTexture ( GL_TEXTURE_2D, 0 );

	FillWholePixelData ( nullptr );
}

GXVoid GXTexture2D::FreeResources ()
{
	if ( textureObject == 0 ) return;

	glDeleteTextures ( 1, &textureObject );
	textureObject = 0;

	width = height = 0;
	internalFormat = INVALID_INTERNAL_FORMAT;
	unpackAlignment = INVALID_UNPACK_ALIGNMENT;
	format = INVALID_TYPE;
	type = INVALID_TYPE;
	isGenerateMipmap = GX_FALSE;
	textureUnit = INVALID_TEXTURE_UNIT;
	
	glDeleteSamplers ( 1, &sampler );
	wrapMode = GL_CLAMP_TO_EDGE;
	sampler = 0;
}

GXBool GXTexture2D::operator == ( const GXTexture2DEntry &other ) const
{
	if ( textureObject != other.GetTexture ().textureObject ) return GX_FALSE;
	if ( isGenerateMipmap != other.GetTexture ().isGenerateMipmap ) return GX_FALSE;
	if ( wrapMode != other.GetTexture ().wrapMode ) return GX_FALSE;

	return GX_TRUE;
}

GXVoid GXTexture2D::operator = ( const GXTexture2D &other )
{
	memcpy ( this, &other, sizeof ( GXTexture2D ) );
}
