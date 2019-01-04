// version 1.8

#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXTexture2DGammaCorrectorMaterial.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXUIntAtomic.h>


#define INVALID_INTERNAL_FORMAT             0
#define INVALID_UNPACK_ALIGNMENT            0x7FFFFFFF
#define INVALID_FORMAT                      0
#define INVALID_TYPE                        0
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

struct GXTexture2DCacheHeader final
{
    GXUByte                 numChannels;
    eGXChannelDataType      channelDataType;
    GXUShort                width;
    GXUShort                height;
    GXUBigInt               pixelOffset;
};

#pragma pack ( pop )

//----------------------------------------------------------------------

class GXTexture2DEntry final : public GXMemoryInspector
{
    private:
        GXTexture2DEntry*               previous;
        GXTexture2DEntry*               next;

        GXUIntAtomic                    references;
        GXWChar*                        fileName;

        GXUShort                        width;
        GXUShort                        height;
        GXUByte                         numChannels;
        GXUByte                         lods;

        GLint                           internalFormat;
        GLint                           unpackAlignment;
        GLenum                          format;
        GLenum                          type;

        GLuint                          textureObject;

        GXBool                          isGenerateMipmap;

        static GXTexture2DEntry*        top;

    public:
        explicit GXTexture2DEntry ( const GXWChar* imageFileName, GXBool doesGenerateMipmap, GXBool isApplyGammaCorrection );
        explicit GXTexture2DEntry ( GXUShort imageWidth, GXUShort imageHeight, GLint imageInternalFormat, GXBool doesGenerateMipmap );

        const GXWChar* GetFileName () const;
        GXUShort GetWidth () const;
        GXUShort GetHeight () const;
        GLint GetInternalFormat () const;
        GXBool IsGenerateMipmap () const;
        GXUByte GetChannelNumber () const;
        GXUByte GetLevelOfDetailNumber () const;

        GLuint GetTextureObject () const;

        GXVoid FillWholePixelData ( const GXVoid* data );
        GXVoid FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort regionWidth, GXUShort regionHeight, const GXVoid* data );
        GXVoid UpdateMipmaps ();

        GXVoid AddReference ();
        GXVoid Release ();

        // Method returns valid pointer if texture was found.
        // Method returns nullptr if texture was not found.
        static GXTexture2DEntry* GXCALL Find ( const GXWChar* textureFile );

        static GXUInt GXCALL GetTotalLoadedTextures ( const GXWChar** lastTexture );

    private:
        ~GXTexture2DEntry () override;

        GXVoid InitResources ( GXUShort textureWidth, GXUShort textureHeight, GLint textureInternalFormat, GXBool isGenerateMipmapPolicy );

        GXTexture2DEntry () = delete;
        GXTexture2DEntry ( const GXTexture2DEntry &other ) = delete;
        GXTexture2DEntry& operator = ( const GXTexture2DEntry &other ) = delete;
};

GXTexture2DEntry* GXTexture2DEntry::top = nullptr;

