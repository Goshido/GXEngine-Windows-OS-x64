// version 1.0

#include <GXCommon/GXStrings.h>
#include <GXCommon/GXUIntAtomic.h>


#define EXTRA_SPACE_SYMBOLS     32u

//---------------------------------------------------------------------------------------------------------------------

class GXStringData final : public GXMemoryInspector
{
    private:
        GXUPointerAtomic        references;

        GXUTF16*                buffer;
        GXUPointer              bufferSize;
        GXUPointer              allocatedBufferSize;
        GXUPointer              symbols;

    public:
        explicit GXStringData ( const GXUTF16* content );

        GXVoid AddReference ();
        GXVoid Release ();

        GXBool IsShared ();

        const GXUTF16* GetData ( GXUPointer &size ) const;
        const GXUPointer GetSymbolCount () const;

        GXVoid UpdateData ( const GXUTF16* content );

        static GXStringData& GXCALL GetEmptyStringData ();

    private:
        ~GXStringData () override;

        GXStringData () = delete;
        GXStringData ( const GXStringData &other ) = delete;
        GXStringData& operator = ( const GXStringData &other ) = delete;
};

GXStringData::GXStringData ( const GXUTF16* content )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXStringData" )
    references ( 1u ),
    buffer ( nullptr ),
    bufferSize ( 0u ),
    allocatedBufferSize ( 0u ),
    symbols ( 0u )
{
    UpdateData ( content );
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

GXBool GXStringData::IsShared ()
{
    return references > static_cast<GXUPointer> ( 1u );
}

const GXUTF16* GXStringData::GetData ( GXUPointer &size ) const
{
    size = bufferSize;
    return buffer;
}

const GXUPointer GXStringData::GetSymbolCount () const
{
    return symbols;
}

GXVoid GXStringData::UpdateData ( const GXUTF16* content )
{
    if ( !content )
    {
        bufferSize = 0u;
        symbols = 0u;
        return;
    }

    symbols = static_cast<const GXUPointer> ( GXWcslen ( reinterpret_cast<const GXWChar*> ( content ) ) + 1u );
    bufferSize = symbols * sizeof ( GXUTF16 );

    if ( bufferSize > allocatedBufferSize )
    {
        SafeFree ( reinterpret_cast<GXVoid**> ( &buffer ) );
        allocatedBufferSize = bufferSize + EXTRA_SPACE_SYMBOLS * sizeof ( GXUTF16 );
        buffer = static_cast<GXUTF16*> ( Malloc ( allocatedBufferSize ) );
    }

    memcpy ( buffer, content, bufferSize );
}

GXStringData::~GXStringData ()
{
    SafeFree ( reinterpret_cast<GXVoid**> ( &buffer ) );
}

GXStringData& GXCALL GXStringData::GetEmptyStringData ()
{
    static GXStringData emptyStringData ( nullptr );
    emptyStringData.AddReference ();
    return emptyStringData;
}

//---------------------------------------------------------------------------------------------------------------------

GXString::GXString ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" ),
    stringData ( &( GXStringData::GetEmptyStringData () ) )
{
    // NOTHING
}

GXString::GXString ( const GXString &other )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    other.stringData->AddReference ();
    stringData = other.stringData;
}

GXString::GXString ( const GXMBChar* string )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    if ( !string )
    {
        stringData = &( GXStringData::GetEmptyStringData () );
        return;
    }

    GXWChar* utf16String = nullptr;

    const GXInt symbols = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    utf16String = static_cast<GXWChar*> ( Malloc ( symbols * sizeof ( GXWChar ) ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( utf16String ) );

    Free ( utf16String );
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
    stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( utf16String ) );

    Free ( utf16String );
}

GXString::GXString ( const GXWChar* string )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    if ( !string )
    {
        stringData = &( GXStringData::GetEmptyStringData () );
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( string ) );
}

GXString::GXString ( GXWChar character )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXString" )
{
    const GXUShort string[ 2u ] = { static_cast<GXUShort> ( character ), 0u };
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    stringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( string ) );
}

GXString::~GXString ()
{
    stringData->Release ();
}

GXVoid GXString::Clear ()
{
    stringData->Release ();
    stringData = &( GXStringData::GetEmptyStringData () );
}

const GXUPointer GXString::GetSymbolCount () const
{
    return stringData->GetSymbolCount ();
}

const GXBool GXString::IsEmpty () const
{
    return stringData->GetSymbolCount () < 1u;
}

