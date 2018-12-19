// version 1.5

#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>


#define INVALID_SYMBOL      0xFFFFFFFFu

//---------------------------------------------------------------------------------------------------------------------

GXUTF8Parser::GXUTF8Parser ( const GXUTF8* string ):
    string ( string ),
    totalSymbols ( GXUTF8len ( string ) )
{
    // NOTHING
}

GXUTF8Parser::~GXUTF8Parser ()
{
    // NOTHING
}

GXUInt GXUTF8Parser::GetSymbol ( GXUInt position )
{
    if ( position >= totalSymbols ) return INVALID_SYMBOL;

    GXUInt offset = GetOffset ( position );

    if ( ( string[ offset ] & 0x80 ) == 0x00 )
        return static_cast<GXUInt> ( string[ offset ] );

    if ( ( string[ offset ] & 0xE0 ) == 0xC0 )
    {
        return ( ( static_cast<GXUInt>( string[ offset ] ) & 0x0000001Fu ) << 6 ) |
                ( static_cast<GXUInt> ( string[ offset + 1u ] ) & 0x0000003Fu );
    }

    if ( ( string[ offset ] & 0xF0 ) == 0xE0 )
    {
        return ( ( static_cast<GXUInt> ( string[ offset ] ) & 0x0000000Fu ) << 12 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 1u ] ) & 0x0000003Fu ) << 6 ) |
                ( static_cast<GXUInt> ( string[ offset + 2u ] ) & 0x0000003Fu );
    }

    if ( ( string[ offset ] & 0xF8 ) == 0xF0 )
    {
        return ( ( static_cast<GXUInt> ( string[ offset ] ) & 0x00000007u ) << 18 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 1u ] ) & 0x0000003Fu ) << 12 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 2u ] ) & 0x0000003F ) << 6 ) |
                ( static_cast<GXUInt> ( string[ offset + 3u ] ) & 0x0000003F );
    }

    if ( ( string[ offset ] & 0xFC ) == 0xF8 )
    {
        return ( ( static_cast<GXUInt> ( string[ offset ] ) & 0x00000003u ) << 24 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 1u ] ) & 0x0000003Fu ) << 18 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 2u ] ) & 0x0000003Fu ) << 12 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 3u ] ) & 0x0000003Fu ) << 6 ) |
                ( static_cast<GXUInt> ( string[ offset + 4u ] ) & 0x0000003Fu );
    }

    if ( ( string[ offset ] & 0xFE ) == 0xFC )
    {
        return ( ( static_cast<GXUInt> ( string[ offset ] ) & 0x00000001u ) << 30 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 1u ] ) & 0x0000003Fu ) << 24 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 2u ] ) & 0x0000003Fu ) << 18 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 3u ] ) & 0x0000003Fu ) << 12 ) |
                ( ( static_cast<GXUInt> ( string[ offset + 4u ] ) & 0x0000003Fu ) << 6 ) |
                ( static_cast<GXUInt> ( string[ offset + 5u ] ) & 0x0000003F );
    }

    GXLogA ( "GXUTF8Parser::GetSymbol::Error - Can't parse symbol [position %i]\n", position );
    return INVALID_SYMBOL;
}

GXUInt GXUTF8Parser::GetLength ()
{
    return totalSymbols;
}

GXVoid GXUTF8Parser::Copy ( GXUTF8* dest, GXUInt startPos, GXUInt endPos )
{
    if ( endPos >= totalSymbols ) return;

    GXUInt offset = 0u;
    GXUInt baseOffset = GetOffset ( startPos );

    for ( GXUInt i = startPos; i <= endPos; ++i )
    {
        GXUInt ind = baseOffset + offset;

        if ( ( string[ ind ] & 0x80 ) == 0x00 )
        {
            dest[ offset ] = string[ ind ];
            ++offset;

            continue;
        }

        if ( ( string[ ind ] & 0xE0 ) == 0xC0 )
        {
            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;

            continue;
        }

        if ( ( string[ ind ] & 0xF0 ) == 0xE0 )
        {
            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;

            continue;
        }

        if ( ( string[ ind ] & 0xF8 ) == 0xF0 )
        {
            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;

            continue;
        }

        if ( ( string[ ind ] & 0xFC ) == 0xF8 )
        {
            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;

            continue;
        }

        if ( ( string[ ind ] & 0xFE ) == 0xFC )
        {
            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
            ++ind;

            dest[ offset ] = string[ ind ];
            ++offset;
        }
    }
}

