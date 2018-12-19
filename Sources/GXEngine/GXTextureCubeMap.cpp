// version 1.6

#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXOpenGL.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXUIntAtomic.h>


#define INVALID_INTERNAL_FORMAT             0
#define INVALID_UNPACK_ALIGNMENT            0x7FFFFFFF
#define INVALID_TEXTURE_UNIT                0xFFu
#define INVALID_CHANNEL_NUMBER              0u
#define INVALID_LEVEL_OF_DETAIL_NUMBER      0u

#define CACHE_DIRECTORY_NAME                L"Cache"
#define CACHE_FILE_EXTENSION                L"cache"

//---------------------------------------------------------------------------------------------------------------------

#pragma pack ( push )
#pragma pack ( 1 )

enum class eGXChannelDataType : GXUByte
{
    UnsignedByte,
    Float
};

struct GXTextureCubeMapCacheHeader final
{
    GXUByte                 numChannels;
    eGXChannelDataType      channelDataType;

    GXUShort                faceLength;

    GXUBigInt               positiveXPixelOffset;
    GXUBigInt               negativeXPixelOffset;
    GXUBigInt               positiveYPixelOffset;
    GXUBigInt               negativeYPixelOffset;
    GXUBigInt               positiveZPixelOffset;
    GXUBigInt               negativeZPixelOffset;
};

#pragma pack ( pop )

//---------------------------------------------------------------------------------------------------------------------

class GXTextureCubeMapEntry final
{
    private:
        GXTextureCubeMapEntry*              previous;
        GXTextureCubeMapEntry*              next;
        GXUIntAtomic                        references;

        GXWChar*                            fileName;

        GXUShort                            faceLength;
        GXUByte                             numChannels;
        GXUByte                             lods;

        GLint                               internalFormat;
        GLint                               unpackAlignment;
        GLenum                              format;
        GLenum                              type;

        GLuint                              textureObject;
        GXBool                              isGenerateMipmap;

        static GXTextureCubeMapEntry*       top;

    public:
        explicit GXTextureCubeMapEntry ( const GXWChar* equirectangularTextureFileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection );
        explicit GXTextureCubeMapEntry ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap );

        const GXWChar* GetFileName () const;

        GLuint GetTextureObject () const;

        GXUShort GetFaceLength () const;
        GXUByte GetChannelNumber () const;
        GXUByte GetLevelOfDetailNumber () const;
        GLint GetInternalFormat () const;
        GXBool IsGenerateMipmap () const;

        GXVoid FillWholePixelData ( const GXVoid* data, GLenum target );
        GXVoid UpdateMipmaps ();

        GXVoid AddReference ();
        GXVoid Release ();

        // Method returns valid pointer if texture was found.
        // Method returns nullptr if texture was not found.
        static GXTextureCubeMapEntry* GXCALL Find ( const GXWChar* equirectangularTextureFileName );

        static GXUInt GXCALL GetTotalLoadedTextures ( const GXWChar** lastTexture );

    private:
        ~GXTextureCubeMapEntry ();

        GXVoid InitResources ( GXUShort textureFaceLength, GLint textureInternalFormat, GXBool isGenerateMipmapPolicy );
        GXVoid ProjectFaces ( GLuint fbo, GLuint faceTextureObject, GXTexture2D &equirectangularTexture, GXBool isApplyGammaCorrection );

        GXTextureCubeMapEntry () = delete;
        GXTextureCubeMapEntry ( const GXTextureCubeMapEntry &other ) = delete;
        GXTextureCubeMapEntry& operator = ( const GXTextureCubeMapEntry &other ) = delete;
};

GXTextureCubeMapEntry* GXTextureCubeMapEntry::top = nullptr;

