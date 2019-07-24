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

    // Warning: Some string Kung-Fu. GXString is COW string. So task is simple. Place all strings in GXDynamicArray without
    // actual assignment operator and losing string data after GXString destructor. So solution is:
    // 1) Allocate size of GXString bytes in stack. 
    // 2) Interpret this memory as GXString
    // 3) Init GXString via placement new operator
    // 4) Push back this string to GXDynamicArray
    //
    // Note GXAbstractDirectoryInfo takes care about GXString destruction by desing.

    GXUByte stringMemory[ sizeof ( GXString ) ];
    GXString* string = reinterpret_cast<GXString*> ( stringMemory );

    do
    {
        ::new ( string ) GXString ( info.cFileName );

        if ( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            _directories.PushBack ( string );
            continue;
        }

        _files.PushBack ( string );

        const GXUBigInt fileSize = static_cast<GXUBigInt> ( info.nFileSizeLow ) + ( static_cast<GXUBigInt> ( info.nFileSizeHigh ) << 32 );
        _fileSizes.PushBack ( &fileSize );
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