GXUInt GXUTF8Parser::GetOffset ( GXUInt position )
{
    if ( position == 0u )
        return 0u;

    GXUInt offset = 0u;
    GXUInt i = 0u;

    while ( GX_TRUE )
    {
        if ( ( string[ offset ] & 0xC0 ) == 0x80 )
        {
            ++offset;
            continue;
        }

        ++i;
        ++offset;

        if ( i < position ) continue;

        while ( GX_TRUE )
        {
            if ( ( string[ offset ] & 0xC0 ) != 0x80 )
                break;

            ++offset;
        }

        break;
    }

    return offset;
}

GXVoid GXUTF8Parser::Debug ()
{
    for ( GXUInt i = 0u; string[ i ]; ++i )
    {
        GXUByte bit0 = static_cast<GXUByte> ( string[ i ] & 0x01 );
        GXUByte bit1 = static_cast<GXUByte> ( ( string[ i ] & 0x02 ) >> 1 );
        GXUByte bit2 = static_cast<GXUByte> ( ( string[ i ] & 0x04 ) >> 2 );
        GXUByte bit3 = static_cast<GXUByte> ( ( string[ i ] & 0x08 ) >> 3 );
        GXUByte bit4 = static_cast<GXUByte> ( ( string[ i ] & 0x10 ) >> 4 );
        GXUByte bit5 = static_cast<GXUByte> ( ( string[ i ] & 0x20 ) >> 5 );
        GXUByte bit6 = static_cast<GXUByte> ( ( string[ i ] & 0x40 ) >> 6 );
        GXUByte bit7 = static_cast<GXUByte> ( ( string[ i ] & 0x80 ) >> 7 );

        GXLogW ( L"GXUTF8Parser::Debug::Info - %i%i%i%i %i%i%i%i\n", bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0 );
    }
}

//--------------------------------------------------------------

eGXEndianness GXCALL GXDetermineEndianness ()
{
    static const GXUByte magic[ 4u ] = { 0xA1u, 0xB2u, 0xC3u, 0xD4u };
    const GXInt* test = reinterpret_cast<const GXInt*> ( magic );

    switch ( *test )
    {
        case 0xA1B2C3D4:
        return eGXEndianness::BigEndian;

        case 0xD4C3B2A1:
        return eGXEndianness::LittleEndian;

        case 0xB2A1D4C3:
        return eGXEndianness::PDPEndian;

        default:
        return eGXEndianness::ErrorEndian;
    }
}

//--------------------------------------------------------------


GXUInt GXCALL GXWcslen ( const GXWChar* str )
{
    if ( !str )
        return 0u;

    return static_cast<GXUInt> ( wcslen ( str ) );
}

GXUInt GXCALL GXUTF8len ( const GXUTF8* str )
{
    if ( !str )
        return 0u;

    GXUInt numBytes = static_cast<GXUInt> ( strlen ( str ) );

    if ( numBytes == 0u )
        return 0u;

    GXUInt numSymbols = 0u;

    for ( GXUInt i = 0u; i < numBytes; ++i )
    {
        if ( ( str[ i ] & 0xC0 ) != 0x80 )
            ++numSymbols;
    }

    return numSymbols;
}

GXInt GXCALL GXWcscmp ( const GXWChar* a, const GXWChar* b )
{
    if ( a == nullptr && b == nullptr ) return 0;
    if ( a == nullptr ) return -1;
    if ( b == nullptr ) return 1;

    return wcscmp ( a, b );
}

