//version 1.0

#include <GXEngine/GXTexture.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXFileSystem.h>


#define INVALID_INTERNAL_FORMAT	0
#define INVALID_FORMAT			0
#define INVALID_TYPE			0
#define INVALID_TEXTURE_UNIT	0xFF

#define CACHE_FOLDER_NAME L"Cache"
#define CACHE_FILE_EXTENSION L"cache"


static GXTextureEntry* gx_TextureHead = nullptr;

class GXTextureEntry
{
	public:
		GXTextureEntry*		next;
		GXTextureEntry*		prev;

	private:
		GXWChar*			fileName;
		GXTexture*			texture;

		GXInt				refs;

	public:
		explicit GXTextureEntry ( GXTexture &texture, const GXWChar* fileName );

		const GXWChar* GetFileName () const;
		GXTexture& GetTexture () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXTextureEntry ();
};

GXTextureEntry::GXTextureEntry ( GXTexture &texture, const GXWChar* fileName )
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

const GXWChar* GXTextureEntry::GetFileName () const
{
	return fileName;
}

GXTexture& GXTextureEntry::GetTexture () const
{
	return *texture;
}

GXVoid GXTextureEntry::AddRef ()
{
	refs++;
}

GXVoid GXTextureEntry::Release ()
{
	refs--;

	if ( refs <= 0 )
		delete this;
}

GXTextureEntry::~GXTextureEntry ()
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

struct GXTextureCacheHeader
{
	GXUByte		numChannels;
	GXUShort	width;
	GXUShort	height;
};

//----------------------------------------------------------------------

GXTexture::GXTexture ()
{
	width = height = 0;
	internalFormat = INVALID_INTERNAL_FORMAT;
	format = INVALID_TYPE;
	type = INVALID_TYPE;
	isGenerateMipmap = GX_FALSE;
	textureUnit = INVALID_TEXTURE_UNIT;
	textureObject = 0;
}

GXTexture::GXTexture ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap )
{
	InitResources ( width, height, internalFormat, isGenerateMipmap );
}

GXTexture::~GXTexture ()
{
	FreeResources ();
}

GXUShort GXTexture::GetWidth () const
{
	return width;
}

GXUShort GXTexture::GetHeight () const
{
	return height;
}

GXTexture& GXCALL GXTexture::LoadTexture ( const GXWChar* fileName, GXBool isGenerateMipmap )
{
	for ( GXTextureEntry* p = gx_TextureHead; p; p = p->next )
	{
		if ( GXWcscmp ( p->GetFileName (), fileName ) == 0 )
		{
			p->AddRef ();
			return p->GetTexture ();
		}
	}

	GXWChar* path = nullptr;
	GXGetFileDirectoryPath ( &path, fileName );
	GXUInt size = GXWcslen ( path ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar ); // "/" symbol
	size += GXWcslen ( CACHE_FOLDER_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar ); // "/" symbol

	GXWChar* baseFileName = nullptr;
	GXGetBaseFileName ( &baseFileName, fileName );
	size += GXWcslen ( baseFileName ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar ); // "." symbol
	size += GXWcslen ( CACHE_FILE_EXTENSION ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar ); // "\0" symbol

	GXWChar* cacheFileName = (GXWChar*)malloc ( size );
	wsprintfW ( cacheFileName, L"%s/%s/%s.%s", path, CACHE_FOLDER_NAME, baseFileName, CACHE_FILE_EXTENSION );

	GXUByte* data = nullptr;

	if ( GXLoadFile ( cacheFileName, (GXVoid**)&data, size, GX_FALSE ) )
	{
		free ( cacheFileName );
		free ( path );
		free ( baseFileName );

		const GXTextureCacheHeader* cacheHeader = (const GXTextureCacheHeader*)data;
		const GXUByte* pixelData = (const GXUByte*)( data + sizeof ( GXTextureCacheHeader ) );
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

		GXTexture* texture = new GXTexture ( cacheHeader->width, cacheHeader->height, internalFormat, isGenerateMipmap );
		texture->FillWholePixelData ( pixelData );

		free ( data );

		new GXTextureEntry ( *texture, fileName );

		return *texture;
	}

	GXTextureCacheHeader cacheHeader;
	GXUInt width = 0;
	GXUInt height = 0;

	if ( !GXLoadImage ( fileName, width, height, cacheHeader.numChannels, &data ) )
	{
		free ( cacheFileName );
		free ( path );
		free ( baseFileName );

		GXTexture* texture = new GXTexture ();
		new GXTextureEntry ( *texture, fileName );
		return *texture;
	}

	cacheHeader.width = (GXUShort)width;
	cacheHeader.height = (GXUShort)height;
	GLuint internalFormat = INVALID_INTERNAL_FORMAT;

	GXWriteStream cacheFile ( cacheFileName );
	cacheFile.Write ( &cacheHeader, sizeof ( GXTextureCacheHeader ) );
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

	GXTexture* texture = new GXTexture ( cacheHeader.width, cacheHeader.height, internalFormat, isGenerateMipmap );
	texture->FillWholePixelData ( data );

	free ( data );
	free ( cacheFileName );
	free ( path );
	free ( baseFileName );

	new GXTextureEntry ( *texture, fileName );

	return *texture;
}

