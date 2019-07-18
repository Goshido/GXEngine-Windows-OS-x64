// verson 1.0

#include <GXEngine/Windows/GXCrashReport.h>
#include <GXEngine/GXVersion.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>

GX_DISABLE_COMMON_WARNINGS

#include <Dbghelp.h>

GX_RESTORE_WARNING_STATE


#define CRASH_REPORT_DIRECTORY      "%APPDATA%/Goshido Inc/GXEngine/crash dumps/"

#define STR(x)                      #x
#define STRINGIZE(x)                STR ( x )
#define VERSION_STRING              STRINGIZE ( GX_VERSION_MAJOR ) "." STRINGIZE ( GX_VERSION_MINOR ) "." STRINGIZE ( GX_VERSION_RELEASE ) "." STRINGIZE ( GX_VERSION_BUILD )

//---------------------------------------------------------------------------------------------------------------------

typedef BOOL ( WINAPI* MiniDumpWriteDumpProc ) ( HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType, PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, PMINIDUMP_CALLBACK_INFORMATION CallbackParam );

GXCrashReport::GXCrashReport ()
{
    const GXString resolvedBaseDirectory ( GXWordExp ( CRASH_REPORT_DIRECTORY ) );

    GXString targetDirectory;
    targetDirectory.Format ( "%Sversion " VERSION_STRING "/", static_cast<const GXWChar*> ( resolvedBaseDirectory ) );
    
    if ( !GXCreateDirectory ( targetDirectory ) )
    {
        GXLogA ( "GXCrashReport::GXCrashReport::Error - Can't create crash report directory %S\n", static_cast<const GXWChar*> ( targetDirectory ) );
        return;
    }

    SetUnhandledExceptionFilter ( &GXCrashReport::UnhandledExceptionFilter );
}

GXCrashReport::~GXCrashReport ()
{
    // NOTHING
}

LONG CALLBACK GXCrashReport::UnhandledExceptionFilter ( EXCEPTION_POINTERS* exceptionPointers )
{
    // Note code here is executed in very unstable environment: stack corruption, heap corruption.
    // So the code can not use existing GXEngine implementation by design.
    // Note all valiables are allocated in global memory (not stack) by design to successfully generate crash dump file.
    // see https://stackoverflow.com/questions/5028781/how-to-write-a-sample-code-that-will-crash-and-produce-dump-file

    static HMODULE dbghelp = LoadLibraryA ( "dbghelp.dll" );

    if ( !dbghelp )
        return EXCEPTION_CONTINUE_SEARCH;

    static MiniDumpWriteDumpProc GXMiniDumpWriteDump = static_cast<MiniDumpWriteDumpProc> ( static_cast<GXVoid*> ( GetProcAddress ( dbghelp, "MiniDumpWriteDump" ) ) );

    if ( !GXMiniDumpWriteDump )
        return EXCEPTION_CONTINUE_SEARCH;

    static FILETIME utcFileTime;
    GetSystemTimeAsFileTime ( &utcFileTime );

    static FILETIME localFileTime;
    FileTimeToLocalFileTime ( &utcFileTime, &localFileTime );

    static SYSTEMTIME currentTime;
    FileTimeToSystemTime ( &localFileTime, &currentTime );

    static GXMBChar directory[ MAX_PATH ];
    ExpandEnvironmentStringsA ( CRASH_REPORT_DIRECTORY, directory, MAX_PATH );

    static GXMBChar name[ MAX_PATH ];
    static const GXMBChar* processFileName = name + GetModuleFileNameA ( GetModuleHandleA ( nullptr ), name, MAX_PATH );

    for ( ; *processFileName != '\\' && *processFileName != '/'; --processFileName );

    ++processFileName;
    static GXMBChar dumpFilePath[ MAX_PATH ];
    wsprintfA ( dumpFilePath, "%sversion " VERSION_STRING "\\%s %04hu.%02hu.%02hu %02hu-%02hu-%02hu.dmp", directory, processFileName, currentTime.wYear, currentTime.wMonth, currentTime.wDay, currentTime.wHour, currentTime.wMinute, currentTime.wSecond );

    static HANDLE dumpFile = CreateFileA ( dumpFilePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );

    if ( !dumpFile )
        return EXCEPTION_CONTINUE_SEARCH;

    static MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
    exceptionInfo.ThreadId = GetCurrentThreadId ();
    exceptionInfo.ExceptionPointers = exceptionPointers;
    exceptionInfo.ClientPointers = FALSE;

    GXMiniDumpWriteDump ( GetCurrentProcess (), GetCurrentProcessId (), dumpFile, static_cast<MINIDUMP_TYPE> ( MiniDumpNormal ), exceptionPointers ? &exceptionInfo : nullptr, nullptr, nullptr );
    CloseHandle ( dumpFile );

    return EXCEPTION_CONTINUE_SEARCH;
}