GXTextureCubeMapEntry::GXTextureCubeMapEntry ( const GXWChar* equirectangularTextureFileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection ):
    previous ( nullptr ),
    next ( top ),
    references ( 1u )
{
    if ( top )
        top->previous = this;

    top = this;

    GXWcsclone ( &this->fileName, equirectangularTextureFileName );

    GXWChar* path = nullptr;
    GXGetFileDirectoryPath ( &path, equirectangularTextureFileName );
    GXUPointer size = GXWcslen ( path ) * sizeof ( GXWChar );

    size += sizeof ( GXWChar );        // L'/' symbol
    size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
    size += sizeof ( GXWChar );        // L'/' symbol

    GXWChar* baseFileName = nullptr;
    GXGetBaseFileName ( &baseFileName, fileName );
    size += GXWcslen ( baseFileName ) * sizeof ( GXWChar );

    size += sizeof ( GXWChar );        // L'.' symbol
    size += GXWcslen ( CACHE_FILE_EXTENSION ) * sizeof ( GXWChar );
    size += sizeof ( GXWChar );        // L'\0' symbol

    GXWChar* cacheFileName = static_cast<GXWChar*> ( malloc ( size ) );
    wsprintfW ( cacheFileName, L"%s/%s/%s.%s", path, CACHE_DIRECTORY_NAME, baseFileName, CACHE_FILE_EXTENSION );

    free ( baseFileName );

    GXUByte* data = nullptr;

    if ( GXLoadFile ( cacheFileName, reinterpret_cast<GXVoid**> ( &data ), size, GX_FALSE ) )
    {
        free ( cacheFileName );
        free ( path );

        const GXTextureCubeMapCacheHeader* cacheHeader = reinterpret_cast<const GXTextureCubeMapCacheHeader*> ( data );
        GLint resolvedInternalFormat = INVALID_INTERNAL_FORMAT;

        switch ( cacheHeader->numChannels )
        {
            case 1u:
            {
                switch ( cacheHeader->channelDataType )
                {
                    case eGXChannelDataType::UnsignedByte:
                        resolvedInternalFormat = GL_R8;
                    break;

                    case eGXChannelDataType::Float:
                        resolvedInternalFormat = GL_R16F;
                    break;

                    default:
                        // NOTHING
                    break;
                }
            }
            break;

            case 3u:
            {
                switch ( cacheHeader->channelDataType )
                {
                    case eGXChannelDataType::UnsignedByte:
                        resolvedInternalFormat = GL_RGB8;
                    break;

                    case eGXChannelDataType::Float:
                        resolvedInternalFormat = GL_RGB16F;
                    break;

                    default:
                        // NOTHING
                    break;
                }
            }
            break;

            case 4u:
            {
                switch ( cacheHeader->channelDataType )
                {
                    case eGXChannelDataType::UnsignedByte:
                        resolvedInternalFormat = GL_RGBA8;
                    break;

                    case eGXChannelDataType::Float:
                        resolvedInternalFormat = GL_RGBA16F;
                    break;

                    default:
                        // NOTHING
                    break;
                }
            }
            break;

            default:
            // NOTHING
            break;
        }

        InitResources ( cacheHeader->faceLength, resolvedInternalFormat, isGenerateMipmap );

        FillWholePixelData ( data + cacheHeader->positiveXPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
        FillWholePixelData ( data + cacheHeader->negativeXPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
        FillWholePixelData ( data + cacheHeader->positiveYPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
        FillWholePixelData ( data + cacheHeader->negativeYPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
        FillWholePixelData ( data + cacheHeader->positiveZPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
        FillWholePixelData ( data + cacheHeader->negativeZPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );

        free ( data );

        if ( isGenerateMipmap )
            UpdateMipmaps ();

        return;
    }

    GXTextureCubeMapCacheHeader cacheHeader;
    GXUInt width = 0u;
    GXUInt height = 0u;

    GXWChar* extension = nullptr;
    GXGetFileExtension ( &extension, fileName );

    GXFloat* hdrPixels = nullptr;
    GXUByte* ldrPixels = nullptr;
    GXTexture2D equirectangularTexture;
    GXBool success = GX_FALSE;
    GXUPointer faceSize = 0;

    if ( GXWcscmp ( extension, L"hdr" ) == 0 || GXWcscmp ( extension, L"HDR" ) == 0 )
    {
        success = GXLoadHDRImage ( fileName, width, height, cacheHeader.numChannels, &hdrPixels );
        cacheHeader.channelDataType = eGXChannelDataType::Float;

        switch ( cacheHeader.numChannels )
        {
            case 1u:
                equirectangularTexture.InitResources ( static_cast<GXUShort> ( width ), static_cast<GXUShort> ( height ), GL_R16F, GX_FALSE );
            break;

            case 3u:
                equirectangularTexture.InitResources ( static_cast<GXUShort> ( width ), static_cast<GXUShort> ( height ), GL_RGB16F, GX_FALSE );
            break;

            case 4u:
                equirectangularTexture.InitResources ( static_cast<GXUShort> ( width ), static_cast<GXUShort> ( height ), GL_RGBA16F, GX_FALSE );
            break;

            default:
                success = GX_FALSE;
            break;
        }

        equirectangularTexture.FillWholePixelData ( hdrPixels );
        free ( hdrPixels );
        GXUInt resolvedFaceLength = width / 4u;
        faceSize = resolvedFaceLength * resolvedFaceLength * cacheHeader.numChannels * sizeof ( GXFloat );
    }
    else
    {
        success = GXLoadLDRImage ( fileName, width, height, cacheHeader.numChannels, &ldrPixels );
        cacheHeader.channelDataType = eGXChannelDataType::UnsignedByte;

        switch ( cacheHeader.numChannels )
        {
            case 1u:
                equirectangularTexture.InitResources ( static_cast<GXUShort> ( width ), static_cast<GXUShort> ( height ), GL_R8, GX_FALSE );
            break;

            case 3u:
                equirectangularTexture.InitResources ( static_cast<GXUShort> ( width ), static_cast<GXUShort> ( height ), GL_RGB8, GX_FALSE );
            break;

            case 4u:
                equirectangularTexture.InitResources ( static_cast<GXUShort> ( width ), static_cast<GXUShort> ( height ), GL_RGBA8, GX_FALSE );
            break;

            default:
                success = GX_FALSE;
            break;
        }

        equirectangularTexture.FillWholePixelData ( ldrPixels );
        free ( ldrPixels );
        GXUInt resolvedFaceLength = width / 4u;
        faceSize = resolvedFaceLength * resolvedFaceLength * cacheHeader.numChannels * sizeof ( GXUByte );
    }

    GXSafeFree ( extension );

    if ( !success )
    {
        GXLogW ( L"GXTextureCubeMap::LoadEquirectangularTexture::Error - Поддерживаются текстуры с количеством каналов 1, 3 и 4 (текущее количество %hhu)\n", cacheHeader.numChannels );
        free ( cacheFileName );
        return;
    }

    cacheHeader.faceLength = static_cast<GXUShort> ( width / 4u );

    size = GXWcslen ( path ) * sizeof ( GXWChar );
    size += sizeof ( GXWChar );        // L'/' symbol
    size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
    size += sizeof ( GXWChar );        // L'\0' symbol

    GXWChar* cacheDirectory = static_cast<GXWChar*> ( malloc ( size ) );
    wsprintfW ( cacheDirectory, L"%s/%s", path, CACHE_DIRECTORY_NAME );

    free ( path );

    if ( !GXDoesDirectoryExist ( cacheDirectory ) )
        GXCreateDirectory ( cacheDirectory );

    free ( cacheDirectory );

    cacheHeader.positiveXPixelOffset = sizeof ( GXTextureCubeMapCacheHeader );
    cacheHeader.negativeXPixelOffset = static_cast<GXUBigInt> ( cacheHeader.positiveXPixelOffset + faceSize );
    cacheHeader.positiveYPixelOffset = static_cast<GXUBigInt> ( cacheHeader.negativeXPixelOffset + faceSize );
    cacheHeader.negativeYPixelOffset = static_cast<GXUBigInt> ( cacheHeader.positiveYPixelOffset + faceSize );
    cacheHeader.positiveZPixelOffset = static_cast<GXUBigInt> ( cacheHeader.negativeYPixelOffset + faceSize );
    cacheHeader.negativeZPixelOffset = static_cast<GXUBigInt> ( cacheHeader.positiveZPixelOffset + faceSize );

    GLint resolvedInternalFormat = INVALID_INTERNAL_FORMAT;
    GLenum readPixelFormat = GL_INVALID_ENUM;
    GLenum readPixelType = GL_INVALID_ENUM;
    GLint packAlignment = 1;

    switch ( cacheHeader.numChannels )
    {
        case 1u:
        {
            switch ( cacheHeader.channelDataType )
            {
                case eGXChannelDataType::UnsignedByte:
                    resolvedInternalFormat = GL_R8;
                    readPixelFormat = GL_RED;
                    readPixelType = GL_UNSIGNED_BYTE;
                    packAlignment = 1;
                break;

                case eGXChannelDataType::Float:
                    resolvedInternalFormat = GL_R16F;
                    readPixelFormat = GL_RED;
                    readPixelType = GL_FLOAT;
                    packAlignment = 2;
                break;

                default:
                    // NOTHING
                break;
            }
        }
        break;

        case 3u:
        {
            switch ( cacheHeader.channelDataType )
            {
                case eGXChannelDataType::UnsignedByte:
                    resolvedInternalFormat = GL_RGB8;
                    readPixelFormat = GL_RGB;
                    readPixelType = GL_UNSIGNED_BYTE;
                    packAlignment = 1;
                break;

                case eGXChannelDataType::Float:
                    resolvedInternalFormat = GL_RGB16F;
                    readPixelFormat = GL_RGB;
                    readPixelType = GL_FLOAT;
                    packAlignment = 2;
                break;

                default:
                    // NOTHING
                break;
            }
        }
        break;

        case 4u:
        {
            switch ( cacheHeader.channelDataType )
            {
                case eGXChannelDataType::UnsignedByte:
                    resolvedInternalFormat = GL_RGBA8;
                    readPixelFormat = GL_RGBA;
                    readPixelType = GL_UNSIGNED_BYTE;
                    packAlignment = 4;
                break;

                case eGXChannelDataType::Float:
                    resolvedInternalFormat = GL_RGBA16F;
                    readPixelFormat = GL_RGBA;
                    readPixelType = GL_FLOAT;
                    packAlignment = 8;
                break;

                default:
                    // NOTHING
                break;
            }
        }
        break;

        default:
            // NOTHING
        break;
    }

    InitResources ( cacheHeader.faceLength, resolvedInternalFormat, isGenerateMipmap );

    GLuint fbo;
    glGenFramebuffers ( 1, &fbo );

    glPixelStorei ( GL_PACK_ALIGNMENT, packAlignment );

    GXWriteFileStream cacheFile ( cacheFileName );
    free ( cacheFileName );
    cacheFile.Write ( &cacheHeader, sizeof ( GXTextureCubeMapCacheHeader ) );

    GXUByte* facePixels = static_cast<GXUByte*> ( malloc ( faceSize ) );

    ProjectFaces ( fbo, textureObject, equirectangularTexture, isApplyGammaCorrection );

    GXOpenGLState state;
    state.Save ();

    glBindFramebuffer ( GL_READ_FRAMEBUFFER, fbo );

    switch ( cacheHeader.numChannels )
    {
        case 1u:
            glColorMask ( GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE );
        break;

        case 3u:
            glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
        break;

        case 4u:
            glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
        break;

        default:
            // NOTHING
        break;
    }

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader.faceLength ), static_cast<GLsizei> ( cacheHeader.faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader.faceLength ), static_cast<GLsizei> ( cacheHeader.faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader.faceLength ), static_cast<GLsizei> ( cacheHeader.faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader.faceLength ), static_cast<GLsizei> ( cacheHeader.faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader.faceLength ), static_cast<GLsizei> ( cacheHeader.faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader.faceLength ), static_cast<GLsizei> ( cacheHeader.faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    state.Restore ();

    cacheFile.Close ();
    free ( facePixels );
    glDeleteFramebuffers ( 1, &fbo );

    if ( !isGenerateMipmap ) return;

    UpdateMipmaps ();
}

GXTextureCubeMapEntry::GXTextureCubeMapEntry ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap ):
    previous ( nullptr ),
    next ( nullptr ),
    references ( 1u ),
    fileName ( nullptr )
{
    InitResources ( faceLength, internalFormat, isGenerateMipmap );
}

const GXWChar* GXTextureCubeMapEntry::GetFileName () const
{
    return fileName;
}

GLuint GXTextureCubeMapEntry::GetTextureObject () const
{
    return textureObject;
}

GXUShort GXTextureCubeMapEntry::GetFaceLength () const
{
    return faceLength;
}

GXUByte GXTextureCubeMapEntry::GetChannelNumber () const
{
    return numChannels;
}

GXUByte GXTextureCubeMapEntry::GetLevelOfDetailNumber () const
{
    return lods;
}

GLint GXTextureCubeMapEntry::GetInternalFormat () const
{
    return internalFormat;
}

GXBool GXTextureCubeMapEntry::IsGenerateMipmap () const
{
    return isGenerateMipmap;
}

GXVoid GXTextureCubeMapEntry::FillWholePixelData ( const GXVoid* data, GLenum target )
{
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_CUBE_MAP, textureObject );

    glPixelStorei ( GL_UNPACK_ALIGNMENT, unpackAlignment );
    glTexImage2D ( target, 0, internalFormat, (GLsizei)faceLength, (GLsizei)faceLength, 0, format, type, data );

    glBindTexture ( GL_TEXTURE_CUBE_MAP, 0u );

    GXCheckOpenGLError ();
}

GXVoid GXTextureCubeMapEntry::UpdateMipmaps ()
{
    if ( textureObject == 0u || !isGenerateMipmap ) return;

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_CUBE_MAP, textureObject );
    glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

    glGenerateMipmap ( GL_TEXTURE_CUBE_MAP );

    glBindTexture ( GL_TEXTURE_CUBE_MAP, 0u );
    GXCheckOpenGLError ();
}

GXVoid GXTextureCubeMapEntry::AddReference ()
{
    ++references;
}

GXVoid GXTextureCubeMapEntry::Release ()
{
    --references;

    if ( references > 0u ) return;

    delete this;
}

GXTextureCubeMapEntry* GXCALL GXTextureCubeMapEntry::Find ( const GXWChar* equirectangularTextureFileName )
{
    for ( GXTextureCubeMapEntry* p = top; p; p = p->next )
    {
        if ( GXWcscmp ( p->fileName, equirectangularTextureFileName ) != 0 ) continue;
        
        return p;
    }

    return nullptr;
}

GXUInt GXCALL GXTextureCubeMapEntry::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
    GXUInt total = 0u;

    for ( GXTextureCubeMapEntry* p = top; p; p = p->next )
        ++total;

    if ( total > 0u )
        *lastTexture = top->fileName;
    else
        *lastTexture = nullptr;

    return total;
}

GXTextureCubeMapEntry::~GXTextureCubeMapEntry ()
{
    if ( top == this )
        top = top->next;

    if ( previous )
        previous->next = next;

    if ( next )
        next->previous = previous;

    GXSafeFree ( fileName );

    if ( textureObject == 0u ) return;

    glDeleteTextures ( 1, &textureObject );
}

GXVoid GXTextureCubeMapEntry::InitResources ( GXUShort textureFaceLength, GLint textureInternalFormat, GXBool isGenerateMipmapPolicy )
{
    faceLength = textureFaceLength;
    internalFormat = textureInternalFormat;
    isGenerateMipmap = isGenerateMipmapPolicy;

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
            numChannels = 1u;
        }
        break;

        case GL_R8:
        case GL_R8I:
        case GL_R8UI:
        {
            unpackAlignment = 1;
            format = GL_RED;
            type = GL_UNSIGNED_BYTE;
            numChannels = 1u;
        }
        break;

        case GL_R16F:
            unpackAlignment = 2;
            format = GL_RED;
            type = GL_FLOAT;
            numChannels = 1u;
        break;

        case GL_RG8:
        case GL_RG8I:
        case GL_RG8UI:
        {
            unpackAlignment = 2;
            format = GL_RG;
            type = GL_UNSIGNED_BYTE;
            numChannels = 2u;
        }
        break;

        case GL_RG16F:
            unpackAlignment = 4;
            format = GL_RG;
            type = GL_FLOAT;
            numChannels = 2u;
        break;

        case GL_RGB8:
        case GL_RGB8I:
        case GL_RGB8UI:
        {
            unpackAlignment = 1;
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            numChannels = 3u;
        }
        break;

        case GL_RGB16:
        {
            unpackAlignment = 2;
            format = GL_RGB;
            type = GL_UNSIGNED_SHORT;
            numChannels = 3u;
        }
        break;

        case GL_RGB16F:
        {
            unpackAlignment = 2;
            format = GL_RGB;
            type = GL_FLOAT;
            numChannels = 3u;
        }
        break;

        case GL_RGBA8:
        case GL_RGBA8I:
        case GL_RGBA8UI:
        {
            unpackAlignment = 4;
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            numChannels = 4u;
        }
        break;

        case GL_RGBA16F:
        {
            unpackAlignment = 4;
            format = GL_RGBA;
            type = GL_FLOAT;
            numChannels = 4u;
        }
        break;

        case GL_DEPTH24_STENCIL8:
        {
            unpackAlignment = 4;
            format = GL_DEPTH_STENCIL;
            type = GL_UNSIGNED_INT_24_8;
            numChannels = 1u;
        }
        break;

        default:
            // NOTHING
        break;
    }

    glBindTexture ( GL_TEXTURE_CUBE_MAP, 0u );

    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );

    if ( !isGenerateMipmap )
    {
        glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0 );
        lods = 1u;
        return;
    }

    UpdateMipmaps ();

    lods = 0u;
    GXUShort currentResolution = 1u;

    while ( currentResolution <= faceLength )
    {
        ++lods;
        currentResolution *= 2u;
    }
}

