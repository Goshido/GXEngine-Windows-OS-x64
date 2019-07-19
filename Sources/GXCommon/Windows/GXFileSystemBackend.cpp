// version 1.15

#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>


#define FAST_BUFFER_SYMBOLS         MAX_PATH
#define BUFFER_SIZE_IN_SYMBOLS      1024u

//---------------------------------------------------------------------------------------------------------------------

class GXFileSystemResource final : public GXMemoryInspector
{
    public:
        GXFileSystemResource ();
        ~GXFileSystemResource () override;

    private:
        GXFileSystemResource ( const GXFileSystemResource &other ) = delete;
        GXFileSystemResource& operator = ( const GXFileSystemResource &other ) = delete;
};

GXFileSystemResource::GXFileSystemResource ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXFileSystemResource" )
{
    // NOTHING
}

GXFileSystemResource::~GXFileSystemResource ()
{
    // NOTHING
}

static GXFileSystemResource gx_common_FileSystemResource;

//---------------------------------------------------------------------------------------------------------------------

GXBool GXCALL GXWriteToFile ( GXString fileName, const GXVoid* buffer, GXUPointer size )
{
    FILE* input;
    _wfopen_s ( &input, fileName, L"wb" );

    if ( !input )
    {
        GXWarningBox ( L"GXWriteToFile::Error - Не могу создать файл" );
        GXLogW ( L"GXWriteToFile::Error - Не могу создать файл %s\n", static_cast<const GXWChar*> ( fileName ) );

        return GX_FALSE;
    }

    fwrite ( buffer, size, 1u, input );
    fclose ( input );

    return GX_TRUE;
}

GXBool GXCALL GXDoesFileExist ( GXString fileName )
{
    FILE* input = nullptr;

    _wfopen_s ( &input, fileName, L"rb" );

    if ( input == nullptr )
        return GX_FALSE;

    fclose ( input );
    return GX_TRUE;
}

GXString GXCALL GXGetCurrentDirectory ()
{
    GXWChar* buffer = nullptr;
    GXWChar fastBuffer[ FAST_BUFFER_SYMBOLS ];

    const GXUPointer size = GetCurrentDirectoryW ( FAST_BUFFER_SYMBOLS, fastBuffer );

    if ( size <= FAST_BUFFER_SYMBOLS )
    {
        buffer = fastBuffer;
    }
    else
    {
        buffer = static_cast<GXWChar*> ( gx_common_FileSystemResource.Malloc ( size * sizeof ( GXWChar ) ) );
        GetCurrentDirectoryW ( static_cast<DWORD> ( size ), buffer );
    }

    for ( GXWChar* p = buffer; *p != L'\0'; ++p )
    {
        if ( *p != '\\' ) continue;

        *p = L'/';
    }

    GXString result ( buffer );

    if ( size > FAST_BUFFER_SYMBOLS )
        gx_common_FileSystemResource.Free ( buffer );

    return result;
}

GXBool GXCALL GXDoesDirectoryExist ( GXString directory )
{
    const DWORD ret = GetFileAttributesW ( directory );
    return ( ret != INVALID_FILE_ATTRIBUTES ) && ( ret & FILE_ATTRIBUTE_DIRECTORY );
}

GXBool GXCALL GXCreateDirectory ( GXString directory )
{
    if ( directory.IsEmpty () || directory.IsNull () )
        return GX_FALSE;

    if ( GXDoesDirectoryExist ( directory ) )
        return GX_TRUE;

    GXUPointer offset = 0u;
    const GXUPointer symbols = directory.GetSymbolCount () + 1u;
    const GXWChar* directoryRaw = directory;

    GXWChar fastBuffer[ FAST_BUFFER_SYMBOLS ];
    GXWChar* buffer = symbols <= FAST_BUFFER_SYMBOLS ? fastBuffer : static_cast<GXWChar*> ( gx_common_FileSystemResource.Malloc ( symbols * sizeof ( GXWChar ) ) );
    GXBool result = GX_TRUE;

    for ( ; ; )
    {
        if ( offset >= symbols ) break;

        const GXWChar symbol = directoryRaw[ offset ];

        if ( symbol != L'\\' && symbol != L'/' && symbol != L'\0' )
        {
            buffer[ offset ] = symbol;
            ++offset;
            continue;
        }

        buffer[ offset ] = symbol;
        ++offset;
        buffer[ offset ] = L'\0';

        if ( GXDoesDirectoryExist ( buffer ) || CreateDirectoryW ( buffer, nullptr ) ) continue;

        GXLogW ( L"GXCreateDirectory::Error - Can't create directory %s.\n", buffer );
        result = GX_FALSE;

        break;
    }

    if ( symbols > FAST_BUFFER_SYMBOLS )
        gx_common_FileSystemResource.Free ( buffer );

    return result;
}

