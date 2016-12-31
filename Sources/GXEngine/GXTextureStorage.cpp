//version 1.8

#include <GXEngine/GXTextureStorage.h>
#include <GXEngine/GXTextureUtils.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXEngine/GXEngineSettings.h>
#include <cstdio>


class GXTextureUnit
{
	private:
		GXTextureUnit*	next;
		GXTextureUnit*	prev;

		GXInt			refs;

		GXTexture		texture;
		GXWChar*		fileName;

	public:
		GXTextureUnit ( const GXWChar* fileName );

		GXVoid AddRef ();
		GXVoid Release ();

		GXTextureUnit* Find ( const GXWChar* fileName );
		GXTextureUnit* Find ( GLuint texObj );

		const GXTexture& GetTexture ();
		GXVoid SetTexture ( const GXTexture &texture );

		GXUInt CheckMemoryLeak ( const GXWChar** lastTexture );

	private:
		~GXTextureUnit ();
};

GXTextureUnit* gx_strg_TexturesExt = 0;

GXTextureUnit::GXTextureUnit ( const GXWChar* fileName )
{
	if ( !gx_strg_TexturesExt )
		next = prev = 0;
	else
	{
		next = gx_strg_TexturesExt;
		prev = 0;
		gx_strg_TexturesExt->prev = this;
	}

	gx_strg_TexturesExt = this;

	refs = 1;

	GXWcsclone ( &this->fileName, fileName );
}

GXVoid GXTextureUnit::AddRef ()
{
	refs++;
}

GXVoid GXTextureUnit::Release ()
{
	refs--;

	if ( refs < 1 )
		delete this;
}

GXTextureUnit* GXTextureUnit::Find ( const GXWChar* fileName )
{
	GXTextureUnit* p = this;

	while ( p )
	{
		if ( GXWcscmp ( p->fileName, fileName ) == 0 )
			return p;

		p = p->next;
	}

	return 0;
}

GXTextureUnit* GXTextureUnit::Find ( GLuint texObj )
{
	GXTextureUnit* p = this;

	while ( p )
	{
		if ( p->texture.texObj == texObj )
			return p;

		p = p->next;
	}

	return 0;
}

const GXTexture& GXTextureUnit::GetTexture ()
{
	return texture;
}

GXVoid GXTextureUnit::SetTexture ( const GXTexture &texture )
{
	memcpy ( &this->texture, &texture, sizeof ( GXTexture ) );
}

GXUInt GXTextureUnit::CheckMemoryLeak ( const GXWChar** lastTexture )
{
	GXUInt objects = 0;
	*lastTexture = 0;

	for ( GXTextureUnit* p = gx_strg_TexturesExt; p; p = p->next )
	{
		*lastTexture = p->fileName;
		objects++;
	}

	return objects;
}

GXTextureUnit::~GXTextureUnit ()
{
	if ( next ) next->prev = prev;

	if ( prev )
		prev->next = next;
	else
		gx_strg_TexturesExt = next;

	free ( fileName );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDeleteTextures ( 1, &texture.texObj );
}

//--------------------------------------------------------------

struct GXTextureCacheHeader
{
	GXUByte		numChannels;
	GXUShort	width;
	GXUShort	height;
};

//--------------------------------------------------------------

