// version 1.9

#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXOpenGL.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXUPointerAtomic.h>
#include <GXCommon/GXWriteFileStream.h>


#define INVALID_INTERNAL_FORMAT             0
#define INVALID_UNPACK_ALIGNMENT            0x7FFFFFFF
#define INVALID_TEXTURE_UNIT                0xFFu
#define INVALID_CHANNEL_NUMBER              0u
#define INVALID_LEVEL_OF_DETAIL_NUMBER      0u

#define CACHE_DIRECTORY_NAME                "Cache"
#define CACHE_FILE_EXTENSION                "cache"

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
    GXUByte                 _numChannels;
    eGXChannelDataType      _channelDataType;

    GXUShort                _faceLength;

    GXUBigInt               _positiveXPixelOffset;
    GXUBigInt               _negativeXPixelOffset;
    GXUBigInt               _positiveYPixelOffset;
    GXUBigInt               _negativeYPixelOffset;
    GXUBigInt               _positiveZPixelOffset;
    GXUBigInt               _negativeZPixelOffset;
};

#pragma pack ( pop )

//---------------------------------------------------------------------------------------------------------------------

class GXTextureCubeMapEntry final : public GXMemoryInspector
{
    private:
        GXTextureCubeMapEntry*              _previous;
        GXTextureCubeMapEntry*              _next;
        GXUPointerAtomic                    _references;

        const GXString                      _fileName;

        GXUShort                            _faceLength;
        GXUByte                             _numChannels;
        GXUByte                             _lods;

        GLint                               _internalFormat;
        GLint                               _unpackAlignment;
        GLenum                              _format;
        GLenum                              _type;

        GLuint                              _textureObject;
        GXBool                              _isGenerateMipmap;

        static GXTextureCubeMapEntry*       _top;

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

        GXVoid InitResources ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmapPolicy );
        GXVoid ProjectFaces ( GLuint fbo, GLuint faceTextureObject, GXTexture2D &equirectangularTexture, GXBool isApplyGammaCorrection );

        GXTextureCubeMapEntry () = delete;
        GXTextureCubeMapEntry ( const GXTextureCubeMapEntry &other ) = delete;
        GXTextureCubeMapEntry& operator = ( const GXTextureCubeMapEntry &other ) = delete;
};

GXTextureCubeMapEntry* GXTextureCubeMapEntry::_top = nullptr;