GXVoid GXCALL GXTexture::RemoveTexture ( GXTexture& texture )
{
	for ( GXTextureEntry* p = gx_TextureHead; p; p = p->next )
	{
		if ( texture == *p )
		{
			p->Release ();
			texture = GXTexture ();
			return;
		}
	}
}

GXUInt GXCALL GXTexture::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
	GXUInt total = 0;
	for ( GXTextureEntry* p = gx_TextureHead; p; p = p->next )
		total++;

	if ( total > 0 )
		*lastTexture = gx_TextureHead->GetFileName ();
	else
		*lastTexture = nullptr;

	return total;
}

GXVoid GXTexture::FillWholePixelData ( const GXVoid* data )
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

GXVoid GXTexture::FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort width, GXUShort height, const GXVoid* data )
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

GXVoid GXTexture::Bind ( GXUByte textureUnit )
{
	this->textureUnit = textureUnit;
	glActiveTexture ( GL_TEXTURE0 + textureUnit );
	glBindTexture ( GL_TEXTURE_2D, textureObject );
}

GXVoid GXTexture::Unbind ()
{
	glActiveTexture ( GL_TEXTURE0 + textureUnit );
	glBindTexture ( GL_TEXTURE_2D, 0 );
}

GLuint GXTexture::GetTextureObject () const
{
	return textureObject;
}

GXVoid GXTexture::InitResources ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap )
{
	this->width = width;
	this->height = height;
	this->internalFormat = internalFormat;
	this->isGenerateMipmap = isGenerateMipmap;

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
		}
		break;

		case GL_R8:
		case GL_R8I:
		case GL_R8UI:
		{
			unpackAlignment = 1;
			format = GL_RED;
			type = GL_UNSIGNED_BYTE;
		}
		break;

		case GL_RG8:
		case GL_RG8I:
		case GL_RG8UI:
		{
			unpackAlignment = 2;
			format = GL_RG;
			type = GL_UNSIGNED_BYTE;
		}
		break;

		case GL_RGB8:
		case GL_RGB8I:
		case GL_RGB8UI:
		{
			unpackAlignment = 1;
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
		}
		break;

		case GL_RGB16:
		{
			unpackAlignment = 4;
			format = GL_RGB;
			type = GL_UNSIGNED_SHORT;
		}
		break;

		case GL_RGBA8:
		case GL_RGBA8I:
		case GL_RGBA8UI:
		{
			unpackAlignment = 4;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		}
		break;

		case GL_RGBA16F:
		{
			unpackAlignment = 4;
			format = GL_RGBA;
			type = GL_HALF_FLOAT;
		}
		break;

		case GL_DEPTH24_STENCIL8:
		{
			unpackAlignment = 4;
			format = GL_DEPTH_STENCIL;
			type = GL_UNSIGNED_INT_24_8;
		}
		break;

		default:
			// NOTHING
		break;
	}

	glBindTexture ( GL_TEXTURE_2D, 0 );

	FillWholePixelData ( nullptr );
}

GXVoid GXTexture::FreeResources ()
{
	if ( textureObject == 0 ) return;

	glDeleteTextures ( 1, &textureObject );

	width = height = 0;
	internalFormat = INVALID_INTERNAL_FORMAT;
	format = INVALID_TYPE;
	type = INVALID_TYPE;
	isGenerateMipmap = GX_FALSE;
	textureUnit = INVALID_TEXTURE_UNIT;
	textureObject = 0;
}

GXBool GXTexture::operator == ( const GXTextureEntry &other ) const
{
	return textureObject == other.GetTexture ().textureObject;
}

GXVoid GXTexture::operator = ( const GXTexture &other )
{
	memcpy ( this, &other, sizeof ( GXTexture ) );
}
