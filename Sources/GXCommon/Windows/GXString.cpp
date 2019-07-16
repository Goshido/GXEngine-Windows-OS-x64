// version 1.0

#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXUIntAtomic.h>


#define EXTRA_SPACE_SYMBOLS     32u
#define CALCULATE_BUFFER_SIZE   0u

//---------------------------------------------------------------------------------------------------------------------

class GXStringData final : public GXMemoryInspector
{
    private:
        GXUPointerAtomic        references;

        GXUTF16*                utf16Buffer;
        GXUPointer              utf16BufferSize;
        GXUPointer              utf16AllocatedBufferSize;
        GXUPointer              symbols;

        GXMBChar*               multibyteCache;
        GXUPointer              multibyteCacheSize;
        GXUPointer              multibyteAllocatedCacheSize;
        GXBool                  invalidMultibyteCache;

        GXUTF8*                 utf8Cache;
        GXUPointer              utf8CacheSize;
        GXUPointer              utf8AllocatedCacheSize;
        GXBool                  invalidUTF8Cache;

        static GXStringData     nullStringData;

    public:
        // Note content ownership will be transmited to this class if canOwnContent equals GX_TRUE.
        explicit GXStringData ( const GXUTF16* content, GXBool canOwnContent );

        GXVoid AddReference ();
        GXVoid Release ();

        GXBool IsNullString ();
        GXBool IsShared ();

        // Returns buffer for fill UTF16 string content.
        // Method will try to reuse existing buffer or make Malloc for new buffer.
        GXUTF16* GetInputBuffer ( GXUPointer neededSpace );

        const GXMBChar* GetMultibyteData ( GXUPointer &size );
        const GXUTF16* GetUTF16Data ( GXUPointer &size ) const;
        const GXUTF8* GetUTF8Data ( GXUPointer &size );

        const GXUPointer GetSymbolCount () const;

        static GXStringData& GXCALL GetNullStringData ();

    private:
        ~GXStringData () override;

        GXStringData () = delete;
        GXStringData ( const GXStringData &other ) = delete;
        GXStringData& operator = ( const GXStringData &other ) = delete;
};

GXStringData::GXStringData ( const GXUTF16* content, GXBool canOwnContent )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXStringData" )
    references ( 1u ),
    utf16Buffer ( nullptr ),
    utf16BufferSize ( 0u ),
    utf16AllocatedBufferSize ( 0u ),
    symbols ( 0u ),
    multibyteCache ( nullptr ),
    multibyteCacheSize ( 0u ),
    multibyteAllocatedCacheSize ( 0u ),
    invalidMultibyteCache ( GX_TRUE ),
    utf8Cache ( nullptr ),
    utf8CacheSize ( 0u ),
    utf8AllocatedCacheSize ( 0u ),
    invalidUTF8Cache ( GX_TRUE )
{
    if ( !content )
    {
        utf16BufferSize = 0u;
        symbols = 0u;
        return;
    }

    symbols = static_cast<const GXUPointer> ( GXWcslen ( reinterpret_cast<const GXWChar*> ( content ) ) + 1u );
    utf16BufferSize = symbols * sizeof ( GXUTF16 );

    if ( canOwnContent )
    {
        utf16Buffer = const_cast<GXUTF16*> ( content );
        utf16AllocatedBufferSize = utf16BufferSize;
        return;
    }

    utf16AllocatedBufferSize = utf16BufferSize + EXTRA_SPACE_SYMBOLS * sizeof ( GXUTF16 );
    utf16Buffer = static_cast<GXUTF16*> ( Malloc ( utf16AllocatedBufferSize ) );
    memcpy ( utf16Buffer, content, utf16BufferSize );
}

GXVoid GXStringData::AddReference ()
{
    ++references;
}