GXInt GXCALL GXUTF8cmp ( const GXUTF8* a, const GXUTF8* b )
{
    return strcmp ( a, b );
}

GXInt GXCALL GXMbscmp ( const GXMBChar* a, const GXMBChar* b )
{
    return strcmp ( a, b );
}

GXUPointer GXCALL GXUTF8size ( const GXUTF8* str )
{
    return strlen ( str ) + 1u;
}

GXVoid GXCALL GXWcsclone ( GXWChar** dest, const GXWChar* src )
{
    if ( !src )
    {
        *dest = nullptr;
        return;
    }

    GXUPointer size = sizeof ( GXWChar ) * ( GXWcslen ( src ) + 1 ); 
    *dest = static_cast<GXWChar*> ( malloc ( size ) );
    memcpy ( *dest, src, size );
}

GXVoid GXCALL GXUTF8clone ( GXUTF8** dest, const GXUTF8* src )
{
    GXUPointer size = GXUTF8size ( src );
    *dest = static_cast<GXUTF8*> ( malloc ( size ) );
    memcpy ( *dest, src, size );
}

GXVoid GXCALL GXMbsclone ( GXMBChar** dest, const GXMBChar* src )
{
    GXUPointer size = strlen ( src ) + 1u;
    *dest = static_cast<GXMBChar*> ( malloc ( size ) );
    memcpy ( *dest, src, size );
}

GXUPointer GXCALL GXCalculateSpaceForUTF8 ( const GXWChar* str )
{
    GXUInt symbols = GXWcslen ( str );
    GXUPointer space = 1u;

    for ( GXUInt i = 0u; i < symbols; ++i )
    {
        if ( str[ i ] < 0x0080 )
        {
            // One byte per symbol
            ++space;
            continue;
        }

        if ( str[ i ] < 0x0800 )
        {
            // Two bytes per symbol
            space += 2u;
            continue;
        }

        // Three bytes per symbol
        space += 3u;
    }

    return space;
}

GXUByte GXWriteUTF8Symbol ( GXUTF8* start, GXWChar symbol )
{
    if ( symbol < 0x0080 )
    {
        *start = static_cast<GXUTF8> ( symbol );
        return 1u;
    }

    if ( symbol < 0x0800 )
    {
        start[ 0 ] = static_cast<GXUTF8> ( 0x00C0 | ( ( symbol & 0x07C0 ) >> 6 ) );
        start[ 1 ] = static_cast<GXUTF8> ( 0x0080 | ( symbol & 0x003F ) );
        return 2u;
    }

    start[ 0 ] = static_cast<GXUTF8> ( 0x00E0 | ( ( symbol & 0x0000F000 ) >> 12 ) );
    start[ 1 ] = static_cast<GXUTF8> ( 0x0080 | ( ( symbol & 0x00000FC0 ) >> 6 ) );
    start[ 2 ] = static_cast<GXUTF8> ( 0x0080 | ( symbol & 0x0000003F ) );
    return 3u;
}

