//version 1.0

#ifndef GX_TEXTURE_CUBE_MAP
#define GX_TEXTURE_CUBE_MAP


#include "GXOpenGL.h"
#include "GXEquirectangularToCubeMapMaterial.h"
#include "GXTexture2D.h"
#include <GXCommon/GXFileSystem.h>


class GXTextureCubeMapEntry;
class GXTextureCubeMap
{
	private:
		GXUShort			faceLength;

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
		explicit GXTextureCubeMap ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap );
		~GXTextureCubeMap ();

		GXUShort GetFaceLength () const;
		GXUByte GetChannelNumber () const;

		static GXTextureCubeMap& GXCALL LoadEquirectangularTexture ( const GXWChar* fileName, GXBool isGenerateMipmap );
		static GXVoid GXCALL RemoveTexture ( GXTextureCubeMap& texture );
		static GXUInt GetTotalLoadedTextures ( const GXWChar** lastTexture );

		GXVoid FillWholePixelData ( const GXVoid* data, GLenum target );
		GXVoid UpdateMipmaps ();

		GXVoid Bind ( GXUByte textureUnit );
		GXVoid Unbind ();

		GLuint GetTextureObject () const;

		GXVoid InitResources ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap );
		GXVoid FreeResources ();

		GXBool operator == ( const GXTextureCubeMapEntry &other ) const;
		GXVoid operator = ( const GXTextureCubeMap &other );

	private:
		static GXVoid GXCALL ProjectFace ( GLuint fbo, GLuint textureObject, eGXCubeMapFace face, GXTexture2D &equirectangularTexture );
};


#endif //GX_TEXTURE_CUBE_MAP
