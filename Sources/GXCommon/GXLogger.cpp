//version 1.4

#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMutex.h>
#include <GXCommon/GXMemory.h>
#include <cstdio>
#include <stdarg.h>


enum GXConsoleLocale
{
	GX_UNICODE_RUSSIAN,
	GX_ASCII,
	GX_UNKNOWN
};


GXConsoleLocale gx_lggr_ConsoleLocale = GX_UNKNOWN;
GXMutex*		gx_lggr_ConsoleMutex = 0;


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
		if ( gx_lggr_ConsoleLocale != GX_ASCII )
		{
			gx_lggr_ConsoleLocale = GX_ASCII;
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
		if ( gx_lggr_ConsoleLocale != GX_UNICODE_RUSSIAN )
		{
			gx_lggr_ConsoleLocale = GX_UNICODE_RUSSIAN;
			setlocale ( LC_CTYPE, "Russian" );
		}

		va_list ap;
		va_start ( ap, format );
		vwprintf ( format, ap );
		va_end ( ap );

		gx_lggr_ConsoleMutex->Release ();
	}
}
