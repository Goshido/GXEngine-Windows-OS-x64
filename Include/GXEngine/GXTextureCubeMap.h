// version 1.0

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
		GXUByte				lods;

		GLint				internalFormat;
		GLint				unpackAlignment;
		GLenum				format;
		GLenum				type;

		GXUByte				textureUnit;
		GLuint				textureObject;

		GXBool				isGenerateMipmap;
		GLuint				sampler;

	public:
		GXTextureCubeMap ();
		explicit GXTextureCubeMap ( GXUShort length, GLint internalFormat, GXBool isGenerateMipmap );
		~GXTextureCubeMap ();

		GXUShort GetFaceLength () const;
		GXUByte GetChannelNumber () const;
		GXUByte GetLevelOfDetailNumber () const;

		static GXTextureCubeMap& GXCALL LoadEquirectangularTexture ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection );
		static GXVoid GXCALL RemoveTexture ( GXTextureCubeMap& texture );
		static GXUInt GetTotalLoadedTextures ( const GXWChar** lastTexture );

		GXVoid FillWholePixelData ( const GXVoid* data, GLenum target );
		GXVoid UpdateMipmaps ();

		GXVoid Bind ( GXUByte unit );
		GXVoid Unbind ();

		GLuint GetTextureObject () const;

		GXVoid InitResources ( GXUShort textureFaceLength, GLint textureInternalFormat, GXBool isGenerateMipmapPolicy );
		GXVoid FreeResources ();

		GXBool operator == ( const GXTextureCubeMapEntry &other ) const;
		GXVoid operator = ( const GXTextureCubeMap &other );

	private:
		static GXVoid GXCALL ProjectFaces ( GLuint fbo, GLuint textureObject, GXTexture2D &equirectangularTexture, GXBool isApplyGammaCorrection );
};


#endif // GX_TEXTURE_CUBE_MAP
