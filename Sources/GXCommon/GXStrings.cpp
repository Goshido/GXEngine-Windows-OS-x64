//version 1.2

#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>


GXUTF8Parser::GXUTF8Parser ( const GXUTF8* string )
{
	this->string = string;
	totalSymbols = GXUTF8len ( string );
}

GXUTF8Parser::~GXUTF8Parser ()
{
	//NOTHING
}

GXUInt GXUTF8Parser::GetSymbol ( GXUInt position )
{
	if ( position >= totalSymbols ) return 0xFFFFFFFF;

	GXUInt offset = GetOffset ( position );

	if ( ( string[ offset ] & 0x80 ) == 0x00 )
		return (GXUInt)string[ offset ];

	if ( ( string[ offset ] & 0xE0 ) == 0xC0 )
	{
		return	( ( ( (GXUInt)string[ offset ] ) & 0x0000001F ) << 6 ) |
				( (GXUInt)( string[ offset + 1 ] ) & 0x0000003F );
	}

	if ( ( string[ offset ] & 0xF0 ) == 0xE0 )
	{
		return	( ( (GXUInt)string[ offset ] & 0x0000000F ) << 12 ) |
				( ( (GXUInt)string[ offset + 1] & 0x0000003F ) << 6 ) |
				( (GXUInt)string[ offset + 2 ] & 0x0000003F );
	}

	if ( ( string[ offset ] & 0xF8 ) == 0xF0 )
	{
		return	( ( (GXUInt)string[ offset ] & 0x00000007 ) << 18 ) |
				( ( (GXUInt)string[ offset + 1 ] & 0x0000003F ) << 12 ) |
				( ( (GXUInt)string[ offset + 2 ] & 0x0000003F ) << 6 ) |
				( (GXUInt)string[ offset + 3 ] & 0x0000003F );
	}

	if ( ( string[ offset ] & 0xFC ) == 0xF8 )
	{
		return	( ( (GXUInt)string[ offset ] & 0x00000003 ) << 24 ) |
				( ( (GXUInt)string[ offset + 1 ] & 0x0000003F ) << 18 ) |
				( ( (GXUInt)string[ offset + 2 ] & 0x0000003F ) << 12 ) |
				( ( (GXUInt)string[ offset + 3 ] & 0x0000003F ) << 6 ) |
				( (GXUInt)string[ offset + 4 ] & 0x0000003F );
	}

	if ( ( string[ offset ] & 0xFE ) == 0xFC )
	{
		return	( ( (GXUInt)string[ offset ] & 0x00000001 ) << 30 ) |
				( ( (GXUInt)string[ offset + 1 ] & 0x0000003F ) << 24 ) |
				( ( (GXUInt)string[ offset + 2 ] & 0x0000003F ) << 18 ) |
				( ( (GXUInt)string[ offset + 3 ] & 0x0000003F ) << 12 ) |
				( ( (GXUInt)string[ offset + 4 ] & 0x0000003F ) << 6 ) |
				( (GXUInt)string[ offset + 5 ] & 0x0000003F );
	}

	GXLogA ( "GXUTF8Parser::GetSymbol::Error - Can't parse symbol [position %i]\n", position );
	return 0xFFFFFFFF;
}

GXUInt GXUTF8Parser::GetLength ()
{
	return totalSymbols;
}

GXVoid GXUTF8Parser::Copy ( GXUTF8* dest, GXUInt startPos, GXUInt endPos )
{
	if ( endPos >= totalSymbols ) return;

	GXUInt offset = 0;
	GXUInt baseOffset = GetOffset ( startPos );

	for ( GXUInt i = startPos; i <= endPos; i++ )
	{
		GXUInt ind = baseOffset + offset;

		if ( ( string[ ind ] & 0x80 ) == 0x00 )
		{
			dest[ offset ] = string[ ind ];
			offset++;

			continue;
		}

		if ( ( string[ ind ] & 0xE0 ) == 0xC0 )
		{
			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;

			continue;
		}

		if ( ( string[ ind ] & 0xF0 ) == 0xE0 )
		{
			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;

			continue;
		}

		if ( ( string[ ind ] & 0xF8 ) == 0xF0 )
		{
			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;

			continue;
		}

		if ( ( string[ ind ] & 0xFC ) == 0xF8 )
		{
			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;

			continue;
		}

		if ( ( string[ ind ] & 0xFE ) == 0xFC )
		{
			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
			ind++;

			dest[ offset ] = string[ ind ];
			offset++;
		}
	}
}