GXVoid GXTextureCubeMapEntry::ProjectFaces ( GLuint fbo, GLuint cubeMapTextureObject, GXTexture2D &equirectangularTexture, GXBool isApplyGammaCorrection )
{
    GXOpenGLState state;
    state.Save ();

    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cubeMapTextureObject, 0 );

    switch ( equirectangularTexture.GetChannelNumber () )
    {
        case 1u:
            glColorMask ( GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE );
        break;

        case 2u:
            glColorMask ( GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE );
        break;

        case 3u:
            glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
        break;

        case 4u:
            glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
        break;

        default:
            // NOTHING
        break;
    }

    glDepthMask ( GL_FALSE );
    glStencilMask ( 0x00u );

    static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    glDisable ( GL_BLEND );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );

    GLsizei resolvedFaceLength = static_cast<GLsizei> ( equirectangularTexture.GetWidth () / 4u );
    glViewport ( 0, 0, resolvedFaceLength, resolvedFaceLength );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogW ( L"GXTextureCubeMap::ProjectSide::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    GXMeshGeometry unitCube;
    unitCube.LoadMesh ( L"Meshes/System/Unit Cube.stm" );

    GXEquirectangularToCubeMapMaterial equirectangularToCubeMapMaterial;
    equirectangularToCubeMapMaterial.SetEquirectangularTexture ( equirectangularTexture );

    if ( isApplyGammaCorrection )
        equirectangularToCubeMapMaterial.EnableGammaCorrection ();
    else
        equirectangularToCubeMapMaterial.DisableGammaCorrection ();

    equirectangularToCubeMapMaterial.Bind ( GXTransform::GetNullTransform () );
    unitCube.Render ();
    equirectangularToCubeMapMaterial.Unbind ();

    glBindFramebuffer ( GL_FRAMEBUFFER, 0u );

    state.Restore ();
}

