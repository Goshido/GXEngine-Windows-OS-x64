// version 1.5

#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXUIntAtomic.h>


#define EXTRA_SPACE_SYMBOLS                     32u
#define CALCULATE_BUFFER_SIZE                   0u
#define LEADING_SURROGATE                       0xD800u
#define SHORT_STRING_OPTIMIZATION_SYMBOLS       512u

//---------------------------------------------------------------------------------------------------------------------

class GXStringData final : public GXMemoryInspector
{
    private:
        GXUPointerAtomic        _references;

        GXUTF16*                _utf16Buffer;
        GXWChar*                _debugFriendlyString;
        GXUPointer              _utf16BufferSize;
        GXUPointer              _utf16AllocatedBufferSize;
        GXUPointer              _symbols;

        GXMBChar*               _multibyteCache;
        GXUPointer              _multibyteCacheSize;
        GXUPointer              _multibyteAllocatedCacheSize;
        GXBool                  _invalidMultibyteCache;

        GXUTF8*                 _utf8Cache;
        GXUPointer              _utf8CacheSize;
        GXUPointer              _utf8AllocatedCacheSize;
        GXBool                  _invalidUTF8Cache;

        static GXStringData     _nullStringData;

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
        GXVoid RecountSymbols ();

        const GXMBChar* GetMultibyteData ( GXUPointer &size );
        const GXUTF16* GetUTF16Data ( GXUPointer &size ) const;
        const GXUTF8* GetUTF8Data ( GXUPointer &size );

        const GXUPointer GetSymbolCount () const;
        const GXUPointer GetUTF16BufferSize () const;

        static GXStringData& GXCALL GetNullStringData ();

    private:
        ~GXStringData () override;

        GXStringData () = delete;
        GXStringData ( const GXStringData &other ) = delete;
        GXStringData& operator = ( const GXStringData &other ) = delete;
};

GXStringData::GXStringData ( const GXUTF16* content, GXBool canOwnContent )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXStringData" )
    _references ( 1u ),
    _utf16Buffer ( nullptr ),
    _debugFriendlyString ( nullptr ),
    _utf16BufferSize ( 0u ),
    _utf16AllocatedBufferSize ( 0u ),
    _symbols ( 0u ),
    _multibyteCache ( nullptr ),
    _multibyteCacheSize ( 0u ),
    _multibyteAllocatedCacheSize ( 0u ),
    _invalidMultibyteCache ( GX_TRUE ),
    _utf8Cache ( nullptr ),
    _utf8CacheSize ( 0u ),
    _utf8AllocatedCacheSize ( 0u ),
    _invalidUTF8Cache ( GX_TRUE )
{
    if ( !content )
    {
        _utf16BufferSize = 0u;
        _symbols = 0u;
        return;
    }

    _symbols = static_cast<const GXUPointer> ( GXWcslen ( reinterpret_cast<const GXWChar*> ( content ) ) + 1u );
    _utf16BufferSize = _symbols * sizeof ( GXUTF16 );

    if ( canOwnContent )
    {
        _utf16Buffer = const_cast<GXUTF16*> ( content );
        _debugFriendlyString = reinterpret_cast<GXWChar*> ( _utf16Buffer );
        _utf16AllocatedBufferSize = _utf16BufferSize;
        return;
    }

    _utf16AllocatedBufferSize = _utf16BufferSize + EXTRA_SPACE_SYMBOLS * sizeof ( GXUTF16 );
    _utf16Buffer = static_cast<GXUTF16*> ( Malloc ( _utf16AllocatedBufferSize ) );
    _debugFriendlyString = reinterpret_cast<GXWChar*> ( _utf16Buffer );
    memcpy ( _utf16Buffer, content, _utf16BufferSize );
}

GXVoid GXStringData::AddReference ()
{
    ++_references;
}

