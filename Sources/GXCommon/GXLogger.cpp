// version 1.6

#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMutex.h>
#include <GXCommon/GXMemory.h>

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


GXConsoleLocale gx_lggr_ConsoleLocale = GXConsoleLocale::Unknown;
GXMutex*		gx_lggr_ConsoleMutex = nullptr;


GXVoid GXCALL GXLogInit ()
{
	if ( gx_lggr_ConsoleMutex ) return;

	gx_lggr_ConsoleMutex = new GXMutex (); 
}

GXVoid GXCALL GXLogDestroy ()
{
	GXSafeDelete ( gx_lggr_ConsoleMutex );
}

GXVoid GXCDECLCALL GXLogA ( const GXMBChar* format, ... )
{
	if ( gx_lggr_ConsoleMutex )
	{
		gx_lggr_ConsoleMutex->Lock ();

		if ( gx_lggr_ConsoleLocale != GXConsoleLocale::ASCII )
		{
			gx_lggr_ConsoleLocale = GXConsoleLocale::ASCII;
			setlocale ( LC_CTYPE, "" );
		}

		va_list ap;
		va_start ( ap, format );
		vprintf ( format, ap );
		va_end ( ap );

		gx_lggr_ConsoleMutex->Release ();
	}
}

GXVoid GXCDECLCALL GXLogW ( const GXWChar* format, ... )
{
	if ( gx_lggr_ConsoleMutex )
	{
		gx_lggr_ConsoleMutex->Lock ();
		if ( gx_lggr_ConsoleLocale != GXConsoleLocale::UnicodeRussian )
		{
			gx_lggr_ConsoleLocale = GXConsoleLocale::UnicodeRussian;
			setlocale ( LC_CTYPE, "Russian" );
		}

		va_list ap;
		va_start ( ap, format );
		vwprintf ( format, ap );
		va_end ( ap );

		gx_lggr_ConsoleMutex->Release ();
	}
}