GXVoid GXStringData::Release ()
{
    --references;

    if ( references != static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXBool GXStringData::IsNullString ()
{
    return this == &nullStringData;
}

GXBool GXStringData::IsShared ()
{
    return references > static_cast<GXUPointer> ( 1u );
}

GXUTF16* GXStringData::GetInputBuffer ( GXUPointer neededSpace )
{
    invalidUTF8Cache = invalidMultibyteCache = GX_TRUE;

    if ( neededSpace <= utf16AllocatedBufferSize )
        return utf16Buffer;

    utf16BufferSize = neededSpace;

    SafeFree ( reinterpret_cast<GXVoid**> ( &utf16Buffer ) );
    utf16AllocatedBufferSize = utf16BufferSize + EXTRA_SPACE_SYMBOLS * sizeof ( GXUTF16 );
    utf16Buffer = static_cast<GXUTF16*> ( Malloc ( utf16AllocatedBufferSize ) );

    return utf16Buffer;
}

const GXMBChar* GXStringData::GetMultibyteData ( GXUPointer &size )
{
    if ( !invalidMultibyteCache )
    {
        size = multibyteCacheSize;
        return multibyteCache;
    }

    invalidMultibyteCache = GX_FALSE;

    if ( !utf16Buffer )
    {
        size = 0u;
        return nullptr;
    }

    const GXInt neededSymbols = WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, reinterpret_cast<const GXWChar*> ( utf16Buffer ), -1, multibyteCache, 0, nullptr, nullptr );
    multibyteCacheSize = static_cast<GXUPointer> ( neededSymbols ) * sizeof ( GXMBChar );

    if ( multibyteCacheSize > multibyteAllocatedCacheSize )
    {
        SafeFree ( reinterpret_cast<GXVoid**> ( &multibyteCache ) );
        multibyteAllocatedCacheSize = multibyteCacheSize;
        multibyteCache = static_cast<GXMBChar*> ( Malloc ( multibyteAllocatedCacheSize ) );
    }

    WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, reinterpret_cast<const GXWChar*> ( utf16Buffer ), -1, multibyteCache, neededSymbols, nullptr, nullptr );

    size = multibyteCacheSize;
    return multibyteCache;
}

const GXUTF16* GXStringData::GetUTF16Data ( GXUPointer &size ) const
{
    size = utf16BufferSize;
    return utf16Buffer;
}

const GXUTF8* GXStringData::GetUTF8Data ( GXUPointer &size )
{
    if ( !invalidUTF8Cache )
    {
        size = utf8CacheSize;
        return utf8Cache;
    }

    invalidUTF8Cache = GX_FALSE;

    if ( !utf16Buffer )
    {
        size = 0u;
        return nullptr;
    }

    const GXInt neededSymbols = WideCharToMultiByte ( CP_UTF8, 0u, reinterpret_cast<const GXWChar*> ( utf16Buffer ), -1, utf8Cache, 0, nullptr, nullptr );
    utf8CacheSize = static_cast<GXUPointer> ( neededSymbols ) * sizeof ( GXMBChar );

    if ( utf8CacheSize > utf8AllocatedCacheSize )
    {
        SafeFree ( reinterpret_cast<GXVoid**> ( &utf8Cache ) );
        utf8AllocatedCacheSize = utf8CacheSize;
        utf8Cache = static_cast<GXMBChar*> ( Malloc ( utf8AllocatedCacheSize ) );
    }

    WideCharToMultiByte ( CP_UTF8, 0u, reinterpret_cast<const GXWChar*> ( utf16Buffer ), -1, utf8Cache, neededSymbols, nullptr, nullptr );

    size = utf8CacheSize;
    return utf8Cache;
}

const GXUPointer GXStringData::GetSymbolCount () const
{
    return symbols;
}

GXStringData::~GXStringData ()
{
    SafeFree ( reinterpret_cast<GXVoid**> ( &multibyteCache ) );
    SafeFree ( reinterpret_cast<GXVoid**> ( &utf16Buffer ) );
    SafeFree ( reinterpret_cast<GXVoid**> ( &utf8Cache ) );
}

GXStringData& GXCALL GXStringData::GetNullStringData ()
{
    nullStringData.AddReference ();
    return nullStringData;
}

GXStringData GXStringData::nullStringData ( nullptr, GX_FALSE );

//---------------------------------------------------------------------------------------------------------------------

GXString::GXString ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXString" )
    _stringData ( &( GXStringData::GetNullStringData () ) )
{
    // NOTHING
}

GXString::GXString ( const GXString &other )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    other._stringData->AddReference ();
    _stringData = other._stringData;
}

GXString::GXString ( const GXMBChar* string )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    if ( !string )
    {
        _stringData = &( GXStringData::GetNullStringData () );
        return;
    }

    GXWChar* utf16String = nullptr;

    const GXInt symbols = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    utf16String = static_cast<GXWChar*> ( Malloc ( symbols * sizeof ( GXWChar ) ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    _stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( utf16String ), GX_TRUE );
}

