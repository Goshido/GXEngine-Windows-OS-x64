// version 1.1

#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXTexture2DGammaCorrectorMaterial.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>


#define INVALID_INTERNAL_FORMAT				0
#define INVALID_UNPACK_ALIGNMENT			0x7FFFFFFF
#define INVALID_FORMAT						0
#define INVALID_TYPE						0
#define INVALID_TEXTURE_UNIT				0xFFu
#define INVALID_CHANNEL_NUMBER				0u
#define INVALID_LEVEL_OF_DETAIL_NUMBER		0u

#define CACHE_DIRECTORY_NAME				L"Cache"
#define CACHE_FILE_EXTENSION				L"cache"


static GXTexture2DEntry* gx_TextureHead = nullptr;

class GXTexture2DEntry
{
	public:
		GXTexture2DEntry*		prev;

	private:
		GXInt					refs;
		GXTexture2D*			texture;

	public:
		GXTexture2DEntry*		next;

	private:
		GXWChar*				fileName;

	public:
		explicit GXTexture2DEntry ( GXTexture2D &texture, const GXWChar* fileName );

		const GXWChar* GetFileName () const;
		GXTexture2D& GetTexture () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXTexture2DEntry ();

		GXTexture2DEntry () = delete;
		GXTexture2DEntry ( const GXTexture2DEntry &other ) = delete;
		GXTexture2DEntry& operator = ( const GXTexture2DEntry &other ) = delete;
};

