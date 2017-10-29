#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXImageLoader.h>


#define INVALID_INTERNAL_FORMAT				0
#define INVALID_UNPACK_ALIGNMENT			0x7FFFFFFF
#define INVALID_FORMAT						0
#define INVALID_TYPE						0
#define INVALID_TEXTURE_UNIT				0xFF
#define INVALID_CHANNEL_NUMBER				0
#define INVALID_LEVEL_OF_DETAIL_NUMBER		0

#define CACHE_DIRECTORY_NAME				L"Cache"
#define CACHE_FILE_EXTENSION				L"cache"


static GXTextureCubeMapEntry* gx_TextureHead = nullptr;

class GXTextureCubeMapEntry
{
	public:
		GXTextureCubeMapEntry*		next;
		GXTextureCubeMapEntry*		prev;

	private:
		GXWChar*					fileName;
		GXTextureCubeMap*			texture;

		GXInt						refs;

	public:
		explicit GXTextureCubeMapEntry ( GXTextureCubeMap &texture, const GXWChar* fileName );

		const GXWChar* GetFileName () const;
		GXTextureCubeMap& GetTexture () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXTextureCubeMapEntry ();
};

GXTextureCubeMapEntry::GXTextureCubeMapEntry ( GXTextureCubeMap &texture, const GXWChar* fileName )
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

const GXWChar* GXTextureCubeMapEntry::GetFileName () const
{
	return fileName;
}

GXTextureCubeMap& GXTextureCubeMapEntry::GetTexture () const
{
	return *texture;
}

GXVoid GXTextureCubeMapEntry::AddRef ()
{
	refs++;
}

GXVoid GXTextureCubeMapEntry::Release ()
{
	refs--;

	if ( refs <= 0 )
		delete this;
}

GXTextureCubeMapEntry::~GXTextureCubeMapEntry ()
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

//-------------------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

enum class eGXChannelDataType : GXUByte
{
	UnsignedByte,
	Float
};

struct GXTextureCubeMapCacheHeader
{
	GXUByte				numChannels;
	eGXChannelDataType	channelDataType;

	GXUShort			faceLength;

	GXUBigInt			positiveXPixelOffset;
	GXUBigInt			negativeXPixelOffset;
	GXUBigInt			positiveYPixelOffset;
	GXUBigInt			negativeYPixelOffset;
	GXUBigInt			positiveZPixelOffset;
	GXUBigInt			negativeZPixelOffset;
};

#pragma pack(pop)

//-------------------------------------------------------------------------------

GXTextureCubeMap::GXTextureCubeMap ()
{
	faceLength = 0;
	numChannels = INVALID_CHANNEL_NUMBER;
	lods = INVALID_LEVEL_OF_DETAIL_NUMBER;
	internalFormat = INVALID_INTERNAL_FORMAT;
	unpackAlignment = INVALID_UNPACK_ALIGNMENT;
	format = INVALID_FORMAT;
	type = INVALID_TYPE;
	textureUnit = INVALID_TEXTURE_UNIT;
	textureObject = 0;
	isGenerateMipmap = GX_FALSE;
	sampler = 0;
}

GXTextureCubeMap::GXTextureCubeMap ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap )
{
	InitResources ( faceLength, internalFormat, isGenerateMipmap );
}

GXTextureCubeMap::~GXTextureCubeMap ()
{
	FreeResources ();
}

GXUShort GXTextureCubeMap::GetFaceLength () const
{
	return faceLength;
}

GXUByte GXTextureCubeMap::GetChannelNumber () const
{
	return numChannels;
}

GXUByte GXTextureCubeMap::GetLevelOfDetailNumber () const
{
	return lods;
}