GXVoid GXStringData::Release ()
{
    --_references;

    if ( _references != static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXBool GXStringData::IsNullString ()
{
    return this == &_nullStringData;
}

GXBool GXStringData::IsShared ()
{
    return _references > static_cast<GXUPointer> ( 1u );
}

GXUTF16* GXStringData::GetInputBuffer ( GXUPointer neededSpace )
{
    _invalidUTF8Cache = _invalidMultibyteCache = GX_TRUE;

    if ( neededSpace <= _utf16AllocatedBufferSize )
        return _utf16Buffer;

    _utf16BufferSize = neededSpace;

    SafeFree ( reinterpret_cast<GXVoid**> ( &_utf16Buffer ) );
    _utf16AllocatedBufferSize = _utf16BufferSize + EXTRA_SPACE_SYMBOLS * sizeof ( GXUTF16 );
    _utf16Buffer = static_cast<GXUTF16*> ( Malloc ( _utf16AllocatedBufferSize ) );
    _debugFriendlyString = reinterpret_cast<GXWChar*> ( _utf16Buffer );

    return _utf16Buffer;
}

GXVoid GXStringData::RecountSymbols ()
{
    _symbols = !_utf16Buffer ? 0u : static_cast<const GXUPointer> ( GXWcslen ( reinterpret_cast<const GXWChar*> ( _utf16Buffer ) ) + 1u );
}

const GXMBChar* GXStringData::GetMultibyteData ( GXUPointer &size )
{
    if ( !_invalidMultibyteCache )
    {
        size = _multibyteCacheSize;
        return _multibyteCache;
    }

    _invalidMultibyteCache = GX_FALSE;

    if ( !_utf16Buffer )
    {
        size = 0u;
        return nullptr;
    }

    const GXInt neededSymbols = WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, reinterpret_cast<const GXWChar*> ( _utf16Buffer ), -1, _multibyteCache, 0, nullptr, nullptr );
    _multibyteCacheSize = static_cast<GXUPointer> ( neededSymbols ) * sizeof ( GXMBChar );

    if ( _multibyteCacheSize > _multibyteAllocatedCacheSize )
    {
        SafeFree ( reinterpret_cast<GXVoid**> ( &_multibyteCache ) );
        _multibyteAllocatedCacheSize = _multibyteCacheSize;
        _multibyteCache = static_cast<GXMBChar*> ( Malloc ( _multibyteAllocatedCacheSize ) );
    }

    WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, reinterpret_cast<const GXWChar*> ( _utf16Buffer ), -1, _multibyteCache, neededSymbols, nullptr, nullptr );

    size = _multibyteCacheSize;
    return _multibyteCache;
}

const GXUTF16* GXStringData::GetUTF16Data ( GXUPointer &size ) const
{
    size = _utf16BufferSize;
    return _utf16Buffer;
}

const GXUTF8* GXStringData::GetUTF8Data ( GXUPointer &size )
{
    if ( !_invalidUTF8Cache )
    {
        size = _utf8CacheSize;
        return _utf8Cache;
    }

    _invalidUTF8Cache = GX_FALSE;

    if ( !_utf16Buffer )
    {
        size = 0u;
        return nullptr;
    }

    const GXInt neededSymbols = WideCharToMultiByte ( CP_UTF8, 0u, reinterpret_cast<const GXWChar*> ( _utf16Buffer ), -1, _utf8Cache, 0, nullptr, nullptr );
    _utf8CacheSize = static_cast<GXUPointer> ( neededSymbols ) * sizeof ( GXMBChar );

    if ( _utf8CacheSize > _utf8AllocatedCacheSize )
    {
        SafeFree ( reinterpret_cast<GXVoid**> ( &_utf8Cache ) );
        _utf8AllocatedCacheSize = _utf8CacheSize;
        _utf8Cache = static_cast<GXMBChar*> ( Malloc ( _utf8AllocatedCacheSize ) );
    }

    WideCharToMultiByte ( CP_UTF8, 0u, reinterpret_cast<const GXWChar*> ( _utf16Buffer ), -1, _utf8Cache, neededSymbols, nullptr, nullptr );

    size = _utf8CacheSize;
    return _utf8Cache;
}