GXVoid GXString::FromSystemMultibyteString ( const GXMBChar* string )
{
    GXWChar* utf16String = nullptr;

    const GXInt symbols = MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    utf16String = static_cast<GXWChar*> ( Malloc ( symbols * sizeof ( GXWChar ) ) );
    MultiByteToWideChar ( CP_ACP, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    if ( !stringData->IsShared () )
    {
        stringData->UpdateData ( reinterpret_cast<const GXUTF16*> ( utf16String ) );
        Free ( utf16String );
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    GXStringData* newStringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( utf16String ) );

    stringData->Release ();
    Free ( utf16String );
    stringData = newStringData;
}

const GXMBChar* GXString::ToSystemMultibyteString ()
{
    // TODO
    return nullptr;
}

const GXWChar* GXString::ToSystemMultibyteString ( GXUPointer& /*stringSize*/ )
{
    // TODO
    return nullptr;
}

GXVoid GXString::FromSystemWideString ( const GXWChar* string )
{
    if ( !stringData->IsShared () )
    {
        stringData->UpdateData ( reinterpret_cast<const GXUTF16*> ( string ) );
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    GXStringData* newStringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( string ) );

    stringData->Release ();
    stringData = newStringData;
}

const GXWChar* GXString::ToSystemWideString ()
{
    // TODO
    return nullptr;
}

const GXWChar* GXString::ToSystemWideString ( GXUPointer& /*stringSize*/ )
{
    // TODO
    return nullptr;
}

GXVoid GXString::FromUTF8 ( const GXUTF8* string )
{
    GXWChar* utf16String = nullptr;

    const GXInt symbols = MultiByteToWideChar ( CP_UTF8, MB_PRECOMPOSED, string, -1, utf16String, 0 );
    utf16String = static_cast<GXWChar*> ( Malloc ( symbols * sizeof ( GXWChar ) ) );
    MultiByteToWideChar ( CP_UTF8, MB_PRECOMPOSED, string, -1, utf16String, symbols );

    if ( !stringData->IsShared () )
    {
        stringData->UpdateData ( reinterpret_cast<const GXUTF16*> ( utf16String ) );
        Free ( utf16String );
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringData" );
    GXStringData* newStringData = new GXStringData ( reinterpret_cast<const GXUTF16*> ( utf16String ) );

    Free ( utf16String );
    stringData->Release ();
    stringData = newStringData;
}

const GXUTF8* GXString::ToUTF8 ()
{
    // TODO
    return nullptr;
}

const GXUTF8* GXString::ToUTF8 ( GXUPointer& /*stringSize*/ )
{
    // TODO
    return nullptr;
}

GXString& GXString::operator = ( const GXString &other )
{
    stringData->Release ();
    other.stringData->AddReference ();
    stringData = other.stringData;
    return *this;
}

GXString& GXString::operator = ( const GXMBChar* /*string*/ )
{
    // TODO
    return *this;
}

GXString& GXString::operator = ( const GXMBChar /*character*/ )
{
    // TODO
    return *this;
}

GXString& GXString::operator = ( const GXWChar* /*string*/ )
{
    // TODO
    return *this;
}

GXString& GXString::operator = ( const GXWChar /*character*/ )
{
    // TODO
    return *this;
}

GXString GXString::operator + ( const GXString /*other*/ )
{
    // TODO
    return {};
}

GXString GXString::operator + ( const GXMBChar* /*string*/ )
{
    // TODO
    return {};
}

GXString GXString::operator + ( const GXMBChar /*character*/ )
{
    // TODO
    return {};
}

GXString GXString::operator + ( const GXWChar* /*string*/ )
{
    // TODO
    return {};
}

GXString GXString::operator + ( const GXWChar /*character*/ )
{
    // TODO
    return {};
}

GXString& GXString::operator += ( const GXString /*other*/ )
{
    // TODO
    return *this;
}

GXString& GXString::operator += ( const GXMBChar* /*string*/ )
{
    // TODO
    return *this;
}

GXString& GXString::operator += ( const GXMBChar /*character*/ )
{
    // TODO
    return *this;
}

GXString& GXString::operator += ( const GXWChar* /*string*/ )
{
    // TODO
    return *this;
}

GXString& GXString::operator += ( const GXWChar /*character*/ )
{
    // TODO
    return *this;
}

GXBool GXString::operator > ( const GXString /*other*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator > ( const GXMBChar* /*string*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator > ( const GXMBChar /*character*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator > ( const GXWChar* /*string*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator > ( const GXWChar /*character*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator < ( const GXString /*other*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator < ( const GXMBChar* /*string*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator < ( const GXMBChar /*character*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator < ( const GXWChar* /*string*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator < ( const GXWChar /*character*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator == ( const GXString /*other*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator == ( const GXMBChar* /*string*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator == ( const GXMBChar /*character*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator == ( const GXWChar* /*string*/ ) const
{
    // TODO
    return GX_FALSE;
}

GXBool GXString::operator == ( const GXWChar /*character*/ ) const
{
    // TODO
    return GX_FALSE;
}