GXTextureCubeMap& GXCALL GXTextureCubeMap::LoadEquirectangularTexture ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection )
{
	for ( GXTextureCubeMapEntry* p = gx_TextureHead; p; p = p->next )
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

	free ( baseFileName );

	GXUByte* data = nullptr;

	if ( GXLoadFile ( cacheFileName, (GXVoid**)&data, size, GX_FALSE ) )
	{
		free ( cacheFileName );
		free ( path );

		const GXTextureCubeMapCacheHeader* cacheHeader = (const GXTextureCubeMapCacheHeader*)data;
		GLint internalFormat = INVALID_INTERNAL_FORMAT;

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
						//NOTHING
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
						//NOTHING
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
						//NOTHING
					break;
				}
			}
			break;

			default:
				//NOTHING
			break;
		}

		GXTextureCubeMap* texture = new GXTextureCubeMap ( cacheHeader->faceLength, internalFormat, isGenerateMipmap );
		
		texture->FillWholePixelData ( data + cacheHeader->positiveXPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
		texture->FillWholePixelData ( data + cacheHeader->negativeXPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
		texture->FillWholePixelData ( data + cacheHeader->positiveYPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
		texture->FillWholePixelData ( data + cacheHeader->negativeYPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
		texture->FillWholePixelData ( data + cacheHeader->positiveZPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
		texture->FillWholePixelData ( data + cacheHeader->negativeZPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );

		free ( data );

		if ( isGenerateMipmap )
			texture->UpdateMipmaps ();

		new GXTextureCubeMapEntry ( *texture, fileName );

		return *texture;
	}

	GXTextureCubeMapCacheHeader cacheHeader;
	GXUInt width = 0;
	GXUInt height = 0;

	GXWChar* extension = nullptr;
	GXGetFileExtension ( &extension, fileName );

	GXFloat* hdrPixels = nullptr;
	GXUByte* ldrPixels = nullptr;
	GXTexture2D equirectangularTexture;
	GXBool success = GX_FALSE;
	GXUPointer faceSize = 0;

	if ( GXWcscmp ( extension, L"hdr" ) == 0 || GXWcscmp ( extension, L"HDR" ) == 0 )
	{
		success = GXLoadHDRImage ( fileName, width, height, cacheHeader.numChannels, &hdrPixels );
		cacheHeader.channelDataType = eGXChannelDataType::Float;

		switch ( cacheHeader.numChannels )
		{
			case 1:
				equirectangularTexture.InitResources ( (GXUShort)width, (GXUShort)height, GL_R16F, GX_FALSE, GL_REPEAT );
			break;

			case 3:
				equirectangularTexture.InitResources ( (GXUShort)width, (GXUShort)height, GL_RGB16F, GX_FALSE, GL_REPEAT );
			break;

			case 4:
				equirectangularTexture.InitResources ( (GXUShort)width, (GXUShort)height, GL_RGBA16F, GX_FALSE, GL_REPEAT );
			break;

			default:
				success = GX_FALSE;
			break;
		}

		equirectangularTexture.FillWholePixelData ( hdrPixels );
		free ( hdrPixels );
		GXUInt faceLength = width / 4;
		faceSize = faceLength * faceLength * cacheHeader.numChannels * sizeof ( GXFloat );
	}
	else
	{
		success = GXLoadLDRImage ( fileName, width, height, cacheHeader.numChannels, &ldrPixels );
		cacheHeader.channelDataType = eGXChannelDataType::UnsignedByte;

		switch ( cacheHeader.numChannels )
		{
			case 1:
				equirectangularTexture.InitResources ( (GXUShort)width, (GXUShort)height, GL_R8, GX_FALSE, GL_REPEAT );
			break;

			case 3:
				equirectangularTexture.InitResources ( (GXUShort)width, (GXUShort)height, GL_RGB8, GX_FALSE, GL_REPEAT );
			break;

			case 4:
				equirectangularTexture.InitResources ( (GXUShort)width, (GXUShort)height, GL_RGBA8, GX_FALSE, GL_REPEAT );
			break;

			default:
				success = GX_FALSE;
			break;
		}

		equirectangularTexture.FillWholePixelData ( ldrPixels );
		free ( ldrPixels );
		GXUInt faceLength = width / 4;
		faceSize = faceLength * faceLength * cacheHeader.numChannels * sizeof ( GXUByte );
	}

	GXSafeFree ( extension );

	if ( !success )
	{
		GXLogW ( L"GXTextureCubeMap::LoadEquirectangularTexture::Error - Поддерживаются текстуры с количеством каналов 1, 3 и 4 (текущее количество %hhu)\n", cacheHeader.numChannels );

		free ( cacheFileName );

		GXTextureCubeMap* texture = new GXTextureCubeMap ();

		new GXTextureCubeMapEntry ( *texture, fileName );
		return *texture;
	}

	cacheHeader.faceLength = (GXUShort)( width / 4 );

	size = GXWcslen ( path ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		//L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		//L'\0' symbol

	GXWChar* cacheDirectory = (GXWChar*)malloc ( size );
	wsprintfW ( cacheDirectory, L"%s/%s", path, CACHE_DIRECTORY_NAME );

	free ( path );

	if ( !GXDoesDirectoryExist ( cacheDirectory ) )
		GXCreateDirectory ( cacheDirectory );

	free ( cacheDirectory );

	cacheHeader.positiveXPixelOffset = sizeof ( GXTextureCubeMapCacheHeader );
	cacheHeader.negativeXPixelOffset = (GXUBigInt)( cacheHeader.positiveXPixelOffset + faceSize );
	cacheHeader.positiveYPixelOffset = (GXUBigInt)( cacheHeader.negativeXPixelOffset + faceSize );
	cacheHeader.negativeYPixelOffset = (GXUBigInt)( cacheHeader.positiveYPixelOffset + faceSize );
	cacheHeader.positiveZPixelOffset = (GXUBigInt)( cacheHeader.negativeYPixelOffset + faceSize );
	cacheHeader.negativeZPixelOffset = (GXUBigInt)( cacheHeader.positiveZPixelOffset + faceSize );

	GLint internalFormat = INVALID_INTERNAL_FORMAT;
	GLenum readPixelFormat = GL_INVALID_ENUM;
	GLenum readPixelType = GL_INVALID_ENUM;
	GLint packAlignment = 1;

	switch ( cacheHeader.numChannels )
	{
		case 1:
		{
			switch ( cacheHeader.channelDataType )
			{
				case eGXChannelDataType::UnsignedByte:
					internalFormat = GL_R8;
					readPixelFormat = GL_RED;
					readPixelType = GL_UNSIGNED_BYTE;
					packAlignment = 1;
				break;

				case eGXChannelDataType::Float:
					internalFormat = GL_R16F;
					readPixelFormat = GL_RED;
					readPixelType = GL_FLOAT;
					packAlignment = 2;
				break;

				default:
					//NOTHING
				break;
			}
		}
		break;

		case 3:
		{
			switch ( cacheHeader.channelDataType )
			{
				case eGXChannelDataType::UnsignedByte:
					internalFormat = GL_RGB8;
					readPixelFormat = GL_RGB;
					readPixelType = GL_UNSIGNED_BYTE;
					packAlignment = 1;
				break;

				case eGXChannelDataType::Float:
					internalFormat = GL_RGB16F;
					readPixelFormat = GL_RGB;
					readPixelType = GL_FLOAT;
					packAlignment = 2;
				break;

				default:
					//NOTHING
				break;
			}
		}
		break;

		case 4:
		{
			switch ( cacheHeader.channelDataType )
			{
				case eGXChannelDataType::UnsignedByte:
					internalFormat = GL_RGBA8;
					readPixelFormat = GL_RGBA;
					readPixelType = GL_UNSIGNED_BYTE;
					packAlignment = 4;
				break;

				case eGXChannelDataType::Float:
					internalFormat = GL_RGBA16F;
					readPixelFormat = GL_RGBA;
					readPixelType = GL_FLOAT;
					packAlignment = 8;
				break;

				default:
					//NOTHING
				break;
			}
		}
		break;

		default:
			//NOTHING
		break;
	}

	GXTextureCubeMap* texture = new GXTextureCubeMap ( cacheHeader.faceLength, internalFormat, isGenerateMipmap );

	GLuint fbo;
	glGenFramebuffers ( 1, &fbo );

	glPixelStorei ( GL_PACK_ALIGNMENT, packAlignment );

	GXWriteFileStream cacheFile ( cacheFileName );
	free ( cacheFileName );
	cacheFile.Write ( &cacheHeader, sizeof ( GXTextureCubeMapCacheHeader ) );

	GXUByte* facePixels = (GXUByte*)malloc ( faceSize );

	GLuint cubeMapTextureObject = texture->GetTextureObject ();
	ProjectFaces ( fbo, cubeMapTextureObject, equirectangularTexture, isApplyGammaCorrection );

	GXOpenGLState state;
	state.Save ();

	glBindFramebuffer ( GL_READ_FRAMEBUFFER, fbo );

	switch ( cacheHeader.numChannels )
	{
		case 1:
			glColorMask ( GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE );
		break;

		case 3:
			glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
		break;

		case 4:
			glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		break;

		default:
			//NOTHING
		break;
	}

	glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubeMapTextureObject, 0 );
	glReadPixels ( 0, 0, (GLsizei)cacheHeader.faceLength, (GLsizei)cacheHeader.faceLength, readPixelFormat, readPixelType, facePixels );
	cacheFile.Write ( facePixels, faceSize );

	glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, cubeMapTextureObject, 0 );
	glReadPixels ( 0, 0, (GLsizei)cacheHeader.faceLength, (GLsizei)cacheHeader.faceLength, readPixelFormat, readPixelType, facePixels );
	cacheFile.Write ( facePixels, faceSize );

	glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, cubeMapTextureObject, 0 );
	glReadPixels ( 0, 0, (GLsizei)cacheHeader.faceLength, (GLsizei)cacheHeader.faceLength, readPixelFormat, readPixelType, facePixels );
	cacheFile.Write ( facePixels, faceSize );

	glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, cubeMapTextureObject, 0 );
	glReadPixels ( 0, 0, (GLsizei)cacheHeader.faceLength, (GLsizei)cacheHeader.faceLength, readPixelFormat, readPixelType, facePixels );
	cacheFile.Write ( facePixels, faceSize );

	glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, cubeMapTextureObject, 0 );
	glReadPixels ( 0, 0, (GLsizei)cacheHeader.faceLength, (GLsizei)cacheHeader.faceLength, readPixelFormat, readPixelType, facePixels );
	cacheFile.Write ( facePixels, faceSize );

	glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, cubeMapTextureObject, 0 );
	glReadPixels ( 0, 0, (GLsizei)cacheHeader.faceLength, (GLsizei)cacheHeader.faceLength, readPixelFormat, readPixelType, facePixels );
	cacheFile.Write ( facePixels, faceSize );

	state.Restore ();

	cacheFile.Close ();
	free ( facePixels );
	glDeleteFramebuffers ( 1, &fbo );
	equirectangularTexture.FreeResources ();

	if ( isGenerateMipmap )
		texture->UpdateMipmaps ();

	new GXTextureCubeMapEntry ( *texture, fileName );
	return *texture;
}