const GXUPointer GXStringData::GetSymbolCount () const
{
    return _symbols;
}

GXStringData::~GXStringData ()
{
    SafeFree ( reinterpret_cast<GXVoid**> ( &_multibyteCache ) );

    SafeFree ( reinterpret_cast<GXVoid**> ( &_utf16Buffer ) );
    _debugFriendlyString = nullptr;

    SafeFree ( reinterpret_cast<GXVoid**> ( &_utf8Cache ) );
}

const GXUPointer GXStringData::GetUTF16BufferSize () const
{
    return _utf16BufferSize;
}

GXStringData& GXCALL GXStringData::GetNullStringData ()
{
    _nullStringData.AddReference ();
    return _nullStringData;
}

GXStringData GXStringData::_nullStringData ( nullptr, GX_FALSE );

//---------------------------------------------------------------------------------------------------------------------

GXStringSymbol::GXStringSymbol ( const GXStringSymbol &other ):
    _lead ( other._lead ),
    _trailing ( other._trailing )
{
    // NOTHING
}

GXStringSymbol::GXStringSymbol ( const GXChar symbol ):
    _lead ( static_cast<GXUTF16> ( symbol ) ),
    _trailing ( 0 )
{
    // NOTHING
}

GXStringSymbol::GXStringSymbol ( const GXWChar symbol ):
    _lead ( static_cast<GXUTF16> ( symbol ) ),
    _trailing ( 0 )
{
    // NOTHING
}

GXStringSymbol::~GXStringSymbol ()
{
    // NOTHING
}

GXUInt GXStringSymbol::ToCodePoint () const
{
    // Based on https://en.wikipedia.org/wiki/UTF-16#U+0000_to_U+D7FF_and_U+E000_to_U+FFFF

    if ( _trailing == 0u )
        return static_cast<GXUInt> ( _lead );

    GXUInt tmp = 0x00010000u;
    tmp |= static_cast<GXUInt> ( _lead & 0x03FFu ) << 10u;
    return tmp | static_cast<GXUInt> ( _trailing & 0x03FFu );
}

GXStringSymbol& GXStringSymbol::operator = ( const GXStringSymbol &other )
{
    _lead = other._lead;
    _trailing = other._trailing;
    return *this;
}

GXStringSymbol& GXStringSymbol::operator = ( const GXChar symbol )
{
    _lead = static_cast<GXUTF16> ( symbol );
    _trailing = 0u;
    return *this;
}

GXStringSymbol& GXStringSymbol::operator = ( const GXWChar symbol )
{
    _lead = static_cast<GXUTF16> ( symbol );
    _trailing = 0u;
    return *this;
}

GXBool GXStringSymbol::operator == ( const GXStringSymbol &other ) const
{
    return _lead == other._lead && _trailing == other._trailing;
}

GXBool GXStringSymbol::operator == ( GXChar symbol ) const
{
    return ToCodePoint () == static_cast<GXUInt> ( symbol );
}

GXBool GXStringSymbol::operator == ( GXWChar symbol ) const
{
    return ToCodePoint () == static_cast<GXUInt> ( symbol );
}

GXBool GXStringSymbol::operator != ( const GXStringSymbol &other ) const
{
    return _lead != other._lead || _trailing != other._trailing;
}

GXBool GXStringSymbol::operator != ( GXChar symbol ) const
{
    return ToCodePoint () != static_cast<GXUInt> ( symbol );
}

GXBool GXStringSymbol::operator != ( GXWChar symbol ) const
{
    return ToCodePoint () != static_cast<GXUInt> ( symbol );
}

