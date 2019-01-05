// version 1.6

#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXFileSystem.h>


class GXImageLoaderMemoryInspector final : public GXMemoryInspector
{
    public:
        GXImageLoaderMemoryInspector ();
        ~GXImageLoaderMemoryInspector () override;

    private:
        GXImageLoaderMemoryInspector ( const GXImageLoaderMemoryInspector &other );
        GXImageLoaderMemoryInspector& operator = ( const GXImageLoaderMemoryInspector &other );
};

GXImageLoaderMemoryInspector::GXImageLoaderMemoryInspector ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXImageLoaderMemoryInspector" )
{
    // NOTHING
}

GXImageLoaderMemoryInspector::~GXImageLoaderMemoryInspector ()
{
    // NOTHING
}

static GXImageLoaderMemoryInspector gx_ImageLoaderMemoryInspector;

//---------------------------------------------------------------------------------------------------------------------

#define STBI_NO_SIMD
#define STBI_NO_STDIO
#define STBI_NO_FAILURE_STRINGS

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA
#define STBI_ONLY_HDR

#define STBI_MALLOC(size)                   gx_ImageLoaderMemoryInspector.Malloc ( size )
#define STBI_REALLOC(heapMemory,newSize)    gx_ImageLoaderMemoryInspector.Realloc ( heapMemory, newSize )
#define STBI_FREE(heapMemory)               gx_ImageLoaderMemoryInspector.Free ( heapMemory )

#define STB_IMAGE_IMPLEMENTATION

GX_DISABLE_COMMON_WARNINGS

#include <STB/stb_image.h>

GX_RESTORE_WARNING_STATE

//---------------------------------------------------------------------------------------------------------------------

GXBool GXCALL GXLoadLDRImage ( const GXWChar* fileName, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXUByte** data )
{
    GXUPointer len;
    GXUByte* content;

    GXFile file ( fileName );

    if ( !file.LoadContent ( content, len, eGXFileContentOwner::GXFile, GX_TRUE ) )
        return GX_FALSE;

    GXInt comp;
    GXInt w;
    GXInt h;

    *data = static_cast<GXUByte*> ( stbi_load_from_memory ( static_cast<const stbi_uc*> ( content ), static_cast<int>( len ), &w, &h, &comp, 0 ) );
    file.Close ();

    if ( !( *data ) )
        return GX_FALSE;

    width = static_cast<GXUInt> ( w );
    height = static_cast<GXUInt> ( h );

    GXUPointer lineSize = width * comp * sizeof ( GXUByte );
    GXUByte* tmp = static_cast<GXUByte*> ( gx_ImageLoaderMemoryInspector.Malloc ( lineSize ) );
    GXUByte* p = *data;
    GXUByte* n = *data + lineSize * ( height - 1u );

    while ( p < n )
    {
        memcpy ( tmp, p, lineSize );
        memcpy ( p, n, lineSize );
        memcpy ( n, tmp, lineSize );

        p += lineSize;
        n -= lineSize;
    }

    gx_ImageLoaderMemoryInspector.Free ( tmp );

    numChannels = static_cast<GXUByte> ( comp );
    return GX_TRUE;
}

GXBool GXCALL GXLoadHDRImage ( const GXWChar* fileName, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXFloat** data )
{
    GXUPointer len;
    GXUByte* content;

    GXFile file ( fileName );

    if ( !file.LoadContent ( content, len, eGXFileContentOwner::GXFile, GX_TRUE ) )
        return GX_FALSE;

    GXInt comp;
    GXInt w;
    GXInt h;

    *data = static_cast<GXFloat*> ( stbi_loadf_from_memory ( static_cast<const stbi_uc*> ( content ), static_cast<int>( len ), &w, &h, &comp, 0 ) );
    file.Close ();

    if ( !( *data ) ) return GX_FALSE;

    width = static_cast<GXUInt> ( w );
    height = static_cast<GXUInt> ( h );

    GXUPointer floatsPerLine = width * comp;
    GXUPointer lineSize = floatsPerLine * sizeof ( GXFloat );
    GXFloat* tmp = static_cast<GXFloat*> ( gx_ImageLoaderMemoryInspector.Malloc ( lineSize ) );
    GXFloat* p = *data;
    GXFloat* n = *data + floatsPerLine * ( height - 1u );

    while ( p < n )
    {
        memcpy ( tmp, p, lineSize );
        memcpy ( p, n, lineSize );
        memcpy ( n, tmp, lineSize );

        p += floatsPerLine;
        n -= floatsPerLine;
    }

    gx_ImageLoaderMemoryInspector.Free ( tmp );

    numChannels = static_cast<GXUByte> ( comp );
    return GX_TRUE;
}
