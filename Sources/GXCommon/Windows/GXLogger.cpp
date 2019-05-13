// version 1.9

#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXSmartLock.h>

GX_DISABLE_COMMON_WARNINGS

#include <cstdio>
#include <stdarg.h>
#include <fcntl.h>
#include <io.h>

GX_RESTORE_WARNING_STATE


#define RUSSIAN_CODE_PAGE       1251u
#define CONSOLE_CODE_PAGE       RUSSIAN_CODE_PAGE

//---------------------------------------------------------------------------------------------------------------------

enum class eGXConsoleMode: GXUByte
{
    WideCharacters,
    Multibyte,
    Unknown
};

static eGXConsoleMode       gx_logger_ConsoleMode = eGXConsoleMode::Unknown;
static GXSmartLock*         gx_logger_SmartLock = nullptr;

GXVoid GXCALL GXWarningBox ( GXString message )
{
    MessageBoxW ( nullptr, message.ToSystemWideString (), L"Дебаг-окно", MB_ICONEXCLAMATION );
}

GXVoid GXCALL GXLogInit ()
{
    if ( gx_logger_SmartLock ) return;

    SetConsoleOutputCP ( CONSOLE_CODE_PAGE );
    gx_logger_SmartLock = new GXSmartLock ();
}

GXVoid GXCALL GXLogDestroy ()
{
    GXSafeDelete ( gx_logger_SmartLock );
}

GXVoid GXCDECLCALL GXLogA ( const GXMBChar* format, ... )
{
    // Note console font is important. For example Win7 x64 can't render cyrillic characters
    // with Raster font. Иге Consolas font works properly.

    if ( !gx_logger_SmartLock ) return;

    gx_logger_SmartLock->AcquireExclusive ();

    if ( gx_logger_ConsoleMode != eGXConsoleMode::Multibyte )
    {
        _setmode ( _fileno ( stdout ), _O_TEXT );
        gx_logger_ConsoleMode = eGXConsoleMode::Multibyte;
    }

    gx_logger_SmartLock->ReleaseExclusive ();

    va_list ap;
    va_start ( ap, format );
    vprintf ( format, ap );
    va_end ( ap );
}

GXVoid GXCDECLCALL GXLogW ( const GXWChar* format, ... )
{
    if ( !gx_logger_SmartLock ) return;

    gx_logger_SmartLock->AcquireExclusive ();

    if ( gx_logger_ConsoleMode != eGXConsoleMode::WideCharacters )
    {
        _setmode ( _fileno ( stdout ), _O_U16TEXT );
        gx_logger_ConsoleMode = eGXConsoleMode::WideCharacters;
    }

    gx_logger_SmartLock->ReleaseExclusive ();

    va_list ap;
    va_start ( ap, format );
    vwprintf ( format, ap );
    va_end ( ap );
}
