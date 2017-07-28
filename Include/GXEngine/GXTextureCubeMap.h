//version 1.0

#ifndef GX_TEXTURE_CUBE_MAP
#define GX_TEXTURE_CUBE_MAP


#include "GXOpenGL.h"


class GXTextureCubeMap
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
		GLuint				sampler;

		static GXUInt		refs;

	public:
		GXTextureCubeMap ();
		explicit GXTextureCubeMap ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap );
		~GXTextureCubeMap ();

		GXUShort GetWidth () const;
		GXUShort GetHeight () const;
		GXUByte GetChannelNumber () const;

		static GXUInt GetTotalLoadedTextures ();

		GXVoid FillWholePixelData ( const GXVoid* data, GLenum target );
		GXVoid UpdateMipmaps ();

		GXVoid Bind ( GXUByte textureUnit );
		GXVoid Unbind ();

		GLuint GetTextureObject () const;

		GXVoid InitResources ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap );
		GXVoid FreeResources ();
};


#endif //GX_TEXTURE_CUBE_MAP