GXStringSymbol::GXStringSymbol ( GXUTF16 trivial ):
    _lead ( trivial ),
    _trailing ( 0u )
{
    // NOTHING
}

GXStringSymbol::GXStringSymbol ( GXUTF16 lead, GXUTF16 trailing ):
    _lead ( lead ),
    _trailing ( trailing )
{
    // NOTHING
}

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

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
    _stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( utf16String ), GX_TRUE );
}

GXString::GXString ( GXMBChar character )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    const GXMBChar string[ 2u ] = { character, '\0' };

    GXWChar* utf16String = nullptr;

    const GXInt symbols = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    utf16String = static_cast<GXWChar*> ( Malloc ( symbols * sizeof ( GXWChar ) ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
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

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
    _stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( string ), GX_FALSE );
}

GXString::GXString ( GXWChar character )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    const GXUShort string[ 2u ] = { static_cast<GXUShort> ( character ), 0u };
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
    _stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( string ), GX_FALSE );
}

GXString::GXString ( const GXUTF16* string )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    if ( !string )
    {
        _stringData = &( GXStringData::GetNullStringData () );
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
    _stringData = new GXStringData ( string, GX_FALSE );
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
    // Short string optimization.
    GXMBChar shortBuffer[ SHORT_STRING_OPTIMIZATION_SYMBOLS ];

    va_list ap;
    va_start ( ap, format );

    const GXUPointer neededSpace = static_cast<GXUPointer> ( vsnprintf ( nullptr, 0u, format, ap ) ) + 1u;
    GXMBChar* string = neededSpace > SHORT_STRING_OPTIMIZATION_SYMBOLS ? static_cast<GXMBChar*> ( Malloc ( neededSpace ) ) : shortBuffer;
    vsnprintf ( string, neededSpace, format, ap );

    va_end ( ap );

    FromSystemMultibyteString ( string );

    if ( neededSpace <= SHORT_STRING_OPTIMIZATION_SYMBOLS ) return;

    Free ( string );
}

const GXUPointer GXString::GetSymbolCount () const
{
    if ( IsEmpty () )
        return 0u;

    return _stringData->GetSymbolCount () - 1u;
}

const GXBool GXString::IsEmpty () const
{
    return _stringData->GetSymbolCount () <= 1u;
}

const GXBool GXString::IsNull () const
{
    return _stringData->IsNullString ();
}

GXString GXString::GetLeft ( GXUPointer lastSymbolIndex ) const
{
    if ( _stringData->IsNullString () )
        return {};

    if ( lastSymbolIndex >= _stringData->GetSymbolCount () )
        return {};

    GXUPointer utf16Size = 0u;
    const GXUTF16* utf16Data = _stringData->GetUTF16Data ( utf16Size );
    GXUPointer targetOffset = 0u;

    for ( GXUPointer i = 0u; i <= lastSymbolIndex; ++i )
        targetOffset += utf16Data[ targetOffset ] >= LEADING_SURROGATE ? 2u : 1u;

    const GXUPointer stringDataSize = targetOffset * sizeof ( GXUTF16 );

    // add space for null terminator.
    GXUTF16* buffer = static_cast<GXUTF16*> ( Malloc ( stringDataSize + sizeof ( GXUTF16 ) ) );
    memcpy ( buffer, utf16Data, stringDataSize );
    buffer[ targetOffset ] = 0u;

    return GXString ( buffer, GX_TRUE );
}