//---------------------------------------------------------------------------------------------------------------------

GXTextureCubeMap::GXTextureCubeMap ():
    textureCubeMapEntry ( nullptr ),
    textureUnit ( INVALID_TEXTURE_UNIT )
{
    // NOTHING
}

GXTextureCubeMap::GXTextureCubeMap ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap ):
    textureCubeMapEntry ( new GXTextureCubeMapEntry ( faceLength, internalFormat, isGenerateMipmap ) ),
    textureUnit ( INVALID_TEXTURE_UNIT )
{
    // NOTHING
}

GXTextureCubeMap::GXTextureCubeMap ( const GXWChar* equirectangularImage, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection ):
    textureCubeMapEntry ( nullptr ),
    textureUnit ( INVALID_TEXTURE_UNIT )
{
    LoadEquirectangularImage ( equirectangularImage, isGenerateMipmap, isApplyGammaCorrection );
}

GXTextureCubeMap::~GXTextureCubeMap ()
{
    if ( !textureCubeMapEntry ) return;

    textureCubeMapEntry->Release ();
}

GXUShort GXTextureCubeMap::GetFaceLength () const
{
    return textureCubeMapEntry->GetFaceLength ();
}

GXUByte GXTextureCubeMap::GetChannelNumber () const
{
    return textureCubeMapEntry->GetChannelNumber ();
}

