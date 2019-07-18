// version 1.10

#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXTexture2DGammaCorrectorMaterial.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXUPointerAtomic.h>


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
    GXUByte                 _numChannels;
    eGXChannelDataType      _channelDataType;
    GXUShort                _width;
    GXUShort                _height;
    GXUBigInt               _pixelOffset;
};

#pragma pack ( pop )

//----------------------------------------------------------------------

class GXTexture2DEntry final : public GXMemoryInspector
{
    private:
        GXTexture2DEntry*               _previous;
        GXTexture2DEntry*               _next;

        GXUPointerAtomic                _references;
        const GXString                  _fileName;

        GXUShort                        _width;
        GXUShort                        _height;
        GXUByte                         _numChannels;
        GXUByte                         _lods;

        GLint                           _internalFormat;
        GLint                           _unpackAlignment;
        GLenum                          _format;
        GLenum                          _type;

        GLuint                          _textureObject;

        GXBool                          _isGenerateMipmap;

        static GXTexture2DEntry*        _top;

    public:
        explicit GXTexture2DEntry ( const GXWChar* imageFileName, GXBool doesGenerateMipmap, GXBool isApplyGammaCorrection );
        explicit GXTexture2DEntry ( GXUShort width, GXUShort height, GLint internalFormat, GXBool doesGenerateMipmap );

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

GXTexture2DEntry* GXTexture2DEntry::_top = nullptr;

GXTexture2DEntry::GXTexture2DEntry ( const GXWChar* imageFileName, GXBool doesGenerateMipmap, GXBool isApplyGammaCorrection )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2DEntry" )
    _previous ( nullptr ),
    _next ( _top ),
    _references ( 1u ),
    _fileName ( imageFileName )
{
    _isGenerateMipmap = doesGenerateMipmap;

    if ( _top )
        _top->_previous = this;

    _top = this;

    GXWChar* path = nullptr;
    GXGetFileDirectoryPath ( &path, _fileName );

    GXWChar* baseFileName = nullptr;
    GXGetBaseFileName ( &baseFileName, _fileName );

    GXString cacheFileName;
    cacheFileName.Format ( "%S/%S/%S.%S", path, CACHE_DIRECTORY_NAME, baseFileName, CACHE_FILE_EXTENSION );

    free ( baseFileName );

    GXUByte* data = nullptr;
    GXUPointer size;
    GLint resolvedInternalFormat = INVALID_INTERNAL_FORMAT;
    GLenum readPixelFormat = GL_INVALID_ENUM;
    GLenum readPixelType = GL_INVALID_ENUM;
    GLint packAlignment = 1;

    GXFile file ( cacheFileName );

    if ( file.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_FALSE ) )
    {
        free ( path );

        const GXTexture2DCacheHeader* cacheHeader = reinterpret_cast<const GXTexture2DCacheHeader*> ( data );

        switch ( cacheHeader->_numChannels )
        {
            case 1u:
            {
                switch ( cacheHeader->_channelDataType )
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
                switch ( cacheHeader->_channelDataType )
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
                switch ( cacheHeader->_channelDataType )
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
                switch ( cacheHeader->_channelDataType )
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

        InitResources ( cacheHeader->_width, cacheHeader->_height, resolvedInternalFormat, _isGenerateMipmap );
        FillWholePixelData ( data + cacheHeader->_pixelOffset );

        file.Close ();
        return;
    }

    GXString cacheDirectory;
    cacheDirectory.Format ( "%S/%S", path, CACHE_DIRECTORY_NAME );

    free ( path );

    if ( !GXDoesDirectoryExist ( cacheDirectory ) )
        GXCreateDirectory ( cacheDirectory );

    GXTexture2DCacheHeader cacheHeader;
    GXUInt resolvedWidth = 0u;
    GXUInt resolvedHeight = 0u;

    GXWChar* extension = nullptr;
    GXGetFileExtension ( &extension, _fileName );
    GXTexture2D textureToGammaCorrect;
    GXBool success = GX_FALSE;
    GXUByte* pixels = nullptr;
    GXUPointer pixelSize = 0u;
    GXWriteFileStream cacheFile ( cacheFileName );

    if ( GXWcscmp ( extension, L"hdr" ) == 0 || GXWcscmp ( extension, L"HDR" ) == 0 )
    {
        GXFloat* hdrPixels = nullptr;
        success = GXLoadHDRImage ( _fileName, resolvedWidth, resolvedHeight, cacheHeader._numChannels, &hdrPixels );

        if ( success )
        {
            switch ( cacheHeader._numChannels )
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

            cacheHeader._channelDataType = eGXChannelDataType::Float;
            cacheHeader._width = static_cast<GXUShort> ( resolvedWidth );
            cacheHeader._height = static_cast<GXUShort> ( resolvedHeight );
            cacheHeader._pixelOffset = sizeof ( GXTexture2DCacheHeader );

            pixelSize = resolvedWidth * resolvedHeight * cacheHeader._numChannels * sizeof ( GXFloat );

            cacheFile.Write ( &cacheHeader, sizeof ( GXTexture2DCacheHeader ) );
            InitResources ( cacheHeader._width, cacheHeader._height, resolvedInternalFormat, _isGenerateMipmap );

            if ( isApplyGammaCorrection )
            {
                pixels = reinterpret_cast<GXUByte*> ( hdrPixels );

                textureToGammaCorrect.InitResources ( cacheHeader._width, cacheHeader._height, resolvedInternalFormat, GX_FALSE );
                textureToGammaCorrect.FillWholePixelData ( pixels );
            }
            else
            {
                FillWholePixelData ( hdrPixels );

                cacheFile.Write ( hdrPixels, pixelSize );
                cacheFile.Close ();

                Free ( hdrPixels );
            }
        }
    }
    else
    {
        GXUByte* ldrPixels = nullptr;
        success = GXLoadLDRImage ( _fileName, resolvedWidth, resolvedHeight, cacheHeader._numChannels, &ldrPixels );

        if ( success )
        {
            switch ( cacheHeader._numChannels )
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

            cacheHeader._channelDataType = eGXChannelDataType::UnsignedByte;
            cacheHeader._width = static_cast<GXUShort> ( resolvedWidth );
            cacheHeader._height = static_cast<GXUShort> ( resolvedHeight );
            cacheHeader._pixelOffset = sizeof ( GXTexture2DCacheHeader );

            pixelSize = resolvedWidth * resolvedHeight * cacheHeader._numChannels * sizeof ( GXUByte );
            cacheFile.Write ( &cacheHeader, sizeof ( GXTexture2DCacheHeader ) );
            InitResources ( cacheHeader._width, cacheHeader._height, resolvedInternalFormat, _isGenerateMipmap );

            if ( isApplyGammaCorrection )
            {
                pixels = ldrPixels;

                textureToGammaCorrect.InitResources ( cacheHeader._width, cacheHeader._height, resolvedInternalFormat, GX_FALSE );
                textureToGammaCorrect.FillWholePixelData ( pixels );
            }
            else
            {
                FillWholePixelData ( ldrPixels );

                cacheFile.Write ( ldrPixels, pixelSize );
                cacheFile.Close ();

                Free ( ldrPixels );
            }
        }
    }

    GXSafeFree ( extension );

    if ( !success )
    {
        GXLogA ( "GXTexture2D::LoadTexture::Error - Поддерживаются текстуры с количеством каналов 1, 3 и 4 (текущее количество %hhu)\n", cacheHeader._numChannels );
        return;
    }

    if ( !isApplyGammaCorrection ) return;

    GXOpenGLState state;
    state.Save ();

    GLuint fbo;
    glGenFramebuffers ( 1, &fbo );
    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textureObject, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    switch ( _numChannels )
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
        GXLogA ( "GXTexture2D::LoadTexture::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    GXMeshGeometry screenQuad;
    screenQuad.LoadMesh ( L"Meshes/System/ScreenQuad.stm" );
    GXTexture2DGammaCorrectorMaterial gammaCorrectorMaterial;

    gammaCorrectorMaterial.SetSRGBTexture ( textureToGammaCorrect );
    gammaCorrectorMaterial.Bind ( GXTransform::GetNullTransform () );
    screenQuad.Render ();
    gammaCorrectorMaterial.Unbind ();

    glPixelStorei ( GL_PACK_ALIGNMENT, packAlignment );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader._width ), static_cast<GLsizei> ( cacheHeader._height ), readPixelFormat, readPixelType, pixels );

    cacheFile.Write ( pixels, pixelSize );
    cacheFile.Close ();

    Free ( pixels );

    state.Restore ();
    glDeleteFramebuffers ( 1, &fbo );

    if ( !_isGenerateMipmap ) return;

    UpdateMipmaps ();
}

GXTexture2DEntry::GXTexture2DEntry ( GXUShort width, GXUShort height, GLint internalFormat, GXBool doesGenerateMipmap )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2DEntry" )
    _previous ( nullptr ),
    _next ( nullptr ),
    _references ( 1u )
{
    InitResources ( width, height, internalFormat, doesGenerateMipmap );
}

const GXWChar* GXTexture2DEntry::GetFileName () const
{
    return _fileName;
}

GXUShort GXTexture2DEntry::GetWidth () const
{
    return _width;
}

GXUShort GXTexture2DEntry::GetHeight () const
{
    return _height;
}

GLint GXTexture2DEntry::GetInternalFormat () const
{
    return _internalFormat;
}

GXBool GXTexture2DEntry::IsGenerateMipmap () const
{
    return _isGenerateMipmap;
}

GXUByte GXTexture2DEntry::GetChannelNumber () const
{
    return _numChannels;
}

GXUByte GXTexture2DEntry::GetLevelOfDetailNumber () const
{
    return _lods;
}

GLuint GXTexture2DEntry::GetTextureObject () const
{
    return _textureObject;
}

GXVoid GXTexture2DEntry::FillWholePixelData ( const GXVoid* data )
{
    if ( _textureObject == 0u ) return;

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, _textureObject );

