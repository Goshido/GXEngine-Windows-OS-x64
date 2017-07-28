//version 1.0

#ifndef GX_TEXTURE_2D
#define GX_TEXTURE_2D


#include "GXOpenGL.h"


class GXTextureEntry;
class GXTexture2D
{
	private:
		GXUShort			width;
		GXUShort			height;
		GXUByte				numChannels;

		GLint				internalFormat;
		GLint				unpackAlignment;
		GLenum				format;
		GLenum				type;

		GXUByte				textureUnit;
		GLuint				textureObject;

		GXBool				isGenerateMipmap;
		GLint				wrapMode;
		GLuint				sampler;

	public:
		GXTexture2D ();
		explicit GXTexture2D ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap, GLint wrapMode );
		~GXTexture2D ();

		GXUShort GetWidth () const;
		GXUShort GetHeight () const;
		GXUByte GetChannelNumber () const;

		static GXTexture2D& GXCALL LoadTexture ( const GXWChar* fileName, GXBool isGenerateMipmap, GLint wrapMode );
		static GXVoid GXCALL RemoveTexture ( GXTexture2D& texture );
		static GXUInt GXCALL GetTotalLoadedTextures ( const GXWChar** lastTexture );

		GXVoid FillWholePixelData ( const GXVoid* data );
		GXVoid FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort width, GXUShort height, const GXVoid* data );
		GXVoid UpdateMipmaps ();

		GXVoid Bind ( GXUByte textureUnit );
		GXVoid Unbind ();

		GLuint GetTextureObject () const;

		GXVoid InitResources ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap, GLint wrapMode );
		GXVoid FreeResources ();

		GXBool operator == ( const GXTextureEntry &other ) const;
		GXVoid operator = ( const GXTexture2D &other );
};


#endif //GX_TEXTURE_2D