GXUInt GXUTF8Parser::GetOffset ( GXUInt position )
{
	GXUInt offset = 0;
	GXUInt i = 0;

	if ( position > 0 )
	{
		while ( GX_TRUE )
		{
			if ( ( string[ offset ] & 0xC0 ) != 0x80 )
			{
				i++;
				offset++;

				if ( i < position ) continue;

				while ( GX_TRUE )
				{
					if ( ( string[ offset ] & 0xC0 ) != 0x80 )
						break;

					offset++;
				}

				break;
			}

			offset++;
		}
	}

	return offset;
}

GXVoid GXUTF8Parser::Debug ()
{
	for ( GXUInt i = 0; string[ i ]; i++ )
	{
		GXUByte bit0 = string[ i ] & 0x01;
		GXUByte bit1 = ( string[ i ] & 0x02 ) >> 1;
		GXUByte bit2 = ( string[ i ] & 0x04 ) >> 2;
		GXUByte bit3 = ( string[ i ] & 0x08 ) >> 3;
		GXUByte bit4 = ( string[ i ] & 0x10 ) >> 4;
		GXUByte bit5 = ( string[ i ] & 0x20 ) >> 5;
		GXUByte bit6 = ( string[ i ] & 0x40 ) >> 6;
		GXUByte bit7 = ( string[ i ] & 0x80 ) >> 7;

		GXLogA ( "GXUTF8Parser::Debug::Info - %i%i%i%i %i%i%i%i\n", bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0 );
	}
}

//--------------------------------------------------------------

eGXEndianness GXCALL GXDetermineEndianness ()
{
	static const GXUByte magic[ 4 ] = { 0xA1, 0xB2, 0xC3, 0xD4 };

	GXInt test = *( (const GXInt*)magic );

	switch ( test )
	{
		case 0xA1B2C3D4:
		return GX_BIG_ENDIAN;

		case 0xD4C3B2A1:
		return GX_LITTLE_ENDIAN;

		case 0xB2A1D4C3:
		return GX_PDP_ENDIAN;

		default:
		return GX_ERROR_ENDIAN;
	}
}

//--------------------------------------------------------------


GXUInt GXCALL GXWcslen ( const GXWChar* str )
{
	if ( !str ) return 0;

	return (GXUInt)wcslen ( str );
}

GXUInt GXCALL GXUTF8len ( const GXUTF8* str )
{
	if ( !str ) return 0;

	GXUInt numBytes = (GXUInt)strlen ( str );
	if ( numBytes == 0 ) return 0;

	GXUInt numSymbols = 0;
	for ( GXUInt i = 0; i < numBytes; i++ )
		if ( ( str[ i ] & 0xC0 ) != 0x80 )
			numSymbols++;

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

GXUInt GXCALL GXUTF8size ( const GXUTF8* str )
{
	GXUInt len;
	for ( len = 0; str[ len ]; len++ );

	return len + 1;
}

GXVoid GXCALL GXWcsclone ( GXWChar** dest, const GXWChar* src )
{
	if ( !src )
	{
		*dest = nullptr;
		return;
	}

	GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( src ) + 1 ); 
	*dest = (GXWChar*)malloc ( size );
	memcpy ( *dest, src, size );
}

GXVoid GXCALL GXUTF8clone ( GXUTF8** dest, const GXUTF8* src )
{
	GXUInt size = GXUTF8size ( src );
	*dest = (GXUTF8*)malloc ( size );
	memcpy ( *dest, src, size );
}

GXVoid GXCALL GXMbsclone ( GXMBChar** dest, const GXMBChar* src )
{
	GXUInt size = (GXUInt)( strlen ( src ) + 1 );
	*dest = (GXMBChar*)malloc ( size );
	memcpy ( *dest, src, size );
}

GXUInt GXCALL GXCalculateSpaceForUTF8 ( const GXWChar* str )
{
	GXUInt symbols = GXWcslen ( str );

	GXUInt space = 1;

	for ( GXUInt i = 0; i < symbols; i++ )
	{
		if ( str[ i ] < 0x0080 )
		{
			//One byte per symbol
			space++;
			continue;
		}

		if ( str[ i ] < 0x0800 )
		{
			//Two bytes per symbol
			space += 2;
			continue;
		}

		//Three bytes per symbol
		space += 3;
	}

	return space;
}

GXUByte GXWriteUTF8Symbol ( GXUTF8* start, GXWChar symbol )
{
	if ( symbol < 0x0080 )
	{
		*start = (GXUTF8)symbol;
		return 1;
	}

	if ( symbol < 0x0800 )
	{
		start[ 0 ] = (GXUTF8)0xC0 | (GXUTF8)( ( symbol & 0x07C0 ) >> 6 );
		start[ 1 ] = (GXUTF8)0x80 | (GXUTF8)( symbol & 0x003F );
		return 2;
	}

	start[ 0 ] = (GXUTF8)0xE0 | (GXUTF8)( ( symbol & 0x0000F000 ) >> 12 );
	start[ 1 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x00000FC0 ) >> 6 );
	start[ 2 ] = (GXUTF8)0x80 | (GXUTF8)( symbol & 0x0000003F );
	return 3;
}