GXUByte GXTextureCubeMap::GetLevelOfDetailNumber () const
{
    return textureCubeMapEntry->GetLevelOfDetailNumber ();
}

GXVoid GXTextureCubeMap::LoadEquirectangularImage ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection )
{
    if ( textureCubeMapEntry )
        textureCubeMapEntry->Release ();

    textureCubeMapEntry = GXTextureCubeMapEntry::Find ( fileName );

    if ( textureCubeMapEntry )
    {
        textureCubeMapEntry->AddReference ();
        return;
    }

    textureCubeMapEntry = new GXTextureCubeMapEntry ( fileName, isGenerateMipmap, isApplyGammaCorrection );
}

GXVoid GXTextureCubeMap::FillWholePixelData ( const GXVoid* data, GLenum target )
{
    if ( !textureCubeMapEntry->GetFileName () )
    {
        textureCubeMapEntry->FillWholePixelData ( data, target );
        return;
    }

    GXUShort faceLength = textureCubeMapEntry->GetFaceLength ();
    GLint internalFormat = textureCubeMapEntry->GetInternalFormat ();
    GXBool isGenerateMipmap = textureCubeMapEntry->IsGenerateMipmap ();

    textureCubeMapEntry->Release ();
    textureCubeMapEntry = new GXTextureCubeMapEntry ( faceLength, internalFormat, isGenerateMipmap );
    textureCubeMapEntry->FillWholePixelData ( data, target );
}