GXBool GXCALL GXGetDirectoryInfo ( GXDirectoryInfo &directoryInfo, GXString directory )
{
    if ( directory.IsEmpty () || directory.IsNull () )
        return GX_FALSE;

    GXString findKey;
    findKey.Format ( "%S/*", static_cast<const GXWChar*> ( directory ) );

    WIN32_FIND_DATAW info;
    HANDLE handleFind;
    handleFind = FindFirstFileW ( findKey, &info );

    if ( handleFind == INVALID_HANDLE_VALUE ) return GX_FALSE;

    GXDynamicArray folderNames ( sizeof ( GXWChar* ) );
    GXDynamicArray fileNames ( sizeof ( GXWChar* ) );
    GXDynamicArray fileSizes ( sizeof ( GXUBigInt ) );

    do
    {
        if ( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            GXWChar* folderName;
            GXWcsclone ( &folderName, info.cFileName );
            folderNames.SetValue ( folderNames.GetLength (), &folderName );
        }
        else
        {
            GXWChar* fileName;
            GXWcsclone ( &fileName, info.cFileName );
            fileNames.SetValue ( fileNames.GetLength (), &fileName );

            GXUBigInt fileSize = static_cast<GXUBigInt> ( info.nFileSizeLow ) + ( static_cast<GXUBigInt> ( info.nFileSizeHigh ) << 32 );
            fileSizes.SetValue ( fileSizes.GetLength (), &fileSize );
        }
    }
    while ( FindNextFileW ( handleFind, &info ) != 0 );

    if ( GetLastError () != ERROR_NO_MORE_FILES )
    {
        FindClose ( handleFind );

        GXWChar** files = reinterpret_cast<GXWChar**> ( fileNames.GetData () );
        GXUPointer total = fileNames.GetLength ();

        for ( GXUPointer i = 0u; i < total; ++i )
            free ( files[ i ] );

        GXWChar** folders = reinterpret_cast<GXWChar**> ( folderNames.GetData () );
        total = folderNames.GetLength ();

        for ( GXUPointer i = 0u; i < total; ++i )
            free ( folders[ i ] );

        return GX_FALSE;
    }

    FindClose ( handleFind );

    directoryInfo.Clear ();

    GXWChar* absolutePath = static_cast<GXWChar*> ( malloc ( BUFFER_SIZE_IN_SYMBOLS * sizeof ( GXWChar ) ) );
    GetFullPathNameW ( directory, BUFFER_SIZE_IN_SYMBOLS, absolutePath, nullptr );

    for ( GXWChar* p = absolutePath; *p != L'\0'; ++p )
    {
        if ( *p != L'\\' ) continue;

        *p = L'/';
    }

    directoryInfo._absolutePath = absolutePath;

    directoryInfo._totalFolders = static_cast<GXUInt> ( folderNames.GetLength () );

    if ( directoryInfo._totalFolders > 0 )
    {
        GXUPointer size = directoryInfo._totalFolders * sizeof ( GXWChar* );
        GXWChar** f = reinterpret_cast<GXWChar**> ( malloc ( size ) );
        memcpy ( f, folderNames.GetData (), size );
        directoryInfo._folderNames = const_cast<const GXWChar**> ( f );
    }
    else
    {
        directoryInfo._folderNames = nullptr;
    }

    directoryInfo._totalFiles = static_cast<GXUInt> ( fileNames.GetLength () );

    if ( directoryInfo._totalFiles < 1u )
    {
        directoryInfo._fileNames = nullptr;
        directoryInfo._fileSizes = nullptr;

        return GX_TRUE;
    }

    GXUPointer size = directoryInfo._totalFiles * sizeof ( GXWChar* );
    GXWChar** f = reinterpret_cast<GXWChar**> ( malloc ( size ) );
    memcpy ( f, fileNames.GetData (), size );
    directoryInfo._fileNames = const_cast<const GXWChar**> ( f );

    size = directoryInfo._totalFiles * sizeof ( GXUBigInt );
    GXUBigInt* s = static_cast<GXUBigInt*> ( malloc ( size ) );
    memcpy ( s, fileSizes.GetData (), size );
    directoryInfo._fileSizes = const_cast<const GXUBigInt*> ( s );

    return GX_TRUE;
}

