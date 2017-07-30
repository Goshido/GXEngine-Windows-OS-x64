#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXImageLoader.h>


#define INVALID_INTERNAL_FORMAT		0
#define INVALID_UNPACK_ALIGNMENT	0x7FFFFFFF
#define INVALID_FORMAT				0
#define INVALID_TYPE				0
#define INVALID_TEXTURE_UNIT		0xFF
#define INVALID_CHANNEL_NUMBER		0xFF

#define CACHE_DIRECTORY_NAME	L"Cache"
#define CACHE_FILE_EXTENSION	L"cache"


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



struct GXTextureCubeMapCacheHeader
{
	GXUByte				numChannels;
	eGXChannelDataType	channelDataType;

	GXUShort			width;
	GXUShort			height;

	GXBigInt			positiveXPixelOffset;
	GXBigInt			negativeXPixelOffset;
	GXBigInt			positiveYPixelOffset;
	GXBigInt			negativeYPixelOffset;
	GXBigInt			positiveZPixelOffset;
	GXBigInt			negativeZPixelOffset;
};

#pragma pack(pop)

//-------------------------------------------------------------------------------

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

GXTextureCubeMap& GXCALL GXTextureCubeMap::LoadEquirectangularTexture ( const GXWChar* fileName, GXBool isGenerateMipmap )
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

	GXUByte* data = nullptr;

	if ( GXLoadFile ( cacheFileName, (GXVoid**)&data, size, GX_FALSE ) )
	{
		free ( cacheFileName );
		free ( path );
		free ( baseFileName );

		const GXTextureCubeMapCacheHeader* cacheHeader = (const GXTextureCubeMapCacheHeader*)data;
		GLuint internalFormat = INVALID_INTERNAL_FORMAT;

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

		GXTextureCubeMap* texture = new GXTextureCubeMap ( cacheHeader->width, cacheHeader->height, internalFormat, isGenerateMipmap );
		
		texture->FillWholePixelData ( data + cacheHeader->positiveXPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
		texture->FillWholePixelData ( data + cacheHeader->negativeXPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
		texture->FillWholePixelData ( data + cacheHeader->positiveYPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
		texture->FillWholePixelData ( data + cacheHeader->negativeYPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
		texture->FillWholePixelData ( data + cacheHeader->positiveZPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
		texture->FillWholePixelData ( data + cacheHeader->negativeZPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );

		free ( data );

		new GXTextureCubeMapEntry ( *texture, fileName );

		return *texture;
	}

	free ( baseFileName );
	free ( path );

	GXTextureCubeMapCacheHeader cacheHeader;
	GXUInt width = 0;
	GXUInt height = 0;

	GXWChar* extension = nullptr;
	GXGetFileExtension ( &extension, fileName );

	GXFloat* hdrPixels = nullptr;
	GXUByte* ldrPixels = nullptr;
	GXTexture2D equirectangularTexture;
	GXBool success = GX_FALSE;
	GXUBigInt faceSize = 0;

	if ( GXWcscmp ( extension, L"hdr" ) == 0 || GXWcscmp ( extension, L"HDR" ) == 0 )
	{
		success = GXLoadHDRImage ( fileName, width, height, cacheHeader.numChannels, &hdrPixels );
		cacheHeader.channelDataType = eGXChannelDataType::Float;

		switch ( cacheHeader.numChannels )
		{
			case 1:
				equirectangularTexture.InitResources ( width, height, GL_R16F, GX_FALSE, GL_CLAMP_TO_EDGE );
			break;

			case 2:
				equirectangularTexture.InitResources ( width, height, GL_RG16F, GX_FALSE, GL_CLAMP_TO_EDGE );
			break;

			case 3:
				equirectangularTexture.InitResources ( width, height, GL_RGB16F, GX_FALSE, GL_CLAMP_TO_EDGE );
			break;

			case 4:
				equirectangularTexture.InitResources ( width, height, GL_RGBA16F, GX_FALSE, GL_CLAMP_TO_EDGE );
			break;

			default:
				//NOTHING
			break;
		}

		equirectangularTexture.FillWholePixelData ( hdrPixels );
		free ( hdrPixels );
		GXUBigInt faceSize = width * height * cacheHeader.numChannels * sizeof ( GXFloat );
	}
	else
	{
		success = GXLoadLDRImage ( fileName, width, height, cacheHeader.numChannels, &ldrPixels );
		cacheHeader.channelDataType = eGXChannelDataType::UnsignedByte;

		switch ( cacheHeader.numChannels )
		{
			case 1:
				equirectangularTexture.InitResources ( width, height, GL_R8, GX_FALSE, GL_CLAMP_TO_EDGE );
			break;

			case 2:
				equirectangularTexture.InitResources ( width, height, GL_RG8, GX_FALSE, GL_CLAMP_TO_EDGE );
			break;

			case 3:
				equirectangularTexture.InitResources ( width, height, GL_RGB8, GX_FALSE, GL_CLAMP_TO_EDGE );
			break;

			case 4:
				equirectangularTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
			break;

			default:
				//NOTHING
			break;
		}

		equirectangularTexture.FillWholePixelData ( ldrPixels );
		free ( ldrPixels );
		GXUBigInt faceSize = width * height * cacheHeader.numChannels * sizeof ( GXUByte );
	}

	GXSafeFree ( extension );

	cacheHeader.width = (GXUShort)width;
	cacheHeader.height = (GXUShort)height;

	if ( !success )
	{
		free ( cacheFileName );

		GXTextureCubeMap* texture = new GXTextureCubeMap ();

		new GXTextureCubeMapEntry ( *texture, fileName );
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

	cacheHeader.positiveXPixelOffset = sizeof ( GXTextureCubeMapCacheHeader );
	cacheHeader.negativeXPixelOffset = cacheHeader.positiveXPixelOffset + faceSize;
	cacheHeader.positiveYPixelOffset = cacheHeader.negativeXPixelOffset + faceSize;
	cacheHeader.negativeYPixelOffset = cacheHeader.positiveYPixelOffset + faceSize;
	cacheHeader.positiveZPixelOffset = cacheHeader.negativeYPixelOffset + faceSize;
	cacheHeader.negativeZPixelOffset = cacheHeader.positiveZPixelOffset + faceSize;

	GLuint internalFormat = INVALID_INTERNAL_FORMAT;

	switch ( cacheHeader.numChannels )
	{
		case 1:
		{
			switch ( cacheHeader.channelDataType )
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

		case 2:
		{
			switch ( cacheHeader.channelDataType )
			{
				case eGXChannelDataType::UnsignedByte:
					internalFormat = GL_RG8;
				break;

				case eGXChannelDataType::Float:
					internalFormat = GL_RG16F;
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
			switch ( cacheHeader.channelDataType )
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

	GXUShort effectiveFaceWidth = cacheHeader.width / 4;
	GXUShort effectiveFaceHeight = cacheHeader.height / 3;

	GXTextureCubeMap* texture = new GXTextureCubeMap ( effectiveFaceWidth, effectiveFaceHeight, internalFormat, isGenerateMipmap );

	GLuint fbo;
	glGenFramebuffers ( 1, &fbo );

	GXOpenGLState state;
	state.Save ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture->textureObject, 0 );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	glViewport ( 0, 0, (GLsizei)effectiveFaceWidth, (GLsizei)effectiveFaceHeight );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXTextureCubeMap::LoadEquirectangularTexture::Error - Что-то не так с FBO на первом проходе (ошибка 0x%08x)\n", status );

	GXMeshGeometry screenQuad = GXMeshGeometry::LoadFromObj ( L"3D Models/System/ScreenQuad.stm" );

	GXEquirectangularToCubeMapMaterial equirectangularToCubeMapMaterial;
	equirectangularToCubeMapMaterial.SetEquirectangularTexture ( equirectangularTexture );
	equirectangularToCubeMapMaterial.SetSide ( eGXCubeMapSide::PositiveX );
	equirectangularToCubeMapMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	equirectangularToCubeMapMaterial.Unbind ();

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, texture->textureObject, 0 );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXTextureCubeMap::LoadEquirectangularTexture::Error - Что-то не так с FBO на втором проходе (ошибка 0x%08x)\n", status );

	equirectangularToCubeMapMaterial.SetSide ( eGXCubeMapSide::NegativeX );
	equirectangularToCubeMapMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	equirectangularToCubeMapMaterial.Unbind ();

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, texture->textureObject, 0 );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXTextureCubeMap::LoadEquirectangularTexture::Error - Что-то не так с FBO на третьем проходе (ошибка 0x%08x)\n", status );

	equirectangularToCubeMapMaterial.SetSide ( eGXCubeMapSide::PositiveY );
	equirectangularToCubeMapMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	equirectangularToCubeMapMaterial.Unbind ();

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, texture->textureObject, 0 );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXTextureCubeMap::LoadEquirectangularTexture::Error - Что-то не так с FBO на четвёртом проходе (ошибка 0x%08x)\n", status );

	equirectangularToCubeMapMaterial.SetSide ( eGXCubeMapSide::NegativeY );
	equirectangularToCubeMapMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	equirectangularToCubeMapMaterial.Unbind ();

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, texture->textureObject, 0 );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXTextureCubeMap::LoadEquirectangularTexture::Error - Что-то не так с FBO на пятом проходе (ошибка 0x%08x)\n", status );

	equirectangularToCubeMapMaterial.SetSide ( eGXCubeMapSide::PositiveZ );
	equirectangularToCubeMapMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	equirectangularToCubeMapMaterial.Unbind ();

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, texture->textureObject, 0 );

	status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXTextureCubeMap::LoadEquirectangularTexture::Error - Что-то не так с FBO на шестом проходе (ошибка 0x%08x)\n", status );

	equirectangularToCubeMapMaterial.SetSide ( eGXCubeMapSide::NegativeZ );
	equirectangularToCubeMapMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	equirectangularToCubeMapMaterial.Unbind ();

	GXWriteFileStream cacheFile ( cacheFileName );
	cacheFile.Write ( &cacheHeader, sizeof ( GXTextureCubeMapCacheHeader ) );
	// TODO write data
	cacheFile.Close ();

	GXMeshGeometry::RemoveMeshGeometry ( screenQuad );
	glDeleteFramebuffers ( 1, &fbo );
	equirectangularTexture.FreeResources ();
	free ( cacheFileName );

	state.Restore ();

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

GXVoid GXCALL GXTextureCubeMap::SaveSide ( GLuint fbo, GXUShort width, GXUShort height, GLuint textureObject, GXTexture2D &equirectangularTexture, GXWriteFileStream &writeStream, eGXCubeMapSide side, GXUByte numChannels, eGXChannelDataType type )
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, textureObject, 0 );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );

	glDisable ( GL_BLEND );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	glViewport ( 0, 0, (GLsizei)width, (GLsizei)height );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXTextureCubeMap::SaveSide::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	GXMeshGeometry screenQuad = GXMeshGeometry::LoadFromObj ( L"3D Models/System/ScreenQuad.stm" );

	GXEquirectangularToCubeMapMaterial equirectangularToCubeMapMaterial;
	equirectangularToCubeMapMaterial.SetEquirectangularTexture ( equirectangularTexture );
	equirectangularToCubeMapMaterial.SetSide ( eGXCubeMapSide::PositiveX );
	equirectangularToCubeMapMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	equirectangularToCubeMapMaterial.Unbind ();
}
