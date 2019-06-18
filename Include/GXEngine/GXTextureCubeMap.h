// version 1.8

#ifndef GX_TEXTURE_CUBE_MAP
#define GX_TEXTURE_CUBE_MAP


#include "GXOpenGL.h"
#include "GXEquirectangularToCubeMapMaterial.h"
#include "GXTexture2D.h"


class GXTextureCubeMapEntry;
class GXTextureCubeMap final : public GXMemoryInspector
{
    private:
        GXUByte                     _textureUnit;
        GXTextureCubeMapEntry*      _textureCubeMapEntry;

    public:
        // Creates uninitiated texture resource.
        GXTextureCubeMap ();

        // Creates program texture resource.
        explicit GXTextureCubeMap ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap );

        // Creates reference counting texture resource.
        explicit GXTextureCubeMap ( const GXWChar* equirectangularImage, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection );

        ~GXTextureCubeMap ();

        GXUShort GetFaceLength () const;
        GXUByte GetChannelNumber () const;
        GXUByte GetLevelOfDetailNumber () const;

        // Existing texture resource will be released.
        // Supported image formats:
        // - 24, 34 bit TGA without RLE compression
        // - JPEG
        // - PNG
        // - HDR
        GXVoid LoadEquirectangularImage ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection );

        // If object holds reference counting resource then method releases that resource and makes new
        // texture resource with specified pixel data. Face length, internal format and generate mipmap intend
        // will be same as old reference counting resource.
        // If object holds program texture then method will update pixel data only.
        GXVoid FillWholePixelData ( const GXVoid* data, GLenum target );

        // This method will do nothing if texture resource is created without generate mipmap intend.
        GXVoid UpdateMipmaps ();

        GXVoid Bind ( GXUByte unit );
        GXVoid Unbind ();

        GLuint GetTextureObject () const;

        GXBool IsInitialized () const;
        GXVoid InitResources ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap );
        GXVoid FreeResources ();

        static GXUInt GXCALL GetTotalLoadedTextures ( const GXWChar** lastTexture );

    private:
        GXTextureCubeMap ( const GXTextureCubeMap &other ) = delete;
        GXTextureCubeMap& operator = ( const GXTextureCubeMap &other ) = delete;
};


#endif // GX_TEXTURE_CUBE_MAP