GXString::GXString ( GXMBChar character )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    const GXMBChar string[ 2u ] = { character, 0u };

    GXWChar* utf16String = nullptr;

    const GXInt symbols = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    utf16String = static_cast<GXWChar*> ( Malloc ( symbols * sizeof ( GXWChar ) ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    _stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( utf16String ), GX_TRUE );
}

GXString::GXString ( const GXWChar* string )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    if ( !string )
    {
        _stringData = &( GXStringData::GetNullStringData () );
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    _stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( string ), GX_FALSE );
}

GXString::GXString ( GXWChar character )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    const GXUShort string[ 2u ] = { static_cast<GXUShort> ( character ), 0u };
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    _stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( string ), GX_FALSE );
}

GXString::~GXString ()
{
    _stringData->Release ();
}

GXVoid GXString::Clear ()
{
    _stringData->Release ();
    _stringData = &( GXStringData::GetNullStringData () );
}

GXVoid GXCDECLCALL GXString::Format ( const GXMBChar* format, ... )
{
    va_list ap;
    va_start ( ap, format );

    GXUPointer neededSpace = static_cast<GXUPointer> ( vsnprintf ( nullptr, 0u, format, ap ) ) + 1u;
    GXMBChar* string = static_cast<GXMBChar*> ( Malloc ( neededSpace ) );
    vsnprintf ( string, neededSpace, format, ap );

    va_end ( ap );

    FromSystemMultibyteString ( string );
    Free ( string );
}

const GXUPointer GXString::GetSymbolCount () const
{
    return _stringData->GetSymbolCount ();
}

const GXBool GXString::IsEmpty () const
{
    return _stringData->GetSymbolCount () < 1u;
}

const GXBool GXString::IsNull () const
{
    return _stringData->IsNullString ();
}

GXVoid GXString::FromSystemMultibyteString ( const GXMBChar* string )
{
    if ( !string )
    {
        Clear ();
        return;
    }

    GXWChar* utf16String = nullptr;
    const GXInt symbols = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    const GXUPointer neededSpace = symbols * sizeof ( GXWChar );

    if ( !_stringData->IsNullString () && !_stringData->IsShared () )
    {
        utf16String = reinterpret_cast<GXWChar*> ( _stringData->GetInputBuffer ( neededSpace ) );
        MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );
        return;
    }

    utf16String = static_cast<GXWChar*> ( Malloc ( neededSpace ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    GXStringData* newStringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( utf16String ), GX_TRUE );

    _stringData->Release ();
    _stringData = newStringData;
}

const GXMBChar* GXString::ToSystemMultibyteString ()
{
    GXUPointer tmp;
    return _stringData->GetMultibyteData ( tmp );
}

const GXMBChar* GXString::ToSystemMultibyteString ( GXUPointer &stringSize )
{
    return _stringData->GetMultibyteData ( stringSize );
}

GXVoid GXString::FromSystemWideString ( const GXWChar* string )
{
    if ( !string )
    {
        Clear ();
        return;
    }

    if ( !_stringData->IsNullString () && !_stringData->IsShared () )
    {
        const GXUPointer neededSpace = ( GXWcslen ( string ) + 1u ) * sizeof ( GXUTF16 );
        memcpy ( _stringData->GetInputBuffer ( neededSpace ), string, neededSpace );
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    GXStringData* newStringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( string ), GX_FALSE );

    _stringData->Release ();
    _stringData = newStringData;
}

const GXWChar* GXString::ToSystemWideString ()
{
    GXUPointer tmp;
    return reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) );
}

const GXWChar* GXString::ToSystemWideString ( GXUPointer &stringSize )
{
    return reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( stringSize ) );
}

