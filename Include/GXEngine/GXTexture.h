//version 1.0

#ifndef GX_TEXTURE
#define GX_TEXTURE


#include "GXOpenGL.h"


class GXTextureEntry;
class GXTexture
{
	private:
		GXUShort			width;
		GXUShort			height;
		GLint				internalFormat;
		GLint				unpackAlignment;
		GLenum				format;
		GLenum				type;
		GXBool				isGenerateMipmap;
		GXUByte				textureUnit;
		GLuint				textureObject;

		static GXTexture	nullTexture;

	public:
		GXTexture ();
		explicit GXTexture ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap );
		~GXTexture ();

		GXUShort GetWidth () const;
		GXUShort GetHeight () const;

		static GXTexture& GXCALL LoadTexture ( const GXWChar* fileName, GXBool isGenerateMipmap );
		static GXVoid GXCALL RemoveTexture ( GXTexture& texture );
		static GXUInt GXCALL GetTotalLoadedTextures ( const GXWChar** lastTexture );

		GXVoid FillWholePixelData ( const GXVoid* data );
		GXVoid FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort width, GXUShort height, const GXVoid* data );

		GXVoid Bind ( GXUByte textureUnit );
		GXVoid Unbind ();

		GLuint GetTextureObject () const;

		GXVoid InitResources ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap );
		GXVoid FreeResources ();

		GXBool operator == ( const GXTextureEntry &other ) const;
		GXVoid operator = ( const GXTexture &other );
};


#endif //GX_TEXTURE