GXString GXString::GetMiddle ( GXUPointer firstSymbolIndex, GXUPointer lastSymbolIndex ) const
{
    if ( _stringData->IsNullString () || lastSymbolIndex < firstSymbolIndex )
        return {};

    const GXUPointer rawSymbols = _stringData->GetSymbolCount ();

    if ( firstSymbolIndex >= rawSymbols || lastSymbolIndex >= rawSymbols )
        return {};

    GXUPointer utf16Size = 0u;
    const GXUTF16* utf16Data = _stringData->GetUTF16Data ( utf16Size );
    const GXUPointer symbols = lastSymbolIndex - firstSymbolIndex;

    GXUPointer leftOffset = 0u;

    for ( GXUPointer i = 0u; i < firstSymbolIndex; ++i )
        leftOffset += utf16Data[ leftOffset ] >= LEADING_SURROGATE ? 2u : 1u;

    GXUPointer rightOffset = leftOffset;

    for ( GXUPointer i = 0u; i <= symbols; ++i )
        rightOffset += utf16Data[ rightOffset ] >= LEADING_SURROGATE ? 2u : 1u;

    const GXUPointer stringDataChunks = rightOffset - leftOffset;
    const GXUPointer stringDataSize = stringDataChunks * sizeof ( GXUTF16 );

    // add space for null terminator.
    GXUTF16* buffer = static_cast<GXUTF16*> ( Malloc ( stringDataSize + sizeof ( GXUTF16 ) ) );
    memcpy ( buffer, utf16Data + leftOffset, stringDataSize );
    buffer[ stringDataChunks ] = 0u;

    return GXString ( buffer, GX_TRUE );
}