GXTextureCubeMapEntry::GXTextureCubeMapEntry ( const GXWChar* equirectangularTextureFileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTextureCubeMapEntry" )
    _previous ( nullptr ),
    _next ( _top ),
    _references ( 1u ),
    _fileName ( equirectangularTextureFileName )
{
    if ( _top )
        _top->_previous = this;

    _top = this;

    const GXString path = GXGetFileDirectoryPath ( equirectangularTextureFileName );
    const GXString baseFileName = GXGetBaseFileName ( _fileName );

    GXString cacheFileName;
    cacheFileName.Format ( "%s/%s/%s.%s", static_cast<const GXMBChar*> ( path ), CACHE_DIRECTORY_NAME, static_cast<const GXMBChar*> ( baseFileName ), CACHE_FILE_EXTENSION );

    GXUByte* data = nullptr;
    GXUPointer size;
    GXFile file ( cacheFileName );

    if ( file.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_FALSE ) )
    {
        const GXTextureCubeMapCacheHeader* cacheHeader = reinterpret_cast<const GXTextureCubeMapCacheHeader*> ( data );
        GLint resolvedInternalFormat = INVALID_INTERNAL_FORMAT;

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

        InitResources ( cacheHeader->_faceLength, resolvedInternalFormat, isGenerateMipmap );

        FillWholePixelData ( data + cacheHeader->_positiveXPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
        FillWholePixelData ( data + cacheHeader->_negativeXPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
        FillWholePixelData ( data + cacheHeader->_positiveYPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
        FillWholePixelData ( data + cacheHeader->_negativeYPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
        FillWholePixelData ( data + cacheHeader->_positiveZPixelOffset, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
        FillWholePixelData ( data + cacheHeader->_negativeZPixelOffset, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );

        file.Close ();

        if ( isGenerateMipmap )
            UpdateMipmaps ();

        return;
    }

    GXTextureCubeMapCacheHeader cacheHeader;
    GXUInt width = 0u;
    GXUInt height = 0u;

    const GXString extension = GXGetFileExtension ( _fileName );
    GXFloat* hdrPixels = nullptr;
    GXUByte* ldrPixels = nullptr;
    GXTexture2D equirectangularTexture;
    GXBool success = GX_FALSE;
    GXUPointer faceSize = 0;

    if ( extension == "hdr" || extension == "HDR" )
    {
        success = GXLoadHDRImage ( _fileName, width, height, cacheHeader._numChannels, &hdrPixels );
        cacheHeader._channelDataType = eGXChannelDataType::Float;

        switch ( cacheHeader._numChannels )
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
        Free ( hdrPixels );
        GXUInt resolvedFaceLength = width / 4u;
        faceSize = resolvedFaceLength * resolvedFaceLength * cacheHeader._numChannels * sizeof ( GXFloat );
    }
    else
    {
        success = GXLoadLDRImage ( _fileName, width, height, cacheHeader._numChannels, &ldrPixels );
        cacheHeader._channelDataType = eGXChannelDataType::UnsignedByte;

        switch ( cacheHeader._numChannels )
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
        Free ( ldrPixels );
        GXUInt resolvedFaceLength = width / 4u;
        faceSize = resolvedFaceLength * resolvedFaceLength * cacheHeader._numChannels * sizeof ( GXUByte );
    }

    if ( !success )
    {
        GXLogA ( "GXTextureCubeMap::LoadEquirectangularTexture::Error - Поддерживаются текстуры с количеством каналов 1, 3 и 4 (текущее количество %hhu)\n", cacheHeader._numChannels );
        return;
    }

    cacheHeader._faceLength = static_cast<GXUShort> ( width / 4u );

    GXString cacheDirectory;
    cacheDirectory.Format ( "%s/%s", static_cast<const GXMBChar*> ( path ), CACHE_DIRECTORY_NAME );

    if ( !GXDoesDirectoryExist ( cacheDirectory ) )
        GXCreateDirectory ( cacheDirectory );

    cacheHeader._positiveXPixelOffset = sizeof ( GXTextureCubeMapCacheHeader );
    cacheHeader._negativeXPixelOffset = static_cast<GXUBigInt> ( cacheHeader._positiveXPixelOffset + faceSize );
    cacheHeader._positiveYPixelOffset = static_cast<GXUBigInt> ( cacheHeader._negativeXPixelOffset + faceSize );
    cacheHeader._negativeYPixelOffset = static_cast<GXUBigInt> ( cacheHeader._positiveYPixelOffset + faceSize );
    cacheHeader._positiveZPixelOffset = static_cast<GXUBigInt> ( cacheHeader._negativeYPixelOffset + faceSize );
    cacheHeader._negativeZPixelOffset = static_cast<GXUBigInt> ( cacheHeader._positiveZPixelOffset + faceSize );

    GLint resolvedInternalFormat = INVALID_INTERNAL_FORMAT;
    GLenum readPixelFormat = GL_INVALID_ENUM;
    GLenum readPixelType = GL_INVALID_ENUM;
    GLint packAlignment = 1;

    switch ( cacheHeader._numChannels )
    {
        case 1u:
        {
            switch ( cacheHeader._channelDataType )
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
            switch ( cacheHeader._channelDataType )
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
            switch ( cacheHeader._channelDataType )
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

    InitResources ( cacheHeader._faceLength, resolvedInternalFormat, isGenerateMipmap );

    GLuint fbo;
    glGenFramebuffers ( 1, &fbo );

    glPixelStorei ( GL_PACK_ALIGNMENT, packAlignment );

    GXWriteFileStream cacheFile ( cacheFileName );
    cacheFile.Write ( &cacheHeader, sizeof ( GXTextureCubeMapCacheHeader ) );

    GXUByte* facePixels = static_cast<GXUByte*> ( Malloc ( faceSize ) );

    ProjectFaces ( fbo, _textureObject, equirectangularTexture, isApplyGammaCorrection );

    GXOpenGLState state;
    state.Save ();

    glBindFramebuffer ( GL_READ_FRAMEBUFFER, fbo );

    switch ( cacheHeader._numChannels )
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

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, _textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader._faceLength ), static_cast<GLsizei> ( cacheHeader._faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, _textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader._faceLength ), static_cast<GLsizei> ( cacheHeader._faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, _textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader._faceLength ), static_cast<GLsizei> ( cacheHeader._faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, _textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader._faceLength ), static_cast<GLsizei> ( cacheHeader._faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, _textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader._faceLength ), static_cast<GLsizei> ( cacheHeader._faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    glFramebufferTexture2D ( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, _textureObject, 0 );
    glReadPixels ( 0, 0, static_cast<GLsizei> ( cacheHeader._faceLength ), static_cast<GLsizei> ( cacheHeader._faceLength ), readPixelFormat, readPixelType, facePixels );
    cacheFile.Write ( facePixels, faceSize );

    state.Restore ();

    cacheFile.Close ();
    Free ( facePixels );
    glDeleteFramebuffers ( 1, &fbo );

    if ( !isGenerateMipmap ) return;

    UpdateMipmaps ();
}

GXTextureCubeMapEntry::GXTextureCubeMapEntry ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTextureCubeMapEntry" )
    _previous ( nullptr ),
    _next ( nullptr ),
    _references ( 1u )
{
    InitResources ( faceLength, internalFormat, isGenerateMipmap );
}

const GXWChar* GXTextureCubeMapEntry::GetFileName () const
{
    return _fileName;
}

GLuint GXTextureCubeMapEntry::GetTextureObject () const
{
    return _textureObject;
}

GXUShort GXTextureCubeMapEntry::GetFaceLength () const
{
    return _faceLength;
}

GXUByte GXTextureCubeMapEntry::GetChannelNumber () const
{
    return _numChannels;
}

GXUByte GXTextureCubeMapEntry::GetLevelOfDetailNumber () const
{
    return _lods;
}

GLint GXTextureCubeMapEntry::GetInternalFormat () const
{
    return _internalFormat;
}

GXBool GXTextureCubeMapEntry::IsGenerateMipmap () const
{
    return _isGenerateMipmap;
}

GXVoid GXTextureCubeMapEntry::FillWholePixelData ( const GXVoid* data, GLenum target )
{
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_CUBE_MAP, _textureObject );

    glPixelStorei ( GL_UNPACK_ALIGNMENT, _unpackAlignment );
    glTexImage2D ( target, 0, _internalFormat, (GLsizei)_faceLength, (GLsizei)_faceLength, 0, _format, _type, data );

    glBindTexture ( GL_TEXTURE_CUBE_MAP, 0u );

    GXCheckOpenGLError ();
}

GXVoid GXTextureCubeMapEntry::UpdateMipmaps ()
{
    if ( _textureObject == 0u || !_isGenerateMipmap ) return;

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_CUBE_MAP, _textureObject );
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
    ++_references;
}

GXVoid GXTextureCubeMapEntry::Release ()
{
    --_references;

    if ( _references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXTextureCubeMapEntry* GXCALL GXTextureCubeMapEntry::Find ( const GXWChar* equirectangularTextureFileName )
{
    for ( GXTextureCubeMapEntry* p = _top; p; p = p->_next )
    {
        if ( GXWcscmp ( p->_fileName, equirectangularTextureFileName ) != 0 ) continue;
        
        return p;
    }

    return nullptr;
}

GXUInt GXCALL GXTextureCubeMapEntry::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
    GXUInt total = 0u;

    for ( GXTextureCubeMapEntry* p = _top; p; p = p->_next )
        ++total;

    if ( total > 0u )
        *lastTexture = _top->_fileName;
    else
        *lastTexture = nullptr;

    return total;
}

GXTextureCubeMapEntry::~GXTextureCubeMapEntry ()
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

GXVoid GXTextureCubeMapEntry::InitResources ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmapPolicy )
{
    _faceLength = faceLength;
    _internalFormat = internalFormat;
    _isGenerateMipmap = isGenerateMipmapPolicy;

    glActiveTexture ( GL_TEXTURE0 );
    glGenTextures ( 1, &_textureObject );
    glBindTexture ( GL_TEXTURE_CUBE_MAP, _textureObject );

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
            _unpackAlignment = 4;
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

    glBindTexture ( GL_TEXTURE_CUBE_MAP, 0u );

    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
    FillWholePixelData ( nullptr, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );

    if ( !_isGenerateMipmap )
    {
        glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0 );
        _lods = 1u;
        return;
    }

    UpdateMipmaps ();

    _lods = 0u;
    GXUShort currentResolution = 1u;

    while ( currentResolution <= _faceLength )
    {
        ++_lods;
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
        GXLogA ( "GXTextureCubeMap::ProjectSide::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

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

GXTextureCubeMap::GXTextureCubeMap ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTextureCubeMap" )
    _textureUnit ( INVALID_TEXTURE_UNIT ),
    _textureCubeMapEntry ( nullptr )
{
    // NOTHING
}

GXTextureCubeMap::GXTextureCubeMap ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTextureCubeMap" )
    _textureUnit ( INVALID_TEXTURE_UNIT )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTextureCubeMapEntry" )
    _textureCubeMapEntry = new GXTextureCubeMapEntry ( faceLength, internalFormat, isGenerateMipmap );
}

GXTextureCubeMap::GXTextureCubeMap ( const GXWChar* equirectangularImage, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXTextureCubeMap" )
    _textureUnit ( INVALID_TEXTURE_UNIT ),
    _textureCubeMapEntry ( nullptr )
{
    LoadEquirectangularImage ( equirectangularImage, isGenerateMipmap, isApplyGammaCorrection );
}

GXTextureCubeMap::~GXTextureCubeMap ()
{
    if ( !_textureCubeMapEntry ) return;

    _textureCubeMapEntry->Release ();
}

GXUShort GXTextureCubeMap::GetFaceLength () const
{
    return _textureCubeMapEntry->GetFaceLength ();
}

GXUByte GXTextureCubeMap::GetChannelNumber () const
{
    return _textureCubeMapEntry->GetChannelNumber ();
}

GXUByte GXTextureCubeMap::GetLevelOfDetailNumber () const
{
    return _textureCubeMapEntry->GetLevelOfDetailNumber ();
}

GXVoid GXTextureCubeMap::LoadEquirectangularImage ( const GXWChar* fileName, GXBool isGenerateMipmap, GXBool isApplyGammaCorrection )
{
    if ( _textureCubeMapEntry )
        _textureCubeMapEntry->Release ();

    _textureCubeMapEntry = GXTextureCubeMapEntry::Find ( fileName );

    if ( _textureCubeMapEntry )
    {
        _textureCubeMapEntry->AddReference ();
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTextureCubeMapEntry" )
    _textureCubeMapEntry = new GXTextureCubeMapEntry ( fileName, isGenerateMipmap, isApplyGammaCorrection );
}

GXVoid GXTextureCubeMap::FillWholePixelData ( const GXVoid* data, GLenum target )
{
    if ( !_textureCubeMapEntry->GetFileName () )
    {
        _textureCubeMapEntry->FillWholePixelData ( data, target );
        return;
    }

    GXUShort faceLength = _textureCubeMapEntry->GetFaceLength ();
    GLint internalFormat = _textureCubeMapEntry->GetInternalFormat ();
    GXBool isGenerateMipmap = _textureCubeMapEntry->IsGenerateMipmap ();

    _textureCubeMapEntry->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTextureCubeMapEntry" )
    _textureCubeMapEntry = new GXTextureCubeMapEntry ( faceLength, internalFormat, isGenerateMipmap );
    _textureCubeMapEntry->FillWholePixelData ( data, target );
}

GXVoid GXTextureCubeMap::UpdateMipmaps ()
{
    _textureCubeMapEntry->UpdateMipmaps ();
}

GXVoid GXTextureCubeMap::Bind ( GXUByte unit )
{
    _textureUnit = unit;

    glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + _textureUnit ) );
    glBindTexture ( GL_TEXTURE_CUBE_MAP, _textureCubeMapEntry->GetTextureObject () );
}

GXVoid GXTextureCubeMap::Unbind ()
{
    glActiveTexture ( static_cast<GLenum> ( GL_TEXTURE0 + _textureUnit ) );
    glBindTexture ( GL_TEXTURE_CUBE_MAP, 0u );
}

GLuint GXTextureCubeMap::GetTextureObject () const
{
    return _textureCubeMapEntry->GetTextureObject ();
}

GXBool GXTextureCubeMap::IsInitialized () const
{
    return _textureCubeMapEntry != nullptr;
}

GXVoid GXTextureCubeMap::InitResources ( GXUShort faceLength, GLint internalFormat, GXBool isGenerateMipmap )
{
    if ( _textureCubeMapEntry )
        _textureCubeMapEntry->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTextureCubeMapEntry" )
    _textureCubeMapEntry = new GXTextureCubeMapEntry ( faceLength, internalFormat, isGenerateMipmap );
}

GXVoid GXTextureCubeMap::FreeResources ()
{
    if ( !_textureCubeMapEntry ) return;

    _textureCubeMapEntry->Release ();
    _textureCubeMapEntry = nullptr;
}

GXUInt GXTextureCubeMap::GetTotalLoadedTextures ( const GXWChar** lastTexture )
{
    return GXTextureCubeMapEntry::GetTotalLoadedTextures ( lastTexture );
}