GXVoid GXCALL GXTextureCubeMap::RemoveTexture ( GXTextureCubeMap& texture )
{
	for ( GXTextureCubeMapEntry* p = gx_TextureHead; p; p = p->next )
	{
		if ( texture == *p )
		{
			p->Release ();
			texture = GXTextureCubeMap ();
			return;
		}
	}
}

GXUInt GXTextureCubeMap::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
	GXUInt total = 0;
	for ( GXTextureCubeMapEntry* p = gx_TextureHead; p; p = p->next )
		total++;

	if ( total > 0 )
		*lastTexture = gx_TextureHead->GetFileName ();
	else
		*lastTexture = nullptr;

	return total;
}

GXVoid GXTextureCubeMap::FillWholePixelData ( const GXVoid* data, GLenum target )
{
	if ( textureObject == 0 ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_CUBE_MAP, textureObject );

	glPixelStorei ( GL_UNPACK_ALIGNMENT, unpackAlignment );
	glTexImage2D ( target, 0, internalFormat, (GLsizei)faceLength, (GLsizei)faceLength, 0, format, type, data );

	glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );

	GXCheckOpenGLError ();
}

GXVoid GXTextureCubeMap::UpdateMipmaps ()
{
	GXCheckOpenGLError ();

	if ( textureObject == 0 || !isGenerateMipmap ) return;

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_CUBE_MAP, textureObject );
	glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

	glGenerateMipmap ( GL_TEXTURE_CUBE_MAP );

	glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );
	GXCheckOpenGLError ();
}