GXString GXString::GetRight ( GXUPointer firstSymbolIndex ) const
{
    if ( _stringData->IsNullString () )
        return {};

    if ( firstSymbolIndex >= _stringData->GetSymbolCount () )
        return {};

    GXUPointer utf16Size = 0u;
    const GXUTF16* utf16Data = _stringData->GetUTF16Data ( utf16Size );
    GXUPointer targetOffset = 0u;

    for ( GXUPointer i = 0u; i < firstSymbolIndex; ++i )
        targetOffset += utf16Data[ targetOffset ] >= LEADING_SURROGATE ? 2u : 1u;

    const GXUPointer neededSpace = utf16Size - targetOffset * sizeof ( GXUTF16 );
    GXUTF16* buffer = static_cast<GXUTF16*> ( Malloc ( neededSpace ) );
    memcpy ( buffer, utf16Data + targetOffset, neededSpace );

    return GXString ( buffer, GX_TRUE );
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
        _stringData->RecountSymbols ();
        return;
    }

    utf16String = static_cast<GXWChar*> ( Malloc ( neededSpace ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
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
        _stringData->RecountSymbols ();
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
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
    const GXUPointer neededSpace = symbols * sizeof ( GXWChar );

    if ( !_stringData->IsNullString () && !_stringData->IsShared () )
    {
        utf16String = reinterpret_cast<GXWChar*> ( _stringData->GetInputBuffer ( neededSpace ) );
        MultiByteToWideChar ( CP_UTF8, MB_PRECOMPOSED, string, -1, utf16String, symbols );
        _stringData->RecountSymbols ();
        return;
    }

    utf16String = static_cast<GXWChar*> ( Malloc ( neededSpace ) );
    MultiByteToWideChar ( CP_UTF8, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
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

GXVoid GXString::FromUTF16 ( const GXUTF16* string )
{
    FromSystemWideString ( reinterpret_cast<const GXWChar*> ( string ) );
}

const GXUTF16* GXString::ToUTF16 () const
{
    GXUPointer tmp;
    return _stringData->GetUTF16Data ( tmp );
}

const GXUTF16* GXString::ToUTF16 ( GXUPointer &stringSize ) const
{
    return _stringData->GetUTF16Data ( stringSize );
}

GXVoid GXString::FromSymbols ( const GXStringSymbol* symbols )
{
    if ( !symbols )
    {
        Clear ();
        return;
    }

    // Null terminator at least.
    GXUPointer neededSpace = sizeof ( GXUTF16 );

    for ( const GXStringSymbol* symbol = symbols; *symbol != '\0'; ++symbol )
        neededSpace += symbol->_trailing == 0u ? sizeof ( GXUTF16 ) : 2 * sizeof ( GXUTF16 );

    GXUTF16* buffer = nullptr;
    const GXBool isReuse = !_stringData->IsNullString () && !_stringData->IsShared ();

    if ( isReuse )
    {
        buffer = _stringData->GetInputBuffer ( neededSpace );
    }
    else
    {
        buffer = static_cast<GXUTF16*> ( Malloc ( neededSpace ) );
        _stringData->Release ();
    }

    GXUTF16* write = buffer;

    for ( const GXStringSymbol* symbol = symbols; *symbol != '\0'; ++symbol )
    {
        *write = symbol->_lead;
        ++write;

        if ( symbol->_trailing == 0u ) continue;

        *write = symbol->_trailing;
        ++write;
    }

    // Null terminator.
    *write = 0u;

    if ( isReuse )
    {
        _stringData->RecountSymbols ();
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
    _stringData = new GXStringData ( buffer, GX_TRUE );
}

GXStringSymbol* GXString::ToSymbols () const
{
    if ( IsNull () )
        return nullptr;

    GXStringSymbol* symbols = static_cast<GXStringSymbol*> ( Malloc ( sizeof ( GXStringSymbol ) * ( GetSymbolCount () + 1u ) ) );
    GXStringSymbol* write = symbols;

    GXUPointer chunkCount = 0u;
    const GXUTF16* data = _stringData->GetUTF16Data ( chunkCount );
    chunkCount /= sizeof ( GXUTF16 );

    for ( GXUPointer i = 0u; i < chunkCount; ++write )
    {
        // Extract high 6 bits. 0xD800 is lead surrogate.
        // See https://en.wikipedia.org/wiki/UTF-16#U+0000_to_U+D7FF_and_U+E000_to_U+FFFF

        const GXUTF16 lead = data[ i ];

        if ( ( lead & 0xFC00u ) == 0xD800u )
        {
            // Two GXUTF16.
            write->_lead = lead;
            write->_trailing = data[ i + 1u ];
            i += 2u;

            continue;
        }

        // One GXUTF16.
        write->_lead = lead;
        write->_trailing = 0u;

        ++i;
    }

    return symbols;
}

GXBool GXString::ToSymbols ( GXStringSymbol*& buffer, GXUPointer maxSymbolCount ) const
{
    const GXUPointer targetSymbolCount = GetSymbolCount () + 1u;

    if ( maxSymbolCount < targetSymbolCount )
    {
        GXLogA ( "GXString::ToSymbols::Error - Buffer is too small. Buffer maximum symbol capacity: %zu. String symbols: %zu\n", maxSymbolCount, targetSymbolCount );
        return GX_FALSE;
    }

    GXStringSymbol* write = buffer;

    GXUPointer chunkCount = 0u;
    const GXUTF16* data = _stringData->GetUTF16Data ( chunkCount );
    chunkCount /= sizeof ( GXUTF16 );

    for ( GXUPointer i = 0u; i < chunkCount; ++write )
    {
        // Extract high 6 bits. 0xD800 is lead surrogate.
        // See https://en.wikipedia.org/wiki/UTF-16#U+0000_to_U+D7FF_and_U+E000_to_U+FFFF

        const GXUTF16 lead = data[ i ];

        if ( ( lead & 0xFC00u ) == 0xD800u )
        {
            // Two GXUTF16.
            write->_lead = lead;
            write->_trailing = data[ i + 1u ];
            i += 2u;

            continue;
        }

        // One GXUTF16.
        write->_lead = lead;
        write->_trailing = 0u;

        ++i;
    }

    return GX_TRUE;
}

eGXCompareResult GXString::Compare ( const GXString other ) const
{
    GXUPointer tmp;
    return static_cast<eGXCompareResult> ( GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), reinterpret_cast<const GXWChar*> ( other._stringData->GetUTF16Data ( tmp ) ) ) );
}

eGXCompareResult GXString::Compare ( const GXMBChar* string ) const
{
    GXUPointer tmp;
    return static_cast<eGXCompareResult> ( GXMbscmp ( _stringData->GetMultibyteData ( tmp ), string ) );
}

eGXCompareResult GXString::Compare ( const GXMBChar character ) const
{
    const GXMBChar string[ 2u ] = { character, '\0' };
    GXUPointer tmp;

    return static_cast<eGXCompareResult> ( GXMbscmp ( _stringData->GetMultibyteData ( tmp ), string ) );
}

eGXCompareResult GXString::Compare ( const GXWChar* string ) const
{
    GXUPointer tmp;
    return static_cast<eGXCompareResult> ( GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), string ) );
}

