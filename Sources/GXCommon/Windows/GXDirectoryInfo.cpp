// version 1.0

#include <GXCommon/Windows/GXDirectoryInfo.h>

GX_DISABLE_COMMON_WARNINGS

#include <new>

GX_RESTORE_WARNING_STATE


#define FAST_BUFFER_SYMBOLS     MAX_PATH

//---------------------------------------------------------------------------------------------------------------------

GXDirectoryInfo::GXDirectoryInfo ( GXString targetDirectory ):
    GXAbstractDirectoryInfo ( targetDirectory )
{
    if ( _targetDirectory.IsEmpty () || _targetDirectory.IsNull () ) return;

    GXString findKey;
    findKey.Format ( "%S/*", static_cast<const GXWChar*> ( _targetDirectory ) );

    WIN32_FIND_DATAW info;
    HANDLE handleFind;
    handleFind = FindFirstFileW ( findKey, &info );

    if ( handleFind == INVALID_HANDLE_VALUE ) return;

    GXUPointer directories = 0u;
    GXUPointer files = 0u;

    do
    {
        if ( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            ++directories;
            _directories.Resize ( directories );
            GXString* directory = static_cast<GXString*> ( _directories.GetValue ( directories - 1u ) );
            ::new ( directory ) GXString ( info.cFileName );

            continue;
        }

        ++files;
        _files.Resize ( files );
        _fileSizes.Resize ( files );

        const GXUPointer fileIndex = files - 1u;

        GXString* file = static_cast<GXString*> ( _files.GetValue ( fileIndex ) );
        ::new ( file ) GXString ( info.cFileName );

        const GXUBigInt fileSize = static_cast<GXUBigInt> ( info.nFileSizeLow ) + ( static_cast<GXUBigInt> ( info.nFileSizeHigh ) << 32 );
        _fileSizes.SetValue ( fileIndex, &fileSize );
    }
    while ( FindNextFileW ( handleFind, &info ) != 0 );

    if ( GetLastError () != ERROR_NO_MORE_FILES )
    {
        FindClose ( handleFind );
        return;
    }

    FindClose ( handleFind );

    GXWChar fastBuffer[ FAST_BUFFER_SYMBOLS ];

    // Note reserving one symbol for null terminator.
    const GXUPointer requiredSymbols = 1u + static_cast<GXUPointer> ( GetFullPathNameW ( _targetDirectory, FAST_BUFFER_SYMBOLS, fastBuffer, nullptr ) );

    GXWChar* buffer = fastBuffer;

    if ( requiredSymbols > FAST_BUFFER_SYMBOLS )
    {
        buffer = static_cast<GXWChar*> ( Malloc ( requiredSymbols * sizeof ( GXWChar ) ) );
        GetFullPathNameW ( _targetDirectory, static_cast<DWORD> ( requiredSymbols ), buffer, nullptr );
    }

    // Windows OS back slashed are replaced by normal slashes for GXEngine consistency.

    GXWChar* p = buffer;

    for ( ; *p != L'\0'; ++p )
    {
        if ( *p != L'\\' ) continue;

        *p = L'/';
    }

    // Cutting end slash if it exists.

    --p;

    if ( *p == L'/' )
        *p = L'\0';

    _absolutePath = buffer;

    if ( requiredSymbols <= FAST_BUFFER_SYMBOLS ) return;

    Free ( buffer );
}

GXDirectoryInfo::~GXDirectoryInfo ()
{
    // NOTHING
}