GXVoid GXCALL GXLoadTexture ( const GXWChar* fileName, GXTexture &texture )
{
	if ( !gx_strg_TexturesExt )
	{
		GXTextureUnit* unit = gx_strg_TexturesExt->Find ( fileName );
		if ( unit )
		{
			unit->AddRef ();
			const GXTexture& t = unit->GetTexture ();
			memcpy ( &texture, &t, sizeof ( GXTexture ) );
			return;
		}
	}

	GXWChar* temp;
	GXWcsclone ( &temp, fileName );
	GXUInt size = ( GXWcslen ( temp ) + 15 ) * sizeof ( GXWChar );
	GXWChar* cacheFile = (GXWChar*)malloc ( size );
	GXUInt pos = GXWcslen ( temp ) - 1;

	while ( temp[ pos ] != '.' )
		pos--;
	temp[ pos ] = 0;

	while ( temp[ pos ] != '/' )
		pos--;
	temp[ pos ] = 0;

	pos++;

	wsprintf ( cacheFile, L"%s/Cache/%s.cache", temp, temp + pos );

	free ( temp );
	
	GXTextureUnit* unit = new GXTextureUnit ( fileName );
	GXUByte* data;
	GXGLTextureStruct ts;

	if ( GXLoadFile ( cacheFile, (GXVoid**)&data, size, GX_FALSE ) )
	{
		free ( cacheFile );

		const GXTextureCacheHeader* h = (const GXTextureCacheHeader*)data;

		texture.channels = h->numChannels;
		ts.type = GL_UNSIGNED_BYTE;
		ts.width = texture.width = h->width;
		ts.height = texture.height = h->height;
		ts.data = data + sizeof ( GXTextureCacheHeader );
		ts.wrap = GL_REPEAT;
		ts.resampling = gx_EngineSettings.resampling;
		ts.anisotropy = gx_EngineSettings.anisotropy;

		switch ( h->numChannels )
		{
			case 1:
				ts.format = GL_RED;
				ts.internalFormat = GL_R8;
			break;

			case 2:
				ts.format = GL_RG;
				ts.internalFormat = GL_RG8;
			break;

			case 3:
				ts.format = GL_RGB;
				ts.internalFormat = GL_RGB8;
			break;

			case 4:
				ts.format = GL_RGBA;
				ts.internalFormat = GL_RGBA8;
			break;
		}

		texture.texObj = GXCreateTexture ( ts );

		free ( data );

		unit->SetTexture ( texture );
		return;
	}

	if ( !GXLoadImage ( fileName, texture.width, texture.height, texture.channels, &data ) )
	{
		free ( cacheFile );
		texture.texObj = 0;
		return;
	}

	GXTextureCacheHeader h;

	h.numChannels = texture.channels;
	ts.width = h.width = texture.width;
	ts.height = h.height = texture.height;
	ts.wrap = GL_REPEAT;
	ts.type = GL_UNSIGNED_BYTE;
	ts.data = data;

	GXUInt cachePixelSize = sizeof ( GXTextureCacheHeader );

	switch ( h.numChannels )
	{
		case 1:
			ts.format = GL_RED;
			ts.internalFormat = GL_R8;
			cachePixelSize = ts.width * ts.height;
		break;

		case 2:
			ts.format = GL_RG;
			ts.internalFormat = GL_RG8;
			cachePixelSize = ( ts.width * ts.height ) << 1;
		break;

		case 3:
			ts.format = GL_RGB;
			ts.internalFormat = GL_RGB8;
			cachePixelSize = ts.width * ts.height * 3;
		break;

		case 4:
			ts.format = GL_RGBA;
			ts.internalFormat = GL_RGBA8;
			cachePixelSize = ( ts.width * ts.height ) << 2;
		break;
	}

	texture.texObj = GXCreateTexture ( ts );
	GXUInt totalSize = sizeof ( GXTextureCacheHeader ) + cachePixelSize;

	GXUByte* cache = (GXUByte*)malloc ( totalSize );
	memcpy ( cache, &h, sizeof ( GXTextureCacheHeader ) );
	memcpy ( cache + sizeof ( GXTextureCacheHeader ), data, cachePixelSize );

	GXWriteToFile ( cacheFile, cache, totalSize );
	free ( cacheFile );
	free ( cache );
	free ( data );

	unit->SetTexture ( texture );
}

GXVoid GXCALL GXAddRefTexture ( const GXTexture &texture )
{
	if ( !gx_strg_TexturesExt ) return;

	GXTextureUnit* unit = gx_strg_TexturesExt->Find ( texture.texObj );

	if ( !unit ) return;

	unit->AddRef ();
}

GXVoid GXCALL GXRemoveTexture ( const GXTexture &texture )
{
	if ( !gx_strg_TexturesExt ) return;

	GXTextureUnit* unit = gx_strg_TexturesExt->Find ( texture.texObj );

	if ( !unit ) return;

	unit->Release ();
}

GXUInt GXCALL GXGetTotalTextureStorageObjects ( const GXWChar** lastTexture )
{
	if ( !gx_strg_TexturesExt )
	{
		*lastTexture = 0;
		return 0;
	}

	return gx_strg_TexturesExt->CheckMemoryLeak ( lastTexture );
}