eGXCompareResult GXString::Compare ( GXWChar character ) const
{
    const GXWChar string[ 2u ] = { character, L'\0' };
    GXUPointer tmp;

    return static_cast<eGXCompareResult> ( GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), string ) );
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
    const GXMBChar string[ 2u ] = { character, '\0' };
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
    const GXWChar string[ 2u ] = { character, L'\0' };
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
    const GXMBChar string[ 2u ] = { character, '\0' };

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
    const GXMBChar string[ 2u ] = { character, '\0' };
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
    const GXWChar string[ 2u ] = { character, L'\0' };
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
    const GXMBChar string[ 2u ] = { character, '\0' };
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
    const GXWChar string[ 2u ] = { character, L'\0' };
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
    const GXMBChar string[ 2u ] = { character, '\0' };
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
    const GXWChar string[ 2u ] = { character, L'\0' };
    GXUPointer tmp;

    return GXWcscmp ( reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) ), string ) == 0;
}

GXBool GXString::operator != ( const GXString &other ) const
{
    return !( operator == ( other ) );
}

GXBool GXString::operator != ( const GXMBChar* string ) const
{
    return !( operator == ( string ) );
}

GXBool GXString::operator != ( GXMBChar character ) const
{
    return !( operator == ( character ) );
}

GXBool GXString::operator != ( const GXWChar* string ) const
{
    return !( operator == ( string ) );
}

GXBool GXString::operator != ( GXWChar character ) const
{
    return !( operator == ( character ) );
}

GXStringSymbol GXString::operator [] ( GXUPointer symbolIndex ) const
{
    const GXUPointer count = GetSymbolCount ();

    if ( symbolIndex >= count )
    {
        GXLogA ( "GXString::operator [] - Can't return symbol with %zu index. String contains %zu symbol(s) only.", symbolIndex, count );
        return GXStringSymbol ( '\0' );
    }

    GXUPointer tmp;
    const GXUTF16* data = _stringData->GetUTF16Data ( tmp );

    for ( GXUPointer i = 0u; i < symbolIndex; ++i )
    {
        // Extract high 6 bits. 0xD800 is lead surrogate.
        // See https://en.wikipedia.org/wiki/UTF-16#U+0000_to_U+D7FF_and_U+E000_to_U+FFFF

        data += ( ( *data ) & 0xFC00u ) == 0xD800u ? 2u : 1u;
    }

    // Same trick.
    return ( ( *data ) & 0xFC00u ) == 0xD800u ? GXStringSymbol ( data[ 0u ], data[ 1u ] ) : GXStringSymbol ( data[ 0u ] );
}

GXString::operator const GXMBChar* () const
{
    GXUPointer tmp;
    return _stringData->GetMultibyteData ( tmp );
}

GXString::operator const GXUTF16* () const
{
    GXUPointer tmp;
    return _stringData->GetUTF16Data ( tmp );
}

GXString::operator const GXWChar* () const
{
    GXUPointer tmp;
    return reinterpret_cast<const GXWChar*> ( _stringData->GetUTF16Data ( tmp ) );
}

GXString::GXString ( const GXUTF16* content, GXBool canOwnContent )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" )
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
