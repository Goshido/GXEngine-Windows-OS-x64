//version 1.2

#ifndef GX_STRINGS
#define GX_STRINGS


#include "GXCommon.h"


class GXUTF8Parser
{
	private:
		const GXUTF8*	string;
		GXUInt			totalSymbols;

	public:
		GXUTF8Parser ( const GXUTF8* string );
		~GXUTF8Parser ();

		GXUInt GetSymbol ( GXUInt position );
		GXUInt GetLength ();

		GXVoid Copy ( GXUTF8* dest, GXUInt startPos, GXUInt endPos );
		GXUInt GetOffset ( GXUInt position );

		GXVoid Debug ();
};

//--------------------------------------------------------------

enum eGXEndianness
{
	GX_BIG_ENDIAN,
	GX_LITTLE_ENDIAN,
	GX_PDP_ENDIAN,
	GX_ERROR_ENDIAN
};

eGXEndianness GXCALL GXDetermineEndianness ();

//--------------------------------------------------------------

GXUInt GXCALL GXWcslen ( const GXWChar* str );
GXUInt GXCALL GXUTF8len ( const GXUTF8* str );

GXInt GXCALL GXWcscmp ( const GXWChar* a, const GXWChar* b );
GXInt GXCALL GXUTF8cmp ( const GXUTF8* a, const GXUTF8* b );
GXInt GXCALL GXMbscmp ( const GXMBChar* a, const GXMBChar* b );

GXUInt GXCALL GXUTF8size ( const GXUTF8* str );

GXVoid GXCALL GXWcsclone ( GXWChar** dest, const GXWChar* src );
GXVoid GXCALL GXUTF8clone ( GXUTF8** dest, const GXUTF8* src );
GXVoid GXCALL GXMbsclone ( GXMBChar** dest, const GXMBChar* src );

GXUInt GXCALL GXToUTF8 ( GXUTF8** dest, const GXWChar* str );
GXVoid GXCALL GXToWcs ( GXWChar** dest, const GXUTF8* str );
GXVoid GXCALL GXToEngineWcs ( GXWChar** dest, const GXMBChar* str );
GXVoid GXCALL GXToSystemMbs ( GXMBChar** dest, const GXWChar* str );


#endif //GX_STRINGS