GXTexture2DEntry::GXTexture2DEntry ( const GXWChar* imageFileName, GXBool doesGenerateMipmap, GXBool isApplyGammaCorrection )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2DEntry" )
    previous ( nullptr ),
    next ( top ),
    references ( 1u )
{
    GXWcsclone ( &fileName, imageFileName );
    isGenerateMipmap = doesGenerateMipmap;

    if ( top )
        top->previous = this;

    top = this;

    GXWChar* path = nullptr;
    GXGetFileDirectoryPath ( &path, fileName );
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

    GXWChar* cacheFileName = static_cast<GXWChar*> ( Malloc ( size ) );
    wsprintfW ( cacheFileName, L"%s/%s/%s.%s", path, CACHE_DIRECTORY_NAME, baseFileName, CACHE_FILE_EXTENSION );

    free ( baseFileName );

    GXUByte* data = nullptr;
    GLint resolvedInternalFormat = INVALID_INTERNAL_FORMAT;
    GLenum readPixelFormat = GL_INVALID_ENUM;
    GLenum readPixelType = GL_INVALID_ENUM;
    GLint packAlignment = 1;

    if ( GXLoadFile ( cacheFileName, reinterpret_cast<GXVoid**> ( &data ), size, GX_FALSE ) )
    {
        Free ( cacheFileName );
        free ( path );

        const GXTexture2DCacheHeader* cacheHeader = (const GXTexture2DCacheHeader*)data;

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

            case 2u:
            {
                switch ( cacheHeader->channelDataType )
                {
                    case eGXChannelDataType::UnsignedByte:
                        resolvedInternalFormat = GL_RG8;
                    break;

                    case eGXChannelDataType::Float:
                        resolvedInternalFormat = GL_RG16F;
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

        InitResources ( cacheHeader->width, cacheHeader->height, resolvedInternalFormat, isGenerateMipmap );
        FillWholePixelData ( data + cacheHeader->pixelOffset );

        free ( data );
        return;
    }

    size = GXWcslen ( path ) * sizeof ( GXWChar );
    size += sizeof ( GXWChar );        // L'/' symbol
    size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
    size += sizeof ( GXWChar );        // L'\0' symbol

    GXWChar* cacheDirectory = static_cast<GXWChar*> ( Malloc ( size ) );
    wsprintfW ( cacheDirectory, L"%s/%s", path, CACHE_DIRECTORY_NAME );

    free ( path );

    if ( !GXDoesDirectoryExist ( cacheDirectory ) )
        GXCreateDirectory ( cacheDirectory );

    Free ( cacheDirectory );

    GXTexture2DCacheHeader cacheHeader;
    GXUInt resolvedWidth = 0u;
    GXUInt resolvedHeight = 0u;

    GXWChar* extension = nullptr;
    GXGetFileExtension ( &extension, fileName );
    GXTexture2D textureToGammaCorrect;
    GXBool success = GX_FALSE;
    GXUByte* pixels = nullptr;
    GXUPointer pixelSize = 0u;
    GXWriteFileStream cacheFile ( cacheFileName );

    if ( GXWcscmp ( extension, L"hdr" ) == 0 || GXWcscmp ( extension, L"HDR" ) == 0 )
    {
        GXFloat* hdrPixels = nullptr;
        success = GXLoadHDRImage ( fileName, resolvedWidth, resolvedHeight, cacheHeader.numChannels, &hdrPixels );

        if ( success )
        {
            switch ( cacheHeader.numChannels )
            {
                case 1u:
                    resolvedInternalFormat = GL_R16F;
                    readPixelFormat = GL_RED;
                    readPixelType = GL_FLOAT;
                    packAlignment = 2;
                break;

                case 2u:
                    resolvedInternalFormat = GL_RG16F;
                    readPixelFormat = GL_RG;
                    readPixelType = GL_FLOAT;
                    packAlignment = 4;
                break;

                case 3u:
                    resolvedInternalFormat = GL_RGB16F;
                    readPixelFormat = GL_RGB;
                    readPixelType = GL_FLOAT;
                    packAlignment = 2;
                break;

                case 4u:
                    resolvedInternalFormat = GL_RGBA16F;
                    readPixelFormat = GL_RGBA;
                    readPixelType = GL_FLOAT;
                    packAlignment = 8;
                break;

                default:
                    // NOTHING
                break;
            }

            cacheHeader.channelDataType = eGXChannelDataType::Float;
            cacheHeader.width = static_cast<GXUShort> ( resolvedWidth );
            cacheHeader.height = static_cast<GXUShort> ( resolvedHeight );
            cacheHeader.pixelOffset = sizeof ( GXTexture2DCacheHeader );

            pixelSize = resolvedWidth * resolvedHeight * cacheHeader.numChannels * sizeof ( GXFloat );

            Free ( cacheFileName );

            cacheFile.Write ( &cacheHeader, sizeof ( GXTexture2DCacheHeader ) );
            InitResources ( cacheHeader.width, cacheHeader.height, resolvedInternalFormat, isGenerateMipmap );

            if ( isApplyGammaCorrection )
            {
                pixels = reinterpret_cast<GXUByte*> ( hdrPixels );

                textureToGammaCorrect.InitResources ( cacheHeader.width, cacheHeader.height, resolvedInternalFormat, GX_FALSE );
                textureToGammaCorrect.FillWholePixelData ( pixels );
            }
            else
            {
                FillWholePixelData ( hdrPixels );

                cacheFile.Write ( hdrPixels, pixelSize );
                cacheFile.Close ();

                free ( hdrPixels );
            }
        }
    }
    else
    {
        GXUByte* ldrPixels = nullptr;
        success = GXLoadLDRImage ( fileName, resolvedWidth, resolvedHeight, cacheHeader.numChannels, &ldrPixels );

        if ( success )
        {
            switch ( cacheHeader.numChannels )
            {
                case 1u:
                    resolvedInternalFormat = GL_R8;
                    readPixelFormat = GL_RED;
                    readPixelType = GL_UNSIGNED_BYTE;
                    packAlignment = 1;
                break;

                case 2u:
                    resolvedInternalFormat = GL_RG8;
                    readPixelFormat = GL_RG;
                    readPixelType = GL_UNSIGNED_BYTE;
                    packAlignment = 2;
                break;

                case 3u:
                    resolvedInternalFormat = GL_RGB8;
                    readPixelFormat = GL_RGB;
                    readPixelType = GL_UNSIGNED_BYTE;
                    packAlignment = 1;
                break;

                case 4u:
                    resolvedInternalFormat = GL_RGBA8;
                    readPixelFormat = GL_RGBA;
                    readPixelType = GL_UNSIGNED_BYTE;
                    packAlignment = 4;
                break;

                default:
                    // NOTHING
                break;
            }

            cacheHeader.channelDataType = eGXChannelDataType::UnsignedByte;
            cacheHeader.width = static_cast<GXUShort> ( resolvedWidth );
            cacheHeader.height = static_cast<GXUShort> ( resolvedHeight );
            cacheHeader.pixelOffset = sizeof ( GXTexture2DCacheHeader );

            pixelSize = resolvedWidth * resolvedHeight * cacheHeader.numChannels * sizeof ( GXUByte );

            Free ( cacheFileName );

            cacheFile.Write ( &cacheHeader, sizeof ( GXTexture2DCacheHeader ) );

            InitResources ( cacheHeader.width, cacheHeader.height, resolvedInternalFormat, isGenerateMipmap );

            if ( isApplyGammaCorrection )
            {
                pixels = ldrPixels;

                textureToGammaCorrect.InitResources ( cacheHeader.width, cacheHeader.height, resolvedInternalFormat, GX_FALSE );
                textureToGammaCorrect.FillWholePixelData ( pixels );
            }
            else
            {
                FillWholePixelData ( ldrPixels );

                cacheFile.Write ( ldrPixels, pixelSize );
                cacheFile.Close ();

                free ( ldrPixels );
            }
        }
    }

    GXSafeFree ( extension );

    if ( !success )
    {
        GXLogW ( L"GXTexture2D::LoadTexture::Error - Поддерживаются текстуры с количеством каналов 1, 3 и 4 (текущее количество %hhu)\n", cacheHeader.numChannels );
        Free ( cacheFileName );
        return;
    }

    if ( !isApplyGammaCorrection ) return;

    GXOpenGLState state;
    state.Save ();

    GLuint fbo;
    glGenFramebuffers ( 1, &fbo );
    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureObject, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    switch ( numChannels )
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

    glViewport ( 0, 0, static_cast<GLsizei> ( resolvedWidth ), static_cast<GLsizei> ( resolvedHeight ) );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogW ( L"GXTexture2D::LoadTexture::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    GXMeshGeometry screenQuad;
    screenQuad.LoadMesh ( L"Meshes/System/ScreenQuad.stm" );
    GXTexture2DGammaCorrectorMaterial gammaCorrectorMaterial;

    gammaCorrectorMaterial.SetSRGBTexture ( textureToGammaCorrect );
    gammaCorrectorMaterial.Bind ( GXTransform::GetNullTransform () );
    screenQuad.Render ();
    gammaCorrectorMaterial.Unbind ();

    glPixelStorei ( GL_PACK_ALIGNMENT, packAlignment );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader.width ), static_cast<GLsizei> ( cacheHeader.height ), readPixelFormat, readPixelType, pixels );

    cacheFile.Write ( pixels, pixelSize );
    cacheFile.Close ();

    free ( pixels );

    state.Restore ();
    glDeleteFramebuffers ( 1, &fbo );

    if ( !isGenerateMipmap ) return;

    UpdateMipmaps ();
}

GXTexture2DEntry::GXTexture2DEntry ( GXUShort imageWidth, GXUShort imageHeight, GLint imageInternalFormat, GXBool doesGenerateMipmap )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2DEntry" )
    previous ( nullptr ),
    next ( nullptr ),
    references ( 1u ),
    fileName ( nullptr )
{
    InitResources ( imageWidth, imageHeight, imageInternalFormat, doesGenerateMipmap );
}

const GXWChar* GXTexture2DEntry::GetFileName () const
{
    return fileName;
}

GXUShort GXTexture2DEntry::GetWidth () const
{
    return width;
}

GXUShort GXTexture2DEntry::GetHeight () const
{
    return height;
}

GLint GXTexture2DEntry::GetInternalFormat () const
{
    return internalFormat;
}

GXBool GXTexture2DEntry::IsGenerateMipmap () const
{
    return isGenerateMipmap;
}

GXUByte GXTexture2DEntry::GetChannelNumber () const
{
    return numChannels;
}

GXUByte GXTexture2DEntry::GetLevelOfDetailNumber () const
{
    return lods;
}

GLuint GXTexture2DEntry::GetTextureObject () const
{
    return textureObject;
}

GXVoid GXTexture2DEntry::FillWholePixelData ( const GXVoid* data )
{
    if ( textureObject == 0u ) return;

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, textureObject );

    glPixelStorei ( GL_UNPACK_ALIGNMENT, unpackAlignment );
    glTexImage2D ( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data );

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    if ( isGenerateMipmap )
        glGenerateMipmap ( GL_TEXTURE_2D );

    glBindTexture ( GL_TEXTURE_2D, 0u );

    GXCheckOpenGLError ();
}