GXTexture2DEntry::GXTexture2DEntry ( GXTexture2D &texture, const GXWChar* fileName ):
	prev ( nullptr ),
	refs ( 1 ),
	texture ( &texture )
{
	GXWcsclone ( &this->fileName, fileName );

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

	if ( refs > 0 ) return;

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

enum class eGXChannelDataType : GXUByte
{
	UnsignedByte,
	Float
};

struct GXTexture2DCacheHeader
{
	GXUByte				numChannels;
	eGXChannelDataType	channelDataType;
	GXUShort			width;
	GXUShort			height;
	GXUBigInt			pixelOffset;
};

#pragma pack(pop)

//----------------------------------------------------------------------

GXTexture2D::GXTexture2D ():
	width ( 0u ),
	height ( 0u ),
	numChannels ( INVALID_CHANNEL_NUMBER ),
	lods ( INVALID_LEVEL_OF_DETAIL_NUMBER ),
	internalFormat ( INVALID_INTERNAL_FORMAT ),
	unpackAlignment ( INVALID_UNPACK_ALIGNMENT ),
	format ( INVALID_TYPE ),
	type ( INVALID_TYPE ),
	textureUnit ( INVALID_TEXTURE_UNIT ),
	textureObject ( 0u ),
	isGenerateMipmap ( GX_FALSE ),
	wrapMode ( GL_CLAMP_TO_EDGE ),
	sampler ( 0u )
{
	// NOTHING
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

GXUByte GXTexture2D::GetLevelOfDetailNumber () const
{
	return lods;
}

GXTexture2D& GXCALL GXTexture2D::LoadTexture ( const GXWChar* fileName, GXBool isGenerateMipmap, GLint wrapMode, GXBool isApplyGammaCorrection )
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

	size += sizeof ( GXWChar );		// L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'/' symbol

	GXWChar* baseFileName = nullptr;
	GXGetBaseFileName ( &baseFileName, fileName );
	size += GXWcslen ( baseFileName ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar );		// L'.' symbol
	size += GXWcslen ( CACHE_FILE_EXTENSION ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'\0' symbol

	GXWChar* cacheFileName = static_cast<GXWChar*> ( malloc ( size ) );
	wsprintfW ( cacheFileName, L"%s/%s/%s.%s", path, CACHE_DIRECTORY_NAME, baseFileName, CACHE_FILE_EXTENSION );

	free ( baseFileName );

	GXUByte* data = nullptr;
	GLint internalFormat = INVALID_INTERNAL_FORMAT;
	GLenum readPixelFormat = GL_INVALID_ENUM;
	GLenum readPixelType = GL_INVALID_ENUM;
	GLint packAlignment = 1;

	if ( GXLoadFile ( cacheFileName, reinterpret_cast<GXVoid**> ( &data ), size, GX_FALSE ) )
	{
		free ( cacheFileName );
		free ( path );

		const GXTexture2DCacheHeader* cacheHeader = (const GXTexture2DCacheHeader*)data;

		switch ( cacheHeader->numChannels )
		{
			case 1:
			{
				switch ( cacheHeader->channelDataType )
				{
					case eGXChannelDataType::UnsignedByte:
						internalFormat = GL_R8;
					break;

					case eGXChannelDataType::Float:
						internalFormat = GL_R16F;
					break;

					default:
						// NOTHING
					break;
				}
			}
			break;

			case 2:
			{
				switch ( cacheHeader->channelDataType )
				{
					case eGXChannelDataType::UnsignedByte:
						internalFormat = GL_RG8;
					break;

					case eGXChannelDataType::Float:
						internalFormat = GL_RG16F;
					break;

					default:
						// NOTHING
					break;
				}
			}
			break;

			case 3:
			{
				switch ( cacheHeader->channelDataType )
				{
					case eGXChannelDataType::UnsignedByte:
						internalFormat = GL_RGB8;
					break;

					case eGXChannelDataType::Float:
						internalFormat = GL_RGB16F;
					break;

					default:
						// NOTHING
					break;
				}
			}
			break;

			case 4:
			{
				switch ( cacheHeader->channelDataType )
				{
					case eGXChannelDataType::UnsignedByte:
						internalFormat = GL_RGBA8;
					break;

					case eGXChannelDataType::Float:
						internalFormat = GL_RGBA16F;
					break;

					default:
						// NOTHING
					break;
				}
			}
			break;

			default:
				// NOTHING
			break;
		}

		GXTexture2D* texture = new GXTexture2D ( cacheHeader->width, cacheHeader->height, internalFormat, isGenerateMipmap, wrapMode );
		texture->FillWholePixelData ( data + cacheHeader->pixelOffset );

		free ( data );

		new GXTexture2DEntry ( *texture, fileName );

		return *texture;
	}

	size = GXWcslen ( path ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'\0' symbol

	GXWChar* cacheDirectory = static_cast<GXWChar*> ( malloc ( size ) );
	wsprintfW ( cacheDirectory, L"%s/%s", path, CACHE_DIRECTORY_NAME );

	free ( path );

	if ( !GXDoesDirectoryExist ( cacheDirectory ) )
		GXCreateDirectory ( cacheDirectory );

	free ( cacheDirectory );

	GXTexture2DCacheHeader cacheHeader;
	GXUInt width = 0u;
	GXUInt height = 0u;

	GXWChar* extension = nullptr;
	GXGetFileExtension ( &extension, fileName );
	GXBool success = GX_FALSE;
	GXTexture2D* texture = nullptr;
	GXUByte* pixels = nullptr;
	GXUPointer pixelSize = 0u;
	GXWriteFileStream cacheFile ( cacheFileName );

	if ( GXWcscmp ( extension, L"hdr" ) == 0 || GXWcscmp ( extension, L"HDR" ) == 0 )
	{
		GXFloat* hdrPixels = nullptr;
		success = GXLoadHDRImage ( fileName, width, height, cacheHeader.numChannels, &hdrPixels );

		if ( success )
		{
			switch ( cacheHeader.numChannels )
			{
				case 1u:
					internalFormat = GL_R16F;
					readPixelFormat = GL_RED;
					readPixelType = GL_FLOAT;
					packAlignment = 2;
				break;

				case 2u:
					internalFormat = GL_RG16F;
					readPixelFormat = GL_RG;
					readPixelType = GL_FLOAT;
					packAlignment = 4;
				break;

				case 3u:
					internalFormat = GL_RGB16F;
					readPixelFormat = GL_RGB;
					readPixelType = GL_FLOAT;
					packAlignment = 2;
				break;

				case 4u:
					internalFormat = GL_RGBA16F;
					readPixelFormat = GL_RGBA;
					readPixelType = GL_FLOAT;
					packAlignment = 8;
				break;

				default:
					// NOTHING
				break;
			}

			cacheHeader.channelDataType = eGXChannelDataType::Float;
			cacheHeader.width = static_cast<GXUShort> ( width );
			cacheHeader.height = static_cast<GXUShort> ( height );
			cacheHeader.pixelOffset = sizeof ( GXTexture2DCacheHeader );

			pixelSize = width * height * cacheHeader.numChannels * sizeof ( GXFloat );

			free ( cacheFileName );

			cacheFile.Write ( &cacheHeader, sizeof ( GXTexture2DCacheHeader ) );

			texture = new GXTexture2D ( cacheHeader.width, cacheHeader.height, internalFormat, isGenerateMipmap, wrapMode );
			texture->FillWholePixelData ( hdrPixels );

			if ( !isApplyGammaCorrection )
			{
				cacheFile.Write ( hdrPixels, pixelSize );
				cacheFile.Close ();

				free ( hdrPixels );
			}
			else
			{
				pixels = reinterpret_cast<GXUByte*> ( hdrPixels );
			}
		}
	}
	else
	{
		GXUByte* ldrPixels = nullptr;
		success = GXLoadLDRImage ( fileName, width, height, cacheHeader.numChannels, &ldrPixels );

		if ( success )
		{
			switch ( cacheHeader.numChannels )
			{
				case 1:
					internalFormat = GL_R8;
					readPixelFormat = GL_RED;
					readPixelType = GL_UNSIGNED_BYTE;
					packAlignment = 1;
				break;

				case 2:
					internalFormat = GL_RG8;
					readPixelFormat = GL_RG;
					readPixelType = GL_UNSIGNED_BYTE;
					packAlignment = 2;
				break;

				case 3:
					internalFormat = GL_RGB8;
					readPixelFormat = GL_RGB;
					readPixelType = GL_UNSIGNED_BYTE;
					packAlignment = 1;
				break;

				case 4:
					internalFormat = GL_RGBA8;
					readPixelFormat = GL_RGBA;
					readPixelType = GL_UNSIGNED_BYTE;
					packAlignment = 4;
				break;

				default:
					// NOTHING
				break;
			}

			cacheHeader.channelDataType = eGXChannelDataType::UnsignedByte;
			cacheHeader.width = static_cast<GXUShort> ( width );
			cacheHeader.height = static_cast<GXUShort> ( height );
			cacheHeader.pixelOffset = sizeof ( GXTexture2DCacheHeader );

			pixelSize = width * height * cacheHeader.numChannels * sizeof ( GXUByte );

			free ( cacheFileName );

			cacheFile.Write ( &cacheHeader, sizeof ( GXTexture2DCacheHeader ) );

			texture = new GXTexture2D ( cacheHeader.width, cacheHeader.height, internalFormat, isGenerateMipmap, wrapMode );
			texture->FillWholePixelData ( ldrPixels );

			if ( !isApplyGammaCorrection )
			{
				cacheFile.Write ( ldrPixels, pixelSize );
				cacheFile.Close ();

				free ( ldrPixels );
			}
			else
			{
				pixels = ldrPixels;
			}
		}
	}

	GXSafeFree ( extension );

	if ( !success )
	{
		GXLogW ( L"GXTexture2D::LoadTexture::Error - Поддерживаются текстуры с количеством каналов 1, 3 и 4 (текущее количество %hhu)\n", cacheHeader.numChannels );
		free ( cacheFileName );

		new GXTexture2DEntry ( *texture, fileName );
		return *texture;
	}

	if ( !isApplyGammaCorrection )
	{
		new GXTexture2DEntry ( *texture, fileName );
		return *texture;
	}

	GXOpenGLState state;
	state.Save ();

	GXTexture2D gammaCorrectedTexture ( cacheHeader.width, cacheHeader.height, internalFormat, isGenerateMipmap, wrapMode );

	GLuint fbo;
	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gammaCorrectedTexture.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, 0u, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

	switch ( gammaCorrectedTexture.GetChannelNumber () )
	{
		case 1:
			glColorMask ( GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE );
		break;

		case 2:
			glColorMask ( GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE );
		break;

		case 3:
			glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
		break;

		case 4:
			glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		break;

		default:
			// NOTHING
		break;
	}

	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00u );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	glViewport ( 0, 0, static_cast<GLsizei> ( gammaCorrectedTexture.GetWidth () ), static_cast<GLsizei> ( gammaCorrectedTexture.GetHeight () ) );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXTexture2D::LoadTexture::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	GXMeshGeometry screenQuad;
	screenQuad.LoadMesh ( L"Meshes/System/ScreenQuad.stm" );
	GXTexture2DGammaCorrectorMaterial gammaCorrectorMaterial;

	gammaCorrectorMaterial.SetsRGBTexture ( *texture );
	gammaCorrectorMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	gammaCorrectorMaterial.Unbind ();

	glPixelStorei ( GL_PACK_ALIGNMENT, packAlignment );
	glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader.width ), static_cast<GLsizei> ( cacheHeader.height ), readPixelFormat, readPixelType, pixels );

	cacheFile.Write ( pixels, pixelSize );
	cacheFile.Close ();

	free ( pixels );

	glDeleteTextures ( 1, &texture->textureObject );
	texture->textureObject = gammaCorrectedTexture.textureObject;
	
	if ( isGenerateMipmap )
		texture->UpdateMipmaps ();

	gammaCorrectedTexture.textureObject = 0u;
	glDeleteSamplers ( 1, &gammaCorrectedTexture.sampler );
	gammaCorrectedTexture.sampler = 0u;

	state.Restore ();
	glDeleteFramebuffers ( 1, &fbo );

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
	GXUInt total = 0u;

	for ( const GXTexture2DEntry* p = gx_TextureHead; p; p = p->next )
		total++;

	if ( total > 0u )
		*lastTexture = gx_TextureHead->GetFileName ();
	else
		*lastTexture = nullptr;

	return total;
}