GXVoid GXString::FromUTF8 ( const GXUTF8* string )
{
    if ( !string )
    {
        Clear ();
        return;
    }

    GXWChar* utf16String = nullptr;

    const GXInt symbols = MultiByteToWideChar ( CP_UTF8, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    utf16String = static_cast<GXWChar*> ( Malloc ( symbols * sizeof ( GXWChar ) ) );
    const GXUPointer neededSpace = symbols * sizeof ( GXWChar );

    if ( !_stringData->IsNullString () && !_stringData->IsShared () )
    {
        utf16String = reinterpret_cast<GXWChar*> ( _stringData->GetInputBuffer ( neededSpace ) );
        MultiByteToWideChar ( CP_UTF8, MB_PRECOMPOSED, string, -1, utf16String, symbols );
        return;
    }

    utf16String = static_cast<GXWChar*> ( Malloc ( neededSpace ) );
    MultiByteToWideChar ( CP_UTF8, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    GXStringData* newStringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( utf16String ), GX_TRUE );

    _stringData->Release ();
    _stringData = newStringData;
}

const GXUTF8* GXString::ToUTF8 ()
{
    GXUPointer tmp;
    return _stringData->GetUTF8Data ( tmp );
}

const GXUTF8* GXString::ToUTF8 ( GXUPointer &stringSize )
{
    return _stringData->GetUTF8Data ( stringSize );
}

GXString& GXString::operator = ( const GXString &other )
{
    other._stringData->AddReference ();
    _stringData->Release ();
    _stringData = other._stringData;
    return *this;
}

GXString& GXString::operator = ( const GXMBChar* string )
{
    FromSystemMultibyteString ( string );
    return *this;
}

GXString& GXString::operator = ( GXMBChar character )
{
    const GXMBChar string[ 2u ] = { character, 0u };
    FromSystemMultibyteString ( string );
    return *this;
}

GXString& GXString::operator = ( const GXWChar* string )
{
    FromSystemWideString ( string );
    return *this;
}

GXString& GXString::operator = ( GXWChar character )
{
    const GXWChar string[ 2u ] = { character, 0u };
    FromSystemWideString ( string );
    return *this;
}

GXString GXString::operator + ( const GXString &other )
{
    if ( other._stringData->IsNullString () )
        return GXString ( *this );

    GXUPointer otherDataSize;
    const GXUTF16* otherData = other._stringData->GetUTF16Data ( otherDataSize );

    return Append ( otherData, otherDataSize, GX_FALSE );
}

GXString GXString::operator + ( const GXMBChar* string )
{
    if ( !string )
        return GXString ( *this );

    GXWChar* utf16String = nullptr;

    const GXInt symbols = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    const GXUPointer utf16StringSize = symbols * sizeof ( GXWChar );
    utf16String = static_cast<GXWChar*> ( Malloc ( utf16StringSize ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    return Append ( reinterpret_cast<const GXUTF16*> ( utf16String ), utf16StringSize, GX_TRUE );
}

GXString GXString::operator + ( GXMBChar character )
{
    const GXMBChar string[ 2u ] = { character, 0u };

    GXWChar* utf16String = nullptr;

    const GXInt symbols = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    const GXUPointer utf16StringSize = symbols * sizeof ( GXWChar );
    utf16String = static_cast<GXWChar*> ( Malloc ( symbols * sizeof ( GXWChar ) ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    return Append ( reinterpret_cast<const GXUTF16*> ( utf16String ), utf16StringSize, GX_TRUE );
}

GXString GXString::operator + ( const GXWChar* string )
{
    if ( !string )
        return GXString ( *this );

    return Append ( reinterpret_cast<const GXUTF16*> ( string ), CALCULATE_BUFFER_SIZE, GX_FALSE );
}

GXString GXString::operator + ( GXWChar character )
{
    const GXUShort string[ 2u ] = { static_cast<GXUShort> ( character ), 0u };
    return Append ( string, 2u * sizeof ( GXUShort ), GX_FALSE );
}

GXString& GXString::operator += ( const GXString &other )
{
    *this = *this + other;
    return *this;
}

GXString& GXString::operator += ( const GXMBChar* string )
{
    *this = *this + string;
    return *this;
}

GXString& GXString::operator += ( GXMBChar character )
{
    *this = *this + character;
    return *this;
}

GXString& GXString::operator += ( const GXWChar* string )
{
    *this = *this + string;
    return *this;
}

GXString& GXString::operator += ( GXWChar character )
{
    *this = *this + character;
    return *this;
}

GXBool GXString::operator > ( const GXString &other ) const
{
    GXUPointer tmp;
    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), reinterpret_cast<const GXWChar*> ( other._stringData->GetUTF16Data ( tmp ) ) ) > 0;
}

GXBool GXString::operator > ( const GXMBChar* string ) const
{
    GXUPointer tmp;
    return GXMbscmp ( _stringData->GetMultibyteData ( tmp ), string ) > 0;
}

GXBool GXString::operator > ( GXMBChar character ) const
{
    const GXMBChar string[ 2u ] = { character, 0 };
    GXUPointer tmp;

    return GXMbscmp ( _stringData->GetMultibyteData ( tmp ), string ) > 0;
}

GXBool GXString::operator > ( const GXWChar* string ) const
{
    GXUPointer tmp;
    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), string ) > 0;
}

GXBool GXString::operator > ( GXWChar character ) const
{
    const GXWChar string[ 2u ] = { character, 0 };
    GXUPointer tmp;

    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), string ) > 0;
}