GXVoid GXTexture2DEntry::FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort regionWidth, GXUShort regionHeight, const GXVoid* data )
{
    if ( textureObject == 0u ) return;

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, textureObject );

    glPixelStorei ( GL_UNPACK_ALIGNMENT, unpackAlignment );
    glTexSubImage2D ( GL_TEXTURE_2D, 0, static_cast<GLint> ( left ), static_cast<GLint> ( bottom ), static_cast<GLint> ( regionWidth ), static_cast<GLint> ( regionHeight ), format, type, data );
    GXCheckOpenGLError ();

    if ( isGenerateMipmap )
    {
        glGenerateMipmap ( GL_TEXTURE_2D );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

        GLfloat maxAnisotropy;
        glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
        glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );
    }

    glBindTexture ( GL_TEXTURE_2D, 0u );
}

GXVoid GXTexture2DEntry::UpdateMipmaps ()
{
    if ( textureObject == 0u || !isGenerateMipmap ) return;

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, textureObject );

    glGenerateMipmap ( GL_TEXTURE_2D );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    GLfloat maxAnisotropy;
    glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
    glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );

    glBindTexture ( GL_TEXTURE_2D, 0u );
}

GXVoid GXTexture2DEntry::AddReference ()
{
    ++references;
}