GXVoid GXTexture2D::FillWholePixelData ( const GXVoid* data )
{
	if ( textureObject == 0u ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, textureObject );

	glPixelStorei ( GL_UNPACK_ALIGNMENT, unpackAlignment );
	glTexImage2D ( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data );

	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	if ( isGenerateMipmap )
		glGenerateMipmap ( GL_TEXTURE_2D );

	glBindTexture ( GL_TEXTURE_2D, 0u );

	GXCheckOpenGLError ();
}

GXVoid GXTexture2D::FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort regionWidth, GXUShort regionHeight, const GXVoid* data )
{
	if ( textureObject == 0u ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, textureObject );

	glPixelStorei ( GL_UNPACK_ALIGNMENT, unpackAlignment );
	glTexSubImage2D ( GL_TEXTURE_2D, 0, static_cast<GLint> ( left ), static_cast<GLint> ( bottom ), static_cast<GLint> ( regionWidth ), static_cast<GLint> ( regionHeight ), format, type, data );
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

	glBindTexture ( GL_TEXTURE_2D, 0u );
}

GXVoid GXTexture2D::UpdateMipmaps ()
{
	if ( textureObject == 0u || !isGenerateMipmap ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, textureObject );

	glGenerateMipmap ( GL_TEXTURE_2D );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	GLfloat maxAnisotropy;
	glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
	glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );

	glBindTexture ( GL_TEXTURE_2D, 0u );
}

