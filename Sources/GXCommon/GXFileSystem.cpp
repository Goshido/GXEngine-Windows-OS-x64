// version 1.14

#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>


#define BUFFER_SIZE_IN_SYMBOLS      1024u

//---------------------------------------------------------------------------------------------------------------------

GXDirectoryInfo::GXDirectoryInfo ()
{
    _absolutePath = nullptr;

    _totalFolders = 0u;
    _folderNames = nullptr;

    _totalFiles = 0u;
    _fileNames = nullptr;
    _fileSizes = nullptr;
}

GXDirectoryInfo::~GXDirectoryInfo ()
{
    Clear ();
}

GXVoid GXDirectoryInfo::Clear ()
{
    GXWChar* p = const_cast<GXWChar*> ( _absolutePath );
    GXSafeFree ( p );
    _absolutePath = nullptr;

    for ( GXUInt i = 0; i < _totalFolders; ++i )
    {
        p = const_cast<GXWChar*> ( _folderNames[ i ] );
        GXSafeFree ( p );
    }

    _totalFolders = 0u;
    GXSafeFree ( _folderNames );

    for ( GXUInt i = 0u; i < _totalFiles; ++i )
    {
        p = (GXWChar*)_fileNames[ i ];
        GXSafeFree ( p );
    }

    _totalFiles = 0u;
    GXSafeFree ( _fileNames );

    GXUBigInt* s = const_cast<GXUBigInt*> ( _fileSizes );
    GXSafeFree ( s );
    _fileSizes = nullptr;
}

//---------------------------------------------------------------------------------------------------------------------

GXBool GXCALL GXLoadFile ( const GXWChar* fileName, GXVoid** buffer, GXUPointer &size, GXBool notsilent )
{
    FILE* input = nullptr;
    GXUPointer fileSize = 0u;
    GXUPointer readed = 0u;

    _wfopen_s ( &input, fileName, L"rb" );

    if ( input == nullptr )
    {
        if ( notsilent )
        {
            GXWarningBox ( L"GXLoadFile::Error - Не могу открыть файл" );
            GXLogA ( "GXLoadFile::Error - Не могу открыть файл %S\n", fileName );
        }

        *buffer = nullptr;
        size = 0u;

        return GX_FALSE;
    }

    fseek ( input, 0, SEEK_END );
    fileSize = static_cast<GXUPointer> ( ftell ( input ) );
    rewind ( input );

    if ( fileSize == 0u )
    {
        GXWarningBox ( L"GXLoadFile::Error - Файл пуст" );
        GXLogA ( "GXLoadFile::Error - Файл пуст %S\n", fileName );

        fclose ( input );
        *buffer = nullptr;
        size = 0u;

        return GX_FALSE;
    }

    *buffer = malloc ( fileSize );
    readed = static_cast<GXUPointer> ( fread ( *buffer, 1, fileSize, input ) );
    fclose ( input );

    if ( readed != fileSize )
    {
        GXWarningBox ( L"GXLoadFile::Error - Не могу прочесть файл" );
        GXLogA ( "GXLoadFile::Error - Не могу прочесть файл %S\n", fileName );

        free ( *buffer );
        *buffer = nullptr;
        size = 0u;

        return GX_FALSE;
    }

    size = fileSize;
    return GX_TRUE;
}

GXBool GXCALL GXWriteToFile ( const GXWChar* fileName, const GXVoid* buffer, GXUPointer size )
{
    FILE* input;
    _wfopen_s ( &input, fileName, L"wb" );

    if ( !input )
    {
        GXWarningBox ( L"GXWriteToFile::Error - Не могу создать файл" );
        GXLogA ( "GXWriteToFile::Error - Не могу создать файл %S\n", fileName );

        return GX_FALSE;
    }

    fwrite ( buffer, size, 1u, input );
    fclose ( input );

    return GX_TRUE;
}

GXBool GXCALL GXDoesFileExist ( const GXWChar* fileName )
{
    FILE* input = nullptr;

    _wfopen_s ( &input, fileName, L"rb" );

    if ( input == nullptr ) return GX_FALSE;
    
    fclose ( input );
    return GX_TRUE;
}

GXVoid GXCALL GXGetCurrentDirectory ( GXWChar** currentDirectory )
{
    GXWChar* buffer = static_cast<GXWChar*> ( malloc ( BUFFER_SIZE_IN_SYMBOLS * sizeof ( GXWChar ) ) );
    GetCurrentDirectoryW ( BUFFER_SIZE_IN_SYMBOLS, buffer );

    for ( GXWChar* p = buffer; *p != L'\0'; ++p )
    {
        if ( *p != '\\' ) continue;

        *p = L'/';
    }

    GXWcsclone ( currentDirectory, buffer );

    free ( buffer );
}

GXBool GXCALL GXDoesDirectoryExist ( const GXWChar* directory )
{
    DWORD ret = GetFileAttributesW ( directory );
    return ( ret != INVALID_FILE_ATTRIBUTES ) && ( ret & FILE_ATTRIBUTE_DIRECTORY );
}

GXBool GXCALL GXCreateDirectory ( const GXWChar* directory )
{
    if ( GXDoesDirectoryExist ( directory ) )
        return GX_TRUE;

    GXUPointer offset = 0;
    GXUPointer symbols = wcslen ( directory ) + 1;
    GXWChar buffer[ MAX_PATH ];

    for ( ; ; )
    {
        if ( offset >= symbols ) break;

        const GXWChar symbol = directory[ offset ];

        if ( symbol != L'\\' && symbol != L'/' && symbol != L'\0' )
        {
            buffer[ offset ] = symbol;
            ++offset;
            continue;
        }

        buffer[ offset ] = symbol;
        ++offset;
        buffer[ offset ] = L'\0';

        if ( GXDoesDirectoryExist ( buffer ) ) continue;

        if ( CreateDirectoryW ( buffer, nullptr ) ) continue;

        GXLogA ( "GXCreateDirectory::Error - Can't create directory %S.\n", buffer );
        return GX_FALSE;
    }

    return GX_TRUE;
}