GXVoid GXTexture2DEntry::Release ()
{
    --references;

    if ( references > 0u ) return;

    delete this;
}

GXTexture2DEntry* GXCALL GXTexture2DEntry::Find ( const GXWChar* textureFile )
{
    for ( GXTexture2DEntry* p = top; p; p = p->next )
    {
        if ( GXWcscmp ( p->fileName, textureFile ) != 0 ) continue;
        
        return p;
    }

    return nullptr;
}

GXUInt GXCALL GXTexture2DEntry::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
    GXUInt total = 0u;

    for ( const GXTexture2DEntry* p = top; p; p = p->next )
        ++total;

    if ( total > 0u )
        *lastTexture = top->GetFileName ();
    else
        *lastTexture = nullptr;

    return total;
}

GXTexture2DEntry::~GXTexture2DEntry ()
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

GXVoid GXTexture2DEntry::InitResources ( GXUShort textureWidth, GXUShort textureHeight, GLint textureInternalFormat, GXBool isGenerateMipmapPolicy )
{
    width = textureWidth;
    height = textureHeight;
    internalFormat = textureInternalFormat;
    isGenerateMipmap = isGenerateMipmapPolicy;

    glActiveTexture ( GL_TEXTURE0 );
    glGenTextures ( 1, &textureObject );
    glBindTexture ( GL_TEXTURE_2D, textureObject );

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

        case GL_R32F:
            unpackAlignment = 4;
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
            unpackAlignment = 8;
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

    if ( isGenerateMipmap )
    {
        GXUShort maxSide = width >= height ? width : height;

        lods = 0u;
        GXUShort currentResolution = 1u;

        while ( currentResolution <= maxSide )
        {
            ++lods;
            currentResolution *= 2;
        }
    }
    else
    {
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
        lods = 1u;
    }

    glBindTexture ( GL_TEXTURE_2D, 0u );
    FillWholePixelData ( nullptr );
}

//----------------------------------------------------------------------

GXTexture2D::GXTexture2D ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2D" )
    textureUnit ( INVALID_TEXTURE_UNIT ),
    texture2DEntry ( nullptr )
{
    // NOTHING
}