GXBool GXString::operator < ( const GXString &other ) const
{
    GXUPointer tmp;
    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), reinterpret_cast<const GXWChar*> ( other._stringData->GetUTF16Data ( tmp ) ) ) < 0;
}

GXBool GXString::operator < ( const GXMBChar* string ) const
{
    GXUPointer tmp;
    return GXMbscmp ( _stringData->GetMultibyteData ( tmp ), string ) < 0;
}

GXBool GXString::operator < ( GXMBChar character ) const
{
    const GXMBChar string[ 2u ] = { character, 0 };
    GXUPointer tmp;

    return GXMbscmp ( _stringData->GetMultibyteData ( tmp ), string ) < 0;
}

GXBool GXString::operator < ( const GXWChar* string ) const
{
    GXUPointer tmp;
    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), string ) < 0;
}

GXBool GXString::operator < ( GXWChar character ) const
{
    const GXWChar string[ 2u ] = { character, 0 };
    GXUPointer tmp;

    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), string ) < 0;
}

GXBool GXString::operator == ( const GXString &other ) const
{
    GXUPointer tmp;
    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), reinterpret_cast<const GXWChar*> ( other._stringData->GetUTF16Data ( tmp ) ) ) == 0;
}

GXBool GXString::operator == ( const GXMBChar* string ) const
{
    GXUPointer tmp;
    return GXMbscmp ( _stringData->GetMultibyteData ( tmp ), string ) == 0;
}

GXBool GXString::operator == ( GXMBChar character ) const
{
    const GXMBChar string[ 2u ] = { character, 0 };
    GXUPointer tmp;

    return GXMbscmp ( _stringData->GetMultibyteData ( tmp ), string ) == 0;
}

GXBool GXString::operator == ( const GXWChar* string ) const
{
    GXUPointer tmp;
    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), string ) == 0;
}

GXBool GXString::operator == ( GXWChar character ) const
{
    const GXWChar string[ 2u ] = { character, 0 };
    GXUPointer tmp;

    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), string ) == 0;
}

GXString::operator const GXMBChar* () const
{
    GXUPointer tmp;
    return _stringData->GetMultibyteData ( tmp );
}

GXString::operator const GXWChar* () const
{
    GXUPointer tmp;
    return reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) );
}

GXString::GXString ( const GXUTF16* content, GXBool canOwnContent )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXString" );
    _stringData = new GXStringData ( content, canOwnContent );
}

GXString GXString::Append ( const GXUTF16* buffer, GXUPointer bufferSize, GXBool canOwnContent )
{
    if ( _stringData->IsNullString () )
        return GXString ( buffer, canOwnContent );

    GXUPointer myDataSize;
    const GXUTF16* myData = _stringData->GetUTF16Data ( myDataSize );
    myDataSize -= sizeof ( GXUTF16 );

    if ( bufferSize == CALCULATE_BUFFER_SIZE )
        bufferSize = GXWcslen ( reinterpret_cast<const GXWChar*> ( buffer ) ) * sizeof ( GXUTF16 );
    else
        bufferSize -= sizeof ( GXUTF16 );

    const GXUPointer tmp = myDataSize + bufferSize;
    GXUByte* resultData = static_cast<GXUByte*> ( Malloc ( tmp + sizeof ( GXUTF16 ) ) );
    memcpy ( resultData, myData, myDataSize );
    memcpy ( resultData + myDataSize, buffer, bufferSize );
    resultData[ tmp ] = resultData[ tmp + 1u ] = 0u;

    if ( canOwnContent )
        Free ( const_cast<GXUTF16*> ( buffer ) );

    return GXString ( reinterpret_cast<const GXUTF16*> ( resultData ), GX_TRUE );
}