    glPixelStorei ( GL_UNPACK_ALIGNMENT, _unpackAlignment );
    glTexImage2D ( GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _format, _type, data );

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    if ( _isGenerateMipmap )
        glGenerateMipmap ( GL_TEXTURE_2D );

    glBindTexture ( GL_TEXTURE_2D, 0u );

    GXCheckOpenGLError ();
}

GXVoid GXTexture2DEntry::FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort regionWidth, GXUShort regionHeight, const GXVoid* data )
{
    if ( _textureObject == 0u ) return;

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, _textureObject );

    glPixelStorei ( GL_UNPACK_ALIGNMENT, _unpackAlignment );
    glTexSubImage2D ( GL_TEXTURE_2D, 0, static_cast<GLint> ( left ), static_cast<GLint> ( bottom ), static_cast<GLint> ( regionWidth ), static_cast<GLint> ( regionHeight ), _format, _type, data );
    GXCheckOpenGLError ();

    if ( _isGenerateMipmap )
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
    if ( _textureObject == 0u || !_isGenerateMipmap ) return;

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, _textureObject );

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
    ++_references;
}

GXVoid GXTexture2DEntry::Release ()
{
    --_references;

    if ( _references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXTexture2DEntry* GXCALL GXTexture2DEntry::Find ( const GXWChar* textureFile )
{
    for ( GXTexture2DEntry* p = _top; p; p = p->_next )
    {
        if ( GXWcscmp ( p->_fileName, textureFile ) != 0 ) continue;
        
        return p;
    }

    return nullptr;
}

GXUInt GXCALL GXTexture2DEntry::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
    GXUInt total = 0u;

    for ( const GXTexture2DEntry* p = _top; p; p = p->_next )
        ++total;

    if ( total > 0u )
        *lastTexture = _top->GetFileName ();
    else
        *lastTexture = nullptr;

    return total;
}

GXTexture2DEntry::~GXTexture2DEntry ()
{
    if ( _top == this )
        _top = _top->_next;

    if ( _previous )
        _previous->_next = _next;

    if ( _next )
        _next->_previous = _previous;

    if ( _textureObject == 0u ) return;

    glDeleteTextures ( 1, &_textureObject );
}

GXVoid GXTexture2DEntry::InitResources ( GXUShort textureWidth, GXUShort textureHeight, GLint textureInternalFormat, GXBool isGenerateMipmapPolicy )
{
    _width = textureWidth;
    _height = textureHeight;
    _internalFormat = textureInternalFormat;
    _isGenerateMipmap = isGenerateMipmapPolicy;

    glActiveTexture ( GL_TEXTURE0 );
    glGenTextures ( 1, &_textureObject );
    glBindTexture ( GL_TEXTURE_2D, _textureObject );

    switch ( _internalFormat )
    {
        case GL_R32I:
        case GL_R32UI:
        {
            _unpackAlignment = 4;
            _format = GL_RED;
            _type = GL_UNSIGNED_INT;
            _numChannels = 1u;
        }
        break;

        case GL_R8:
        case GL_R8I:
        case GL_R8UI:
        {
            _unpackAlignment = 1;
            _format = GL_RED;
            _type = GL_UNSIGNED_BYTE;
            _numChannels = 1u;
        }
        break;

        case GL_R16F:
            _unpackAlignment = 2;
            _format = GL_RED;
            _type = GL_FLOAT;
            _numChannels = 1u;
        break;

        case GL_R32F:
            _unpackAlignment = 4;
            _format = GL_RED;
            _type = GL_FLOAT;
            _numChannels = 1u;
        break;

        case GL_RG8:
        case GL_RG8I:
        case GL_RG8UI:
        {
            _unpackAlignment = 2;
            _format = GL_RG;
            _type = GL_UNSIGNED_BYTE;
            _numChannels = 2u;
        }
        break;

        case GL_RG16F:
            _unpackAlignment = 4;
            _format = GL_RG;
            _type = GL_FLOAT;
            _numChannels = 2u;
        break;

        case GL_RGB8:
        case GL_RGB8I:
        case GL_RGB8UI:
        {
            _unpackAlignment = 1;
            _format = GL_RGB;
            _type = GL_UNSIGNED_BYTE;
            _numChannels = 3u;
        }
        break;

        case GL_RGB16:
        {
            _unpackAlignment = 2;
            _format = GL_RGB;
            _type = GL_UNSIGNED_SHORT;
            _numChannels = 3u;
        }
        break;

        case GL_RGB16F:
        {
            _unpackAlignment = 2;
            _format = GL_RGB;
            _type = GL_FLOAT;
            _numChannels = 3u;
        }
        break;

        case GL_RGBA8:
        case GL_RGBA8I:
        case GL_RGBA8UI:
        {
            _unpackAlignment = 4;
            _format = GL_RGBA;
            _type = GL_UNSIGNED_BYTE;
            _numChannels = 4u;
        }
        break;

        case GL_RGBA16F:
        {
            _unpackAlignment = 8;
            _format = GL_RGBA;
            _type = GL_FLOAT;
            _numChannels = 4u;
        }
        break;

        case GL_DEPTH24_STENCIL8:
        {
            _unpackAlignment = 4;
            _format = GL_DEPTH_STENCIL;
            _type = GL_UNSIGNED_INT_24_8;
            _numChannels = 1u;
        }
        break;

        default:
            // NOTHING
        break;
    }

    if ( _isGenerateMipmap )
    {
        GXUShort maxSide = _width >= _height ? _width : _height;

        _lods = 0u;
        GXUShort currentResolution = 1u;

        while ( currentResolution <= maxSide )
        {
            ++_lods;
            currentResolution *= 2;
        }
    }
    else
    {
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
        _lods = 1u;
    }

    glBindTexture ( GL_TEXTURE_2D, 0u );
    FillWholePixelData ( nullptr );
}

//----------------------------------------------------------------------

GXTexture2D::GXTexture2D ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2D" )
    _textureUnit ( INVALID_TEXTURE_UNIT ),
    _texture2DEntry ( nullptr )
{
    // NOTHING
}

GXTexture2D::GXTexture2D ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2D" )
    _textureUnit ( INVALID_TEXTURE_UNIT )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    _texture2DEntry = new GXTexture2DEntry ( width, height, internalFormat, isGenerateMipmap );
    // NOTHING
}

