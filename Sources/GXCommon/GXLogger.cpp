// version 1.8

#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXSmartLock.h>

GX_DISABLE_COMMON_WARNINGS

#include <cstdio>
#include <stdarg.h>

GX_RESTORE_WARNING_STATE


enum class GXConsoleLocale : GXUByte
{
    UnicodeRussian,
    ASCII,
    Unknown
};


GXConsoleLocale     gx_logger_ConsoleLocale = GXConsoleLocale::Unknown;
GXSmartLock*        gx_logger_SmartLock = nullptr;


GXVoid GXCALL GXLogInit ()
{
    if ( gx_logger_SmartLock ) return;

    gx_logger_SmartLock = new GXSmartLock ();
}

GXVoid GXCALL GXLogDestroy ()
{
    GXSafeDelete ( gx_logger_SmartLock );
}

GXVoid GXCDECLCALL GXLogA ( const GXMBChar* format, ... )
{
    if ( !gx_logger_SmartLock ) return;

    gx_logger_SmartLock->AcquireExlusive ();

    if ( gx_logger_ConsoleLocale != GXConsoleLocale::ASCII )
    {
        gx_logger_ConsoleLocale = GXConsoleLocale::ASCII;
        setlocale ( LC_CTYPE, "" );
    }

    va_list ap;
    va_start ( ap, format );
    vprintf ( format, ap );
    va_end ( ap );

    gx_logger_SmartLock->ReleaseExlusive ();
}

GXVoid GXCDECLCALL GXLogW ( const GXWChar* format, ... )
{
    if ( !gx_logger_SmartLock ) return;

    gx_logger_SmartLock->AcquireExlusive ();

    if ( gx_logger_ConsoleLocale != GXConsoleLocale::UnicodeRussian )
    {
        gx_logger_ConsoleLocale = GXConsoleLocale::UnicodeRussian;
        setlocale ( LC_CTYPE, "Russian" );
    }

    va_list ap;
    va_start ( ap, format );
    vwprintf ( format, ap );
    va_end ( ap );

    gx_logger_SmartLock->ReleaseExlusive ();
}