GXVoid GXTextureCubeMap::UpdateMipmaps ()
{
    textureCubeMapEntry->UpdateMipmaps ();
}

GXVoid GXTextureCubeMap::Bind ( GXUByte unit )
{
    textureUnit = unit;

    glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + textureUnit ) );
    glBindTexture ( GL_TEXTURE_CUBE_MAP, textureCubeMapEntry->GetTextureObject () );
}

GXVoid GXTextureCubeMap::Unbind ()
{
    glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + textureUnit ) );
    glBindTexture ( GL_TEXTURE_CUBE_MAP, 0u );
}

GLuint GXTextureCubeMap::GetTextureObject () const
{
    return textureCubeMapEntry->GetTextureObject ();
}

GXBool GXTextureCubeMap::IsInitialized () const
{
    return textureCubeMapEntry != nullptr;
}

GXVoid GXTextureCubeMap::InitResources ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap )
{
    if ( textureCubeMapEntry )
        textureCubeMapEntry->Release ();

    textureCubeMapEntry = new GXTextureCubeMapEntry ( faceLength, internalFormat, isGenerateMipmap );
}

GXVoid GXTextureCubeMap::FreeResources ()
{
    if ( !textureCubeMapEntry ) return;

    textureCubeMapEntry->Release ();
    textureCubeMapEntry = nullptr;
}

GXUInt GXTextureCubeMap::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
    return GXTextureCubeMapEntry::GetTotalLoadedTextures ( lastTexture );
}