GXVoid GXTextureCubeMap::Bind ( GXUByte unit )
{
	textureUnit = unit;

	glBindSampler ( textureUnit, sampler );
	glActiveTexture ( (GLenum)( GL_TEXTURE0 + textureUnit ) );
	glBindTexture ( GL_TEXTURE_CUBE_MAP, textureObject );
}

GXVoid GXTextureCubeMap::Unbind ()
{
	glActiveTexture ( (GLenum)( GL_TEXTURE0 + textureUnit ) );
	glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );
	glBindSampler ( textureUnit, 0 );
}

GLuint GXTextureCubeMap::GetTextureObject () const
{
	return textureObject;
}

GXVoid GXTextureCubeMap::InitResources ( GXUShort textureFaceLength, GLint textureInternalFormat, GXBool isGenerateMipmapPolicy )
{
	faceLength = textureFaceLength;
	internalFormat = textureInternalFormat;
	isGenerateMipmap = isGenerateMipmapPolicy;

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

		case GL_R16F:
			unpackAlignment = 2;
			format = GL_RED;
			type = GL_FLOAT;
			numChannels = 1;
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

		case GL_RG16F:
			unpackAlignment = 4;
			format = GL_RG;
			type = GL_FLOAT;
			numChannels = 2;
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

	glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );

	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
	FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );

	GXGLSamplerInfo samplerInfo;

	if ( isGenerateMipmap )
	{
		samplerInfo.anisotropy = 16.0f;
		samplerInfo.resampling = eGXSamplerResampling::Trilinear;
		UpdateMipmaps ();

		lods = 0;
		GXUShort currentResolution = 1;
		while ( currentResolution <= faceLength )
		{
			lods++;
			currentResolution *= 2;
		}
	}
	else
	{
		glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0 );
		samplerInfo.anisotropy = 1.0f;
		samplerInfo.resampling = eGXSamplerResampling::None;
		lods = 1;
	}

	samplerInfo.wrap = GL_CLAMP_TO_EDGE;
	sampler = GXCreateSampler ( samplerInfo );
}