GXTexture2D::GXTexture2D ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2D" )
    textureUnit ( INVALID_TEXTURE_UNIT )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    texture2DEntry = new GXTexture2DEntry ( width, height, internalFormat, isGenerateMipmap );
    // NOTHING
}

GXTexture2D::GXTexture2D ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2D" )
    textureUnit ( INVALID_TEXTURE_UNIT ),
    texture2DEntry ( nullptr )
{
    LoadImage ( fileName, isGenerateMipmap, isApplyGammaCorrection );
}

GXTexture2D::~GXTexture2D ()
{
    if ( !texture2DEntry ) return;

    texture2DEntry->Release ();
}

GXUShort GXTexture2D::GetWidth () const
{
    return texture2DEntry->GetWidth ();
}

GXUShort GXTexture2D::GetHeight () const
{
    return texture2DEntry->GetHeight ();
}

GXUByte GXTexture2D::GetChannelNumber () const
{
    return texture2DEntry->GetChannelNumber ();
}

GXUByte GXTexture2D::GetLevelOfDetailNumber () const
{
    return texture2DEntry->GetLevelOfDetailNumber ();
}

GXVoid GXTexture2D::LoadImage ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection )
{
    if ( texture2DEntry )
        texture2DEntry->Release ();

    texture2DEntry = GXTexture2DEntry::Find ( fileName );

    if ( texture2DEntry )
    {
        texture2DEntry->AddReference ();
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    texture2DEntry = new GXTexture2DEntry ( fileName, isGenerateMipmap, isApplyGammaCorrection );
}

GXVoid GXTexture2D::FillWholePixelData ( const GXVoid* data )
{
    if ( !texture2DEntry->GetFileName () )
    {
        texture2DEntry->FillWholePixelData ( data );
        return;
    }

    GXUShort width = texture2DEntry->GetWidth ();
    GXUShort height = texture2DEntry->GetHeight ();
    GLint internalFormat = texture2DEntry->GetInternalFormat ();
    GXBool isGenerateMipmap = texture2DEntry->IsGenerateMipmap ();

    texture2DEntry->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    texture2DEntry = new GXTexture2DEntry ( width, height, internalFormat, isGenerateMipmap );
    texture2DEntry->FillWholePixelData ( data );
}

GXVoid GXTexture2D::FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort regionWidth, GXUShort regionHeight, const GXVoid* data )
{
    if ( !texture2DEntry->GetFileName () )
    {
        texture2DEntry->FillRegionPixelData ( left, bottom, regionWidth, regionHeight, data );
        return;
    }

    GXUShort width = texture2DEntry->GetWidth ();
    GXUShort height = texture2DEntry->GetHeight ();
    GLint internalFormat = texture2DEntry->GetInternalFormat ();
    GXBool isGenerateMipmap = texture2DEntry->IsGenerateMipmap ();

    texture2DEntry->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    texture2DEntry = new GXTexture2DEntry ( width, height, internalFormat, isGenerateMipmap );
    texture2DEntry->FillWholePixelData ( nullptr );
    texture2DEntry->FillRegionPixelData ( left, bottom, regionWidth, regionHeight, data );
}

GXVoid GXTexture2D::UpdateMipmaps ()
{
    texture2DEntry->UpdateMipmaps ();
}

GXVoid GXTexture2D::Bind ( GXUByte unit )
{
    textureUnit = unit;

    glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + unit ) );
    glBindTexture ( GL_TEXTURE_2D, texture2DEntry->GetTextureObject () );
}

GXVoid GXTexture2D::Unbind ()
{
    glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + textureUnit ) );
    glBindTexture ( GL_TEXTURE_2D, 0u );
}

GLuint GXTexture2D::GetTextureObject () const
{
    return texture2DEntry->GetTextureObject ();
}

GXBool GXTexture2D::IsInitialized () const
{
    return texture2DEntry != nullptr;
}

GXVoid GXTexture2D::InitResources ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap )
{
    if ( texture2DEntry )
        texture2DEntry->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    texture2DEntry = new GXTexture2DEntry ( width, height, internalFormat, isGenerateMipmap );
}

GXVoid GXTexture2D::FreeResources ()
{
    if ( !texture2DEntry ) return;

    texture2DEntry->Release ();
    texture2DEntry = nullptr;
}

GXUInt GXCALL GXTexture2D::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
    return GXTexture2DEntry::GetTotalLoadedTextures ( lastTexture );
}