GXTexture2D::GXTexture2D ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTexture2D" )
    _textureUnit ( INVALID_TEXTURE_UNIT ),
    _texture2DEntry ( nullptr )
{
    LoadImage ( fileName, isGenerateMipmap, isApplyGammaCorrection );
}

GXTexture2D::~GXTexture2D ()
{
    if ( !_texture2DEntry ) return;

    _texture2DEntry->Release ();
}

GXUShort GXTexture2D::GetWidth () const
{
    return _texture2DEntry->GetWidth ();
}

GXUShort GXTexture2D::GetHeight () const
{
    return _texture2DEntry->GetHeight ();
}

GXUByte GXTexture2D::GetChannelNumber () const
{
    return _texture2DEntry->GetChannelNumber ();
}

GXUByte GXTexture2D::GetLevelOfDetailNumber () const
{
    return _texture2DEntry->GetLevelOfDetailNumber ();
}

GXVoid GXTexture2D::LoadImage ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection )
{
    if ( _texture2DEntry )
        _texture2DEntry->Release ();

    _texture2DEntry = GXTexture2DEntry::Find ( fileName );

    if ( _texture2DEntry )
    {
        _texture2DEntry->AddReference ();
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    _texture2DEntry = new GXTexture2DEntry ( fileName, isGenerateMipmap, isApplyGammaCorrection );
}

GXVoid GXTexture2D::FillWholePixelData ( const GXVoid* data )
{
    if ( !_texture2DEntry->GetFileName () )
    {
        _texture2DEntry->FillWholePixelData ( data );
        return;
    }

    GXUShort width = _texture2DEntry->GetWidth ();
    GXUShort height = _texture2DEntry->GetHeight ();
    GLint internalFormat = _texture2DEntry->GetInternalFormat ();
    GXBool isGenerateMipmap = _texture2DEntry->IsGenerateMipmap ();

    _texture2DEntry->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    _texture2DEntry = new GXTexture2DEntry ( width, height, internalFormat, isGenerateMipmap );
    _texture2DEntry->FillWholePixelData ( data );
}

GXVoid GXTexture2D::FillRegionPixelData ( GXUShort left, GXUShort bottom, GXUShort regionWidth, GXUShort regionHeight, const GXVoid* data )
{
    if ( !_texture2DEntry->GetFileName () )
    {
        _texture2DEntry->FillRegionPixelData ( left, bottom, regionWidth, regionHeight, data );
        return;
    }

    GXUShort width = _texture2DEntry->GetWidth ();
    GXUShort height = _texture2DEntry->GetHeight ();
    GLint internalFormat = _texture2DEntry->GetInternalFormat ();
    GXBool isGenerateMipmap = _texture2DEntry->IsGenerateMipmap ();

    _texture2DEntry->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    _texture2DEntry = new GXTexture2DEntry ( width, height, internalFormat, isGenerateMipmap );
    _texture2DEntry->FillWholePixelData ( nullptr );
    _texture2DEntry->FillRegionPixelData ( left, bottom, regionWidth, regionHeight, data );
}

GXVoid GXTexture2D::UpdateMipmaps ()
{
    _texture2DEntry->UpdateMipmaps ();
}

GXVoid GXTexture2D::Bind ( GXUByte unit )
{
    _textureUnit = unit;

    glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + unit ) );
    glBindTexture ( GL_TEXTURE_2D, _texture2DEntry->GetTextureObject () );
}

GXVoid GXTexture2D::Unbind ()
{
    glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + _textureUnit ) );
    glBindTexture ( GL_TEXTURE_2D, 0u );
}

GLuint GXTexture2D::GetTextureObject () const
{
    return _texture2DEntry->GetTextureObject ();
}

GXBool GXTexture2D::IsInitialized () const
{
    return _texture2DEntry != nullptr;
}

GXVoid GXTexture2D::InitResources ( GXUShort width, GXUShort height, GLint internalFormat, GXBool isGenerateMipmap )
{
    if ( _texture2DEntry )
        _texture2DEntry->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTexture2DEntry" );
    _texture2DEntry = new GXTexture2DEntry ( width, height, internalFormat, isGenerateMipmap );
}

GXVoid GXTexture2D::FreeResources ()
{
    if ( !_texture2DEntry ) return;

    _texture2DEntry->Release ();
    _texture2DEntry = nullptr;
}

GXUInt GXCALL GXTexture2D::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
    return GXTexture2DEntry::GetTotalLoadedTextures ( lastTexture );
}
