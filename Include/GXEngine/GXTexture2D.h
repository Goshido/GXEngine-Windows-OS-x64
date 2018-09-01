// version 1.6

#ifndef GX_TEXTURE_2D
#define GX_TEXTURE_2D


#include "GXOpenGL.h"


// Class handles lazy loading reference counting texture 2D resource creation.

class GXTexture2DEntry;
class GXTexture2D final
{
	private:
		GXTexture2DEntry*	texture2DEntry;
		GXUByte				textureUnit;

	public:
		// Creates uninitiated texture resource.
		GXTexture2D ();

		// Creates program texture resource.
		explicit GXTexture2D ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap );

		// Creates reference counting texture resource.
		explicit GXTexture2D ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection );

		~GXTexture2D ();

		GXUShort GetWidth () const;
		GXUShort GetHeight () const;
		GXUByte GetChannelNumber () const;
		GXUByte GetLevelOfDetailNumber () const;

		// Existing texture resource will be released.
		// Supported image formats:
		// - 24, 34 bit TGA without RLE compression
		// - JPEG
		// - PNG
		// - HDR
		GXVoid LoadImage ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection );

		// If object holds reference counting resource then method releases that resource and makes new
		// texture resource with specified pixel data. Width, height, internal format and generate mipmap intend
		// will be same as old reference counting resource.
		// If object holds program texture then method will update pixel data only.
		GXVoid FillWholePixelData ( const GXVoid* data );

		// If texture holds reference counting resource then method releases that resource and created new
		// texture resource with undefined pixel data. Width, height, internal format and generate mipmap intend
		// will be same as old reference counting resource. Then method will fill specified texture region with
		// specified pixel data.
		// If texture holds program texture then method will update pixel region only.
		GXVoid FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort regionWidth, GXUShort regionHeight, const GXVoid* data );
		
		// This method will do nothing if texture resource is created without generate mipmap intend.
		GXVoid UpdateMipmaps ();

		GXVoid Bind ( GXUByte unit );
		GXVoid Unbind ();

		GLuint GetTextureObject () const;

		GXBool IsInitialized () const;
		GXVoid InitResources ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap );
		GXVoid FreeResources ();

		static GXUInt GXCALL GetTotalLoadedTextures ( const GXWChar** lastTexture );

	private:
		GXTexture2D ( const GXTexture2D &other ) = delete;
		GXTexture2D& operator = ( const GXTexture2D &other ) = delete;
};


#endif // GX_TEXTURE_2D