GXBool GXCALL GXCreateDirectory ( GXString directory )
{
    return GXCreateDirectory ( static_cast<const GXWChar*> ( directory ) );
}

GXBool GXCALL GXGetDirectoryInfo ( GXDirectoryInfo &directoryInfo, const GXWChar* directory )
{
    if ( !directory ) return GX_FALSE;

    GXUPointer size = ( GXWcslen ( directory ) + 3u ) * sizeof ( GXWChar );    // /, * and \0 symbols
    GXWChar* listedDirectory = static_cast<GXWChar*> ( malloc ( size ) );
    wsprintfW ( listedDirectory, L"%s/*", directory );

    WIN32_FIND_DATAW info;
    HANDLE handleFind;
    handleFind = FindFirstFileW ( listedDirectory, &info );

    free ( listedDirectory );

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

    GXWChar* absolutePath = (GXWChar*)malloc ( BUFFER_SIZE_IN_SYMBOLS * sizeof ( GXWChar) );
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
        size = directoryInfo._totalFolders * sizeof ( GXWChar* );
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

    size = directoryInfo._totalFiles * sizeof ( GXWChar* );
    GXWChar** f = reinterpret_cast<GXWChar**> ( malloc ( size ) );
    memcpy ( f, fileNames.GetData (), size );
    directoryInfo._fileNames = const_cast<const GXWChar**> ( f );

    size = directoryInfo._totalFiles * sizeof ( GXUBigInt );
    GXUBigInt* s = static_cast<GXUBigInt*> ( malloc ( size ) );
    memcpy ( s, fileSizes.GetData (), size );
    directoryInfo._fileSizes = const_cast<const GXUBigInt*> ( s );

    return GX_TRUE;
}

GXVoid GXCALL GXGetFileDirectoryPath ( GXWChar** path, const GXWChar* fileName )
{
    if ( !fileName )
    {
        *path = nullptr;
        return;
    }

    GXInt symbols = static_cast<GXInt> ( GXWcslen ( fileName ) );

    if ( symbols == 0 )
    {
        *path = nullptr;
        return;
    }

    GXInt i = symbols;

    for ( ; i > 0; --i )
        if ( fileName[ i ] == L'\\' || fileName[ i ] == L'/' ) break;

    if ( i < 0 )
    {
        *path = nullptr;
        return;
    }


    GXUPointer size = ( i + 1 ) * sizeof ( GXWChar );
    *path = static_cast<GXWChar*> ( malloc ( size ) );
    memcpy ( *path, fileName, size - sizeof ( GXWChar ) );

    ( *path )[ i ] = 0;
}

GXVoid GXCALL GXGetBaseFileName ( GXWChar** baseFileName, const GXWChar* fileName )
{
    if ( !fileName )
    {
        *baseFileName = nullptr;
        return;
    }

    GXInt symbols = static_cast<GXInt> ( GXWcslen ( fileName ) );

    if ( symbols == 0 )
    {
        *baseFileName = nullptr;
        return;
    }

    GXInt i = symbols;

    for ( ; i > 0; --i )
        if ( fileName[ i ] == L'.' ) break;

    if ( i <= 0 )
    {
        *baseFileName = nullptr;
        return;
    }

    --i;
    GXInt end = i;

    for ( ; i >= 0; --i )
        if ( fileName[ i ] == L'\\' || fileName[ i ] == L'/' ) break;

    ++i;

    if ( i < 0 )
        i = 0;

    GXInt start = i;
    GXInt baseFileNameSymbols = end - start + 1;
    GXUPointer size = ( baseFileNameSymbols + 1 ) * sizeof ( GXWChar );
    *baseFileName = static_cast<GXWChar*> ( malloc ( size ) );
    memcpy ( *baseFileName, fileName + start, size - sizeof ( GXWChar ) );

    ( *baseFileName )[ baseFileNameSymbols ] = 0;
}

GXVoid GXCALL GXGetFileExtension ( GXWChar** extension, const GXWChar* fileName )
{
    if ( !fileName )
    {
        *extension = nullptr;
        return;
    }

    GXInt symbols = static_cast<GXInt> ( GXWcslen ( fileName ) );
    GXInt i = symbols - 1;

    for ( ; i > 0; --i )
        if ( fileName[ i ] == L'.' ) break;

    if ( i < 0 )
    {
        *extension = nullptr;
        return;
    }

    ++i;

    GXUInt extensionSymbols = static_cast<GXUInt> ( symbols ) - i + 1;
    GXUInt size = extensionSymbols * sizeof ( GXWChar );

    *extension = static_cast<GXWChar*> ( malloc ( size ) );
    memcpy ( *extension, fileName + i, size );
}

//------------------------------------------------------------------------------------------------

GXWriteFileStream::GXWriteFileStream ( const GXWChar* fileName )
{
    _wfopen_s ( &_file, fileName, L"wb" );

    if ( _file ) return;

    GXLogA ( "GXWriteToFile::Error - Не могу создать файл %S\n", fileName );
}

GXWriteFileStream::~GXWriteFileStream ()
{
    Close ();
}

GXVoid GXWriteFileStream::Write ( const GXVoid* data, GXUPointer size )
{
    if ( !_file ) return;

    fwrite ( data, size, 1u, _file );
}

GXVoid GXWriteFileStream::Flush ()
{
    fflush ( _file );
}

GXVoid GXWriteFileStream::Close ()
{
    if ( !_file ) return;

    fclose ( _file );
    _file = nullptr;
}