GXUByte GXWriteUTF8Symbol ( GXUTF8* start, GXUInt symbol )
{
	if ( symbol < 0x00000080 )
	{
		*start = (GXUTF8)symbol;
		return 1;
	}

	if ( symbol < 0x00000800 )
	{
		start[ 0 ] = (GXUTF8)0xC0 | (GXUTF8)( ( symbol & 0x000007C0 ) >> 6 );
		start[ 1 ] = (GXUTF8)0x80 | (GXUTF8)( symbol & 0x0000003F );
		return 2;
	}

	if ( symbol < 0x00010000 )
	{
		start[ 0 ] = (GXUTF8)0xE0 | (GXUTF8)( ( symbol & 0x0000F000 ) >> 12 );
		start[ 1 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x00000FC0 ) >> 6 );
		start[ 2 ] = (GXUTF8)0x80 | (GXUTF8)( symbol & 0x0000003F );
		return 3;
	}

	if ( symbol < 0x00200000 )
	{
		start[ 0 ] = (GXUTF8)0xF0 | (GXUTF8)( ( symbol & 0x001C0000 ) >> 18 );
		start[ 1 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x0003F000 ) >> 12 );
		start[ 2 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x00000FC0 ) >> 6 );
		start[ 3 ] = (GXUTF8)0x80 | (GXUTF8)( symbol & 0x0000003F );
		return 4;
	}

	if ( symbol < 0x04000000 )
	{
		start[ 0 ] = (GXUTF8)0xF8 | (GXUTF8)( ( symbol & 0x001C0000 ) >> 24 );
		start[ 1 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x00FC0000 ) >> 18 );
		start[ 2 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x0003F000 ) >> 12 );
		start[ 3 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x00000FC0 ) >> 6 );
		start[ 4 ] = (GXUTF8)0x80 | (GXUTF8)( symbol & 0x0000003F );
		return 5;
	}

	if ( symbol < 0x80000000 )
	{
		start[ 0 ] = (GXUTF8)0xFC | (GXUTF8)( ( symbol & 0x40000000 ) >> 30 );
		start[ 1 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x3F000000 ) >> 24 );
		start[ 2 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x00FC0000 ) >> 18 );
		start[ 3 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x0003F000 ) >> 12 );
		start[ 4 ] = (GXUTF8)0x80 | (GXUTF8)( ( symbol & 0x00000FC0 ) >> 6 );
		start[ 5 ] = (GXUTF8)0x80 | (GXUTF8)( symbol & 0x0000003F );
		return 6;
	}

	GXLogA ( "GXWriteUTF8Symbol::Warning - Unexpected symbol (code %i)\n", symbol );
	return 0;
}

GXUInt GXCALL GXToUTF8 ( GXUTF8** dest, const GXWChar* str )
{
	GXUInt symbols = GXWcslen ( str );
	if ( symbols == 0 ) return 0;

	GXUInt space = GXCalculateSpaceForUTF8 ( str );

	*dest = (GXUTF8*)malloc ( space );
	( *dest )[ space - 1 ] = 0;

	GXUInt offset = 0;
	for ( GXUInt i = 0; i < symbols; i++ )
		offset += GXWriteUTF8Symbol ( ( *dest ) + offset, (GXUInt)str[ i ] );

	return space;
}

GXVoid GXCALL GXToWcs ( GXWChar** dest, const GXUTF8* str )
{
	GXUTF8Parser parser ( str );
	GXUInt len = parser.GetLength ();

	*dest = (GXWChar*)malloc ( ( len + 1 ) * sizeof ( GXWChar ) );
	( *dest )[ len ] = 0;

	for ( GXUInt i = 0; i < len; i++ )
		( *dest )[ i ] = parser.GetSymbol ( i );
}

GXVoid GXCALL GXToEngineWcs ( GXWChar** dest, const GXMBChar* str )
{
	GXUInt size = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, str, -1, *dest, 0 );
	*dest = (GXWChar*)malloc ( size * sizeof ( GXWChar ) );
	MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, str, -1, *dest, size );
}

GXVoid GXCALL GXToSystemMbs ( GXMBChar** dest, const GXWChar* str )
{
	GXUInt size = WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, str, -1, *dest, 0, 0, 0 );
	*dest = (GXMBChar*)malloc ( size );
	WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK, str, -1, *dest, size, 0, 0 );
}
