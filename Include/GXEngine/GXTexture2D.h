// version 1.1

#ifndef GX_TEXTURE_2D
#define GX_TEXTURE_2D


#include "GXOpenGL.h"


class GXTexture2DEntry;
class GXTexture2D
{
	private:
		GXUShort			width;
		GXUShort			height;
		GXUByte				numChannels;
		GXUByte				lods;

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
		GXUByte GetLevelOfDetailNumber () const;

		static GXTexture2D& GXCALL LoadTexture ( const GXWChar* fileName, GXBool isGenerateMipmap, GLint wrapMode, GXBool isApplyGammaCorrection );
		static GXVoid GXCALL RemoveTexture ( GXTexture2D& texture );
		static GXUInt GXCALL GetTotalLoadedTextures ( const GXWChar** lastTexture );

		GXVoid FillWholePixelData ( const GXVoid* data );
		GXVoid FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort regionWidth, GXUShort regionHeight, const GXVoid* data );
		GXVoid UpdateMipmaps ();

		GXVoid Bind ( GXUByte unit );
		GXVoid Unbind ();

		GLuint GetTextureObject () const;

		GXVoid InitResources ( GXUShort textureWidth, GXUShort textureHeight, GLint textureInternalFormat, GXBool isGenerateMipmapPolicy, GLint wrapModePolicy );
		GXVoid FreeResources ();

		GXBool operator == ( const GXTexture2DEntry &other ) const;
		GXTexture2D& operator = ( const GXTexture2D &other );

	private:
		GXTexture2D ( const GXTexture2D &other ) = delete;
};


#endif // GX_TEXTURE_2D