GXVoid GXTexture2D::Bind ( GXUByte unit )
{
	textureUnit = unit;

	glBindSampler ( textureUnit, sampler );
	glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + unit ) );
	glBindTexture ( GL_TEXTURE_2D, textureObject );
}

GXVoid GXTexture2D::Unbind ()
{
	glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + textureUnit ) );
	glBindTexture ( GL_TEXTURE_2D, 0u );
	glBindSampler ( textureUnit, 0u );
}

GLuint GXTexture2D::GetTextureObject () const
{
	return textureObject;
}

GXVoid GXTexture2D::InitResources ( GXUShort textureWidth, GXUShort textureHeight, GLint textureInternalFormat, GXBool isGenerateMipmapPolicy, GLint wrapModePolicy )
{
	width = textureWidth;
	height = textureHeight;
	internalFormat = textureInternalFormat;
	isGenerateMipmap = isGenerateMipmapPolicy;
	wrapMode = wrapModePolicy;

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
			numChannels = 1u;
		}
		break;

		case GL_R8:
		case GL_R8I:
		case GL_R8UI:
		{
			unpackAlignment = 1;
			format = GL_RED;
			type = GL_UNSIGNED_BYTE;
			numChannels = 1u;
		}
		break;

		case GL_R16F:
			unpackAlignment = 2;
			format = GL_RED;
			type = GL_FLOAT;
			numChannels = 1u;
		break;

		case GL_R32F:
			unpackAlignment = 4;
			format = GL_RED;
			type = GL_FLOAT;
			numChannels = 1u;
		break;

		case GL_RG8:
		case GL_RG8I:
		case GL_RG8UI:
		{
			unpackAlignment = 2;
			format = GL_RG;
			type = GL_UNSIGNED_BYTE;
			numChannels = 2u;
		}
		break;

		case GL_RG16F:
			unpackAlignment = 4;
			format = GL_RG;
			type = GL_FLOAT;
			numChannels = 2u;
		break;

		case GL_RGB8:
		case GL_RGB8I:
		case GL_RGB8UI:
		{
			unpackAlignment = 1;
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			numChannels = 3u;
		}
		break;

		case GL_RGB16:
		{
			unpackAlignment = 2;
			format = GL_RGB;
			type = GL_UNSIGNED_SHORT;
			numChannels = 3u;
		}
		break;

		case GL_RGB16F:
		{
			unpackAlignment = 2;
			format = GL_RGB;
			type = GL_FLOAT;
			numChannels = 3u;
		}
		break;

		case GL_RGBA8:
		case GL_RGBA8I:
		case GL_RGBA8UI:
		{
			unpackAlignment = 4;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			numChannels = 4u;
		}
		break;

		case GL_RGBA16F:
		{
			unpackAlignment = 8;
			format = GL_RGBA;
			type = GL_FLOAT;
			numChannels = 4u;
		}
		break;

		case GL_DEPTH24_STENCIL8:
		{
			unpackAlignment = 4;
			format = GL_DEPTH_STENCIL;
			type = GL_UNSIGNED_INT_24_8;
			numChannels = 1u;
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

		GXUShort maxSide = width >= height ? width : height;

		lods = 0u;
		GXUShort currentResolution = 1u;

		while ( currentResolution <= maxSide )
		{
			lods++;
			currentResolution *= 2;
		}
	}
	else
	{
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
		samplerInfo.anisotropy = 1.0f;
		samplerInfo.resampling = eGXSamplerResampling::None;
		lods = 1u;
	}

	samplerInfo.wrap = wrapMode;
	sampler = GXCreateSampler ( samplerInfo );

	glBindTexture ( GL_TEXTURE_2D, 0u );

	FillWholePixelData ( nullptr );
}