GXUByte GXWriteUTF8Symbol ( GXUTF8* start, GXUInt symbol )
{
    if ( symbol < 0x00000080u )
    {
        *start = static_cast<GXUTF8> ( symbol );
        return 1u;
    }

    if ( symbol < 0x00000800u )
    {
        start[ 0 ] = static_cast<GXUTF8> ( 0x000000C0u | ( ( symbol & 0x000007C0u ) >> 6 ) );
        start[ 1 ] = static_cast<GXUTF8> ( 0x00000080u | ( symbol & 0x0000003Fu ) );
        return 2u;
    }

    if ( symbol < 0x00010000u )
    {
        start[ 0 ] = static_cast<GXUTF8> ( 0x000000E0u | ( ( symbol & 0x0000F000u ) >> 12 ) );
        start[ 1 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x00000FC0u ) >> 6 ) );
        start[ 2 ] = static_cast<GXUTF8> ( 0x00000080u | ( symbol & 0x0000003Fu ) );
        return 3u;
    }

    if ( symbol < 0x00200000u )
    {
        start[ 0 ] = static_cast<GXUTF8> ( 0x000000F0u | ( ( symbol & 0x001C0000u ) >> 18 ) );
        start[ 1 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x0003F000u ) >> 12 ) );
        start[ 2 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x00000FC0u ) >> 6 ) );
        start[ 3 ] = static_cast<GXUTF8> ( 0x00000080u | ( symbol & 0x0000003Fu ) );
        return 4u;
    }

    if ( symbol < 0x04000000u )
    {
        start[ 0 ] = static_cast<GXUTF8> ( 0x000000F8u | ( ( symbol & 0x001C0000u ) >> 24 ) );
        start[ 1 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x00FC0000u ) >> 18 ) );
        start[ 2 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x0003F000u ) >> 12 ) );
        start[ 3 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x00000FC0u ) >> 6 ) );
        start[ 4 ] = static_cast<GXUTF8> ( 0x00000080u | ( symbol & 0x0000003Fu ) );
        return 5u;
    }

    if ( symbol < 0x80000000u )
    {
        start[ 0 ] = static_cast<GXUTF8> ( 0x000000FCu | ( ( symbol & 0x40000000u ) >> 30 ) );
        start[ 1 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x3F000000u ) >> 24 ) );
        start[ 2 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x00FC0000u ) >> 18 ) );
        start[ 3 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x0003F000u ) >> 12 ) );
        start[ 4 ] = static_cast<GXUTF8> ( 0x00000080u | ( ( symbol & 0x00000FC0u ) >> 6 ) );
        start[ 5 ] = static_cast<GXUTF8> ( 0x00000080u | ( symbol & 0x0000003Fu ) );
        return 6u;
    }

    GXLogW ( L"GXWriteUTF8Symbol::Warning - Unexpected symbol (code %i)\n", symbol );
    return 0u;
}

GXUPointer GXCALL GXToUTF8 ( GXUTF8** dest, const GXWChar* str )
{
    GXUInt symbols = GXWcslen ( str );

    if ( symbols == 0u ) return 0u;

    GXUPointer space = GXCalculateSpaceForUTF8 ( str );

    *dest = static_cast<GXUTF8*> ( malloc ( space ) );
    ( *dest )[ space - 1u ] = 0;

    GXUPointer offset = 0u;

    for ( GXUInt i = 0u; i < symbols; ++i )
        offset += GXWriteUTF8Symbol ( ( *dest ) + offset, (GXUInt)str[ i ] );

    return space;
}

GXVoid GXCALL GXToWcs ( GXWChar** dest, const GXUTF8* str )
{
    if ( !str )
    {
        *dest = nullptr;
        return;
    }

    GXUTF8Parser parser ( str );
    GXUInt len = parser.GetLength ();

    *dest = static_cast<GXWChar*> ( malloc ( ( len + 1 ) * sizeof ( GXWChar ) ) );
    ( *dest )[ len ] = 0;

    for ( GXUInt i = 0u; i < len; ++i )
        ( *dest )[ i ] = static_cast<GXWChar> ( parser.GetSymbol ( i ) );
}

GXVoid GXCALL GXToEngineWcs ( GXWChar** dest, const GXMBChar* str )
{
    GXInt size = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, str, -1, *dest, 0 );
    *dest = static_cast<GXWChar*> ( malloc ( size * sizeof ( GXWChar ) ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, str, -1, *dest, size );
}

GXVoid GXCALL GXToSystemMbs ( GXMBChar** dest, const GXWChar* str )
{
    GXInt size = WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, str, -1, *dest, 0, nullptr, nullptr );
    *dest = static_cast<GXMBChar*> ( malloc ( (size_t)size ) );
    WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, str, -1, *dest, size, nullptr, nullptr );
}