GXVoid GXTextureCubeMap::FreeResources ()
{
	if ( textureObject == 0 ) return;

	glDeleteTextures ( 1, &textureObject );
	textureObject = 0;

	faceLength = 0;
	numChannels = INVALID_CHANNEL_NUMBER;
	lods = INVALID_LEVEL_OF_DETAIL_NUMBER;
	internalFormat = INVALID_INTERNAL_FORMAT;
	format = INVALID_TYPE;
	type = INVALID_TYPE;
	isGenerateMipmap = GX_FALSE;
	textureUnit = INVALID_TEXTURE_UNIT;

	glDeleteSamplers ( 1, &sampler );
	sampler = 0;
}

GXBool GXTextureCubeMap::operator == ( const GXTextureCubeMapEntry &other ) const
{
	if ( textureObject != other.GetTexture ().textureObject ) return GX_FALSE;
	if ( isGenerateMipmap != other.GetTexture ().isGenerateMipmap ) return GX_FALSE;

	return GX_TRUE;
}

GXVoid GXTextureCubeMap::operator = ( const GXTextureCubeMap &other )
{
	memcpy ( this, &other, sizeof ( GXTextureCubeMap ) );
}

GXVoid GXCALL GXTextureCubeMap::ProjectFaces ( GLuint fbo, GLuint textureObject, GXTexture2D &equirectangularTexture, GXBool isApplyGammaCorrection )
{
	GXOpenGLState state;
	state.Save ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureObject, 0 );

	switch ( equirectangularTexture.GetChannelNumber () )
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
			//NOTHING
		break;
	}

	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	GLsizei faceLength = (GLsizei)( equirectangularTexture.GetWidth () / 4 );
	glViewport ( 0, 0, faceLength, faceLength );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXTextureCubeMap::ProjectSide::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	GXMeshGeometry unitCube = GXMeshGeometry::LoadFromStm ( L"Meshes/System/Unit Cube.stm" );

	GXEquirectangularToCubeMapMaterial equirectangularToCubeMapMaterial;
	equirectangularToCubeMapMaterial.SetEquirectangularTexture ( equirectangularTexture );

	if ( isApplyGammaCorrection )
		equirectangularToCubeMapMaterial.EnableGammaCorrection ();
	else
		equirectangularToCubeMapMaterial.DisableGammaCorrection ();

	equirectangularToCubeMapMaterial.Bind ( GXTransform::GetNullTransform () );
	unitCube.Render ();
	equirectangularToCubeMapMaterial.Unbind ();

	GXMeshGeometry::RemoveMeshGeometry ( unitCube );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	state.Restore ();
}
