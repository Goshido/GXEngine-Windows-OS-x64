// version 1.12

#ifndef GX_STRINGS
#define GX_STRINGS


#include "GXMemory.h"


class GXUTF8Parser final
{
    private:
        const GXUTF8*       _string;
        GXUPointer          _totalSymbols;

    public:
        explicit GXUTF8Parser ( const GXUTF8* string );
        ~GXUTF8Parser ();

        GXUInt GetSymbol ( GXUPointer position );
        GXUPointer GetLength ();

        GXVoid Copy ( GXUTF8* dest, GXUPointer startPos, GXUPointer endPos );
        GXUPointer GetOffset ( GXUPointer position );

        GXVoid Debug ();

    private:
        GXUTF8Parser ( const GXUTF8Parser &other ) = delete;
        GXUTF8Parser& operator = ( const GXUTF8Parser &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

enum class eGXEndianness : GXUByte
{
    BigEndian,
    LittleEndian,
    PDPEndian,
    ErrorEndian
};

eGXEndianness GXCALL GXDetermineEndianness ();

//---------------------------------------------------------------------------------------------------------------------

GXUPointer GXCALL GXWcslen ( const GXWChar* str );
GXUPointer GXCALL GXUTF8len ( const GXUTF8* str );

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

class GXString;
class GXStringSymbol final
{
    friend class GXString;

    private:
        GXUTF16     _lead;
        GXUTF16     _trailing;

    public:
        GXStringSymbol ( const GXStringSymbol &other );
        GXStringSymbol ( const GXChar symbol );
        GXStringSymbol ( const GXWChar symbol );
        ~GXStringSymbol ();

        // Method returns Unicode code point.
        GXUInt ToCodePoint () const;

        GXStringSymbol& operator = ( const GXStringSymbol &other );
        GXStringSymbol& operator = ( const GXChar symbol );
        GXStringSymbol& operator = ( const GXWChar symbol );

        GXBool operator == ( const GXStringSymbol &other ) const;
        GXBool operator == ( GXChar symbol ) const;
        GXBool operator == ( GXWChar symbol ) const;

        GXBool operator != ( const GXStringSymbol &other ) const;
        GXBool operator != ( GXChar symbol ) const;
        GXBool operator != ( GXWChar symbol ) const;

    private:
        explicit GXStringSymbol ( GXUTF16 trivial );
        explicit GXStringSymbol ( GXUTF16 lead, GXUTF16 trailing );
};

//---------------------------------------------------------------------------------------------------------------------

// This is crosscompile, no thread safe but reentrant, lock free string. Note use GXString objects by value.
// GXString object has size of GXUPointer bytes. GXString implements copy on write (COW) architecture.
// Note static allocation is not supported for now. Use standart string literal instead.

class GXStringData;
class GXString final : public GXMemoryInspector
{
    private:
        GXStringData*       _stringData;

    public:
        GXString ();
        GXString ( const GXString &string );
        GXString ( const GXMBChar* string );
        GXString ( GXMBChar character );
        GXString ( const GXWChar* string );
        GXString ( GXWChar character );
        GXString ( GXUTF16* string );
        ~GXString () override;

        GXVoid Clear ();

        GXVoid GXCDECLCALL Format ( const GXMBChar* format, ... );

        // Note method returns symbol count without null terminator.
        const GXUPointer GetSymbolCount () const;

        const GXBool IsEmpty () const;
        const GXBool IsNull () const;

        GXString GetLeft ( GXUPointer lastSymbolIndex ) const;
        GXString GetMiddle ( GXUPointer firstSymbolIndex, GXUPointer lastSymbolIndex ) const;
        GXString GetRight ( GXUPointer firstSymbolIndex ) const;

        GXVoid FromSystemMultibyteString ( const GXMBChar* string );
        const GXMBChar* ToSystemMultibyteString ();
        const GXMBChar* ToSystemMultibyteString ( GXUPointer &stringSize );

        GXVoid FromSystemWideString ( const GXWChar* string );
        const GXWChar* ToSystemWideString ();
        const GXWChar* ToSystemWideString ( GXUPointer &stringSize );

        GXVoid FromUTF8 ( const GXUTF8* string );
        const GXUTF8* ToUTF8 ();
        const GXUTF8* ToUTF8 ( GXUPointer &stringSize );

        GXVoid FromUTF16 ( const GXUTF16* string );
        const GXUTF16* ToUTF16 () const;
        const GXUTF16* ToUTF16 ( GXUPointer &stringSize ) const;

        GXVoid FromSymbols ( const GXStringSymbol* symbols );

        // Method make string copy in heap memory via GXMemoryInspector::Malloc allocation.
        // Caller MUST invoke GXMemoryInspector::Free manually.
        // Note method will return nullptr if string is null.
        GXStringSymbol* ToSymbols () const;

        // "maxSymbolCount" is maximum "buffer" capacity in symbols.
        // Note no any heap allocation will be done.
        // Method returns GX_TRUE is success. Otherwise method returns GX_FALSE.
        GXBool ToSymbols ( GXStringSymbol*& buffer, GXUPointer maxSymbolCount ) const;

        eGXCompareResult Compare ( const GXString other ) const;
        eGXCompareResult Compare ( const GXMBChar* string ) const;
        eGXCompareResult Compare ( const GXMBChar character ) const;
        eGXCompareResult Compare ( const GXWChar* string ) const;
        eGXCompareResult Compare ( GXWChar character ) const;

        GXString& operator = ( const GXString &other );
        GXString& operator = ( const GXMBChar* string );
        GXString& operator = ( GXMBChar character );
        GXString& operator = ( const GXWChar* string );
        GXString& operator = ( GXWChar character );

        GXString operator + ( const GXString &other );
        GXString operator + ( const GXMBChar* string );
        GXString operator + ( GXMBChar character );
        GXString operator + ( const GXWChar* string );
        GXString operator + ( GXWChar character );

        GXString& operator += ( const GXString &other );
        GXString& operator += ( const GXMBChar* string );
        GXString& operator += ( GXMBChar character );
        GXString& operator += ( const GXWChar* string );
        GXString& operator += ( GXWChar character );

        GXBool operator > ( const GXString &other ) const;
        GXBool operator > ( const GXMBChar* string ) const;
        GXBool operator > ( GXMBChar character ) const;
        GXBool operator > ( const GXWChar* string ) const;
        GXBool operator > ( GXWChar character ) const;

        GXBool operator < ( const GXString &other ) const;
        GXBool operator < ( const GXMBChar* string ) const;
        GXBool operator < ( GXMBChar character ) const;
        GXBool operator < ( const GXWChar* string ) const;
        GXBool operator < ( GXWChar character ) const;

        GXBool operator == ( const GXString &other ) const;
        GXBool operator == ( const GXMBChar* string ) const;
        GXBool operator == ( GXMBChar character ) const;
        GXBool operator == ( const GXWChar* string ) const;
        GXBool operator == ( GXWChar character ) const;

        GXBool operator != ( const GXString &other ) const;
        GXBool operator != ( const GXMBChar* string ) const;
        GXBool operator != ( GXMBChar character ) const;
        GXBool operator != ( const GXWChar* string ) const;
        GXBool operator != ( GXWChar character ) const;

        // Note this function will traverse string data from start to target index each time by design.
        // Use iterator stuff for more performance string analysis.
        GXStringSymbol operator [] ( GXUPointer symbolIndex ) const;

        operator const GXMBChar* () const;
        operator const GXUTF16* () const;
        operator const GXWChar* () const;

    private:
        // Special constructor.
        explicit GXString ( const GXUTF16* content, GXBool canOwnContent );

        GXString Append ( const GXUTF16* buffer, GXUPointer bufferSize, GXBool canOwnContent );
};


#endif // GX_STRINGS
