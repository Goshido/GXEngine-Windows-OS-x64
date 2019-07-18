// version 1.14

#include <GXCommon/GXFileSystem.h>


#define FAST_BUFFER_SYMBOLS     MAX_PATH

//---------------------------------------------------------------------------------------------------------------------

class GXWordExpResource final : public GXMemoryInspector
{
    public:
        GXWordExpResource ();
        ~GXWordExpResource () override;

    private:
        GXWordExpResource ( const GXWordExpResource &other ) = delete;
        GXWordExpResource& operator = ( const GXWordExpResource &other ) = delete;
};

GXWordExpResource::GXWordExpResource ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXWordExpResource" )
{
    // NOTHING
}

GXWordExpResource::~GXWordExpResource ()
{
    // NOTHING
}

static GXWordExpResource gx_common_WordExpResource;

//---------------------------------------------------------------------------------------------------------------------

GXString GXCALL GXWordExp ( GXString expression )
{
    GXWChar fastBuffer[ FAST_BUFFER_SYMBOLS ];
    const GXUPointer size = static_cast<GXUPointer> ( ExpandEnvironmentStringsW ( expression, fastBuffer, FAST_BUFFER_SYMBOLS ) );

    if ( size <= FAST_BUFFER_SYMBOLS )
        return GXString ( fastBuffer );

    GXWChar* workingBuffer = static_cast<GXWChar*> ( gx_common_WordExpResource.Malloc ( size * sizeof ( GXWChar ) ) );
    ExpandEnvironmentStringsW ( expression, workingBuffer, static_cast<DWORD> ( size ) );
    GXString result ( workingBuffer );

    gx_common_WordExpResource.Free ( workingBuffer );
    return result;
}