GXVoid GXTexture2D::FreeResources ()
{
	if ( textureObject == 0u ) return;

	glDeleteTextures ( 1, &textureObject );
	textureObject = 0u;

	width = height = 0u;
	numChannels = INVALID_CHANNEL_NUMBER;
	lods = 0u;
	internalFormat = INVALID_INTERNAL_FORMAT;
	unpackAlignment = INVALID_UNPACK_ALIGNMENT;
	format = INVALID_TYPE;
	type = INVALID_TYPE;
	isGenerateMipmap = GX_FALSE;
	textureUnit = INVALID_TEXTURE_UNIT;
	
	glDeleteSamplers ( 1, &sampler );
	wrapMode = GL_CLAMP_TO_EDGE;
	sampler = 0u;
}

GXBool GXTexture2D::operator == ( const GXTexture2DEntry &other ) const
{
	if ( textureObject != other.GetTexture ().textureObject )
		return GX_FALSE;

	if ( isGenerateMipmap != other.GetTexture ().isGenerateMipmap )
		return GX_FALSE;

	if ( wrapMode != other.GetTexture ().wrapMode )
		return GX_FALSE;

	return GX_TRUE;
}

GXVoid GXTexture2D::operator = ( const GXTexture2D &other )
{
	memcpy ( this, &other, sizeof ( GXTexture2D ) );
}