GXString GXCALL GXGetFileDirectoryPath ( GXString fileName )
{
    if ( fileName.IsEmpty () || fileName.IsNull () )
        return {};

    const GXInt symbols = static_cast<const GXInt> ( fileName.GetSymbolCount () );

    if ( symbols == 0 )
        return {};

    GXInt i = symbols;
    const GXWChar* fileNameRaw = fileName;

    for ( ; i > 0; --i )
        if ( fileNameRaw[ i ] == L'\\' || fileNameRaw[ i ] == L'/' ) break;

    if ( i < 0 )
        return {};

    const GXUPointer targetSymbols = static_cast<const GXUPointer> ( i + 1 );
    const GXUPointer size = targetSymbols * sizeof ( GXWChar );

    GXWChar fastBuffer[ FAST_BUFFER_SYMBOLS ];
    GXWChar* buffer = targetSymbols <= FAST_BUFFER_SYMBOLS ? fastBuffer : static_cast<GXWChar*> ( gx_common_FileSystemResource.Malloc ( symbols * sizeof ( GXWChar ) ) );

    memcpy ( buffer, fileNameRaw, size - sizeof ( GXWChar ) );
    buffer[ i ] = L'\0';

    GXString result ( buffer );

    if ( targetSymbols > FAST_BUFFER_SYMBOLS )
        gx_common_FileSystemResource.Free ( buffer );

    return result;
}

GXString GXCALL GXGetBaseFileName ( GXString fileName )
{
    if ( fileName.IsEmpty () || fileName.IsNull () )
        return {};

    const GXInt symbols = static_cast<const GXInt> ( fileName.GetSymbolCount () );

    if ( symbols == 0 )
        return {};

    GXInt i = symbols;
    const GXWChar* fileNameRaw = fileName;

    for ( ; i > 0; --i )
        if ( fileNameRaw[ i ] == L'.' ) break;

    if ( i <= 0 )
        return {};

    --i;
    const GXInt end = i;

    for ( ; i >= 0; --i )
        if ( fileNameRaw[ i ] == L'\\' || fileNameRaw[ i ] == L'/' ) break;

    ++i;

    if ( i < 0 )
        i = 0;

    const GXInt start = i;
    const GXUPointer baseFileNameSymbols = static_cast<const GXUPointer> ( end - start + 1 );
    const GXUPointer targetSymbols = baseFileNameSymbols + 1u;
    const GXUPointer size = targetSymbols * sizeof ( GXWChar );

    GXWChar fastBuffer[ FAST_BUFFER_SYMBOLS ];
    GXWChar* buffer = targetSymbols <= FAST_BUFFER_SYMBOLS ? fastBuffer : static_cast<GXWChar*> ( gx_common_FileSystemResource.Malloc ( symbols * sizeof ( GXWChar ) ) );

    memcpy ( buffer, fileNameRaw + start, size - sizeof ( GXWChar ) );
    buffer[ baseFileNameSymbols ] = L'\0';

    GXString result ( buffer );

    if ( targetSymbols > FAST_BUFFER_SYMBOLS )
        gx_common_FileSystemResource.Free ( buffer );

    return result;
}

GXString GXCALL GXGetFileExtension ( GXString fileName )
{
    if ( fileName.IsEmpty () || fileName.IsNull () )
        return {};

    const GXInt symbols = static_cast<const GXInt> ( fileName.GetSymbolCount () );
    GXInt i = symbols - 1;
    const GXWChar* fileNameRaw = fileName;

    for ( ; i > 0; --i )
        if ( fileNameRaw[ i ] == L'.' ) break;

    if ( i < 0 )
        return {};

    ++i;

    const GXUPointer targetSymbols = static_cast<const GXUPointer> ( symbols - i + 1 );
    const GXUPointer size = targetSymbols * sizeof ( GXWChar );

    GXWChar fastBuffer[ FAST_BUFFER_SYMBOLS ];
    GXWChar* buffer = targetSymbols <= FAST_BUFFER_SYMBOLS ? fastBuffer : static_cast<GXWChar*> ( gx_common_FileSystemResource.Malloc ( symbols * sizeof ( GXWChar ) ) );

    memcpy ( buffer, fileNameRaw + i, size );
    GXString result ( buffer );

    if ( targetSymbols > FAST_BUFFER_SYMBOLS )
        gx_common_FileSystemResource.Free ( buffer );

    return result;
}

//---------------------------------------------------------------------------------------------------------------------

GXString GXCALL GXWordExp ( GXString expression )
{
    GXWChar fastBuffer[ FAST_BUFFER_SYMBOLS ];
    const GXUPointer size = static_cast<GXUPointer> ( ExpandEnvironmentStringsW ( expression, fastBuffer, FAST_BUFFER_SYMBOLS ) );

    if ( size <= FAST_BUFFER_SYMBOLS )
        return GXString ( fastBuffer );

    GXWChar* workingBuffer = static_cast<GXWChar*> ( gx_common_FileSystemResource.Malloc ( size * sizeof ( GXWChar ) ) );
    ExpandEnvironmentStringsW ( expression, workingBuffer, static_cast<DWORD> ( size ) );
    GXString result ( workingBuffer );

    gx_common_FileSystemResource.Free ( workingBuffer );
    return result;
}
