// version 1.7

#ifndef GX_STRINGS
#define GX_STRINGS


#include "GXMemory.h"


class GXUTF8Parser final
{
    private:
        const GXUTF8*       string;
        GXUInt              totalSymbols;

    public:
        explicit GXUTF8Parser ( const GXUTF8* string );
        ~GXUTF8Parser ();

        GXUInt GetSymbol ( GXUInt position );
        GXUInt GetLength ();

        GXVoid Copy ( GXUTF8* dest, GXUInt startPos, GXUInt endPos );
        GXUInt GetOffset ( GXUInt position );

        GXVoid Debug ();

    private:
        GXUTF8Parser ( const GXUTF8Parser &other ) = delete;
        GXUTF8Parser& operator = ( const GXUTF8Parser &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

enum eGXEndianness : GXUByte
{
    BigEndian,
    LittleEndian,
    PDPEndian,
    ErrorEndian
};

eGXEndianness GXCALL GXDetermineEndianness ();

//---------------------------------------------------------------------------------------------------------------------

GXUInt GXCALL GXWcslen ( const GXWChar* str );
GXUInt GXCALL GXUTF8len ( const GXUTF8* str );

GXInt GXCALL GXWcscmp ( const GXWChar* a, const GXWChar* b );
GXInt GXCALL GXUTF8cmp ( const GXUTF8* a, const GXUTF8* b );
GXInt GXCALL GXMbscmp ( const GXMBChar* a, const GXMBChar* b );

GXUPointer GXCALL GXUTF8size ( const GXUTF8* str );

// Note string will be cloned via malloc allocation. User code MUST call free
// when string content is no needed anymore.

GXVoid GXCALL GXMbsclone ( GXMBChar** dest, const GXMBChar* src );
GXVoid GXCALL GXCstrClone ( GXChar** dest, const GXChar* src );
GXVoid GXCALL GXUTF8clone ( GXUTF8** dest, const GXUTF8* src );
GXVoid GXCALL GXWcsclone ( GXWChar** dest, const GXWChar* src );

GXUPointer GXCALL GXToUTF8 ( GXUTF8** dest, const GXWChar* str );
GXVoid GXCALL GXToWcs ( GXWChar** dest, const GXUTF8* str );
GXVoid GXCALL GXToEngineWcs ( GXWChar** dest, const GXMBChar* str );
GXVoid GXCALL GXToSystemMbs ( GXMBChar** dest, const GXWChar* str );

//---------------------------------------------------------------------------------------------------------------------

// This is crosscompile, no thread safe but reentrant, lock free string. Note use GXString objects by value.
// GXString has size of GXUPointer bytes. GXString implements copy on write (COW) architecture.

class GXStringData;
class GXString final: public GXMemoryInspector
{
    private:
        GXStringData*       stringData;

    public:
        GXString ();
        explicit GXString ( const GXString &string );
        explicit GXString ( const GXMBChar* string );
        explicit GXString ( GXMBChar character );
        explicit GXString ( const GXWChar* string );
        explicit GXString ( GXWChar character );
        ~GXString () override;

        GXVoid Clear ();

        // Note return symbol count without null terminator.
        const GXUPointer GetSymbolCount () const;

        const GXBool IsEmpty () const;

        GXVoid FromSystemMultibyteString ( const GXMBChar* string );
        const GXMBChar* ToSystemMultibyteString ();
        const GXWChar* ToSystemMultibyteString ( GXUPointer &stringSize );

        GXVoid FromSystemWideString ( const GXWChar* string );
        const GXWChar* ToSystemWideString ();
        const GXWChar* ToSystemWideString ( GXUPointer &stringSize );

        GXVoid FromUTF8 ( const GXUTF8* string );
        const GXUTF8* ToUTF8 ();
        const GXUTF8* ToUTF8 ( GXUPointer &stringSize );

        GXString& operator = ( const GXString &other );
        GXString& operator = ( const GXMBChar* string );
        GXString& operator = ( const GXMBChar character );
        GXString& operator = ( const GXWChar* string );
        GXString& operator = ( const GXWChar character );

        GXString operator + ( const GXString other );
        GXString operator + ( const GXMBChar* string );
        GXString operator + ( const GXMBChar character );
        GXString operator + ( const GXWChar* string );
        GXString operator + ( const GXWChar character );

        GXString& operator += ( const GXString other );
        GXString& operator += ( const GXMBChar* string );
        GXString& operator += ( const GXMBChar character );
        GXString& operator += ( const GXWChar* string );
        GXString& operator += ( const GXWChar character );

        GXBool operator > ( const GXString other ) const;
        GXBool operator > ( const GXMBChar* string ) const;
        GXBool operator > ( const GXMBChar character ) const;
        GXBool operator > ( const GXWChar* string ) const;
        GXBool operator > ( const GXWChar character ) const;

        GXBool operator < ( const GXString other ) const;
        GXBool operator < ( const GXMBChar* string ) const;
        GXBool operator < ( const GXMBChar character ) const;
        GXBool operator < ( const GXWChar* string ) const;
        GXBool operator < ( const GXWChar character ) const;

        GXBool operator == ( const GXString other ) const;
        GXBool operator == ( const GXMBChar* string ) const;
        GXBool operator == ( const GXMBChar character ) const;
        GXBool operator == ( const GXWChar* string ) const;
        GXBool operator == ( const GXWChar character ) const;
};


#endif // GX_STRINGS
