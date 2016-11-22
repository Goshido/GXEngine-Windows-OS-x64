//version 1.4

#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMutex.h>
#include <GXCommon/GXCommon.h>
#include <stdio.h>


enum GXConsoleLocale
{
	GX_UNICODE_RUSSIAN,
	GX_ASCII,
	GX_UNKNOWN
};


GXConsoleLocale gx_lggr_ConsoleLocale = GX_UNKNOWN;
GXMutex*		gx_lggr_ConsoleMutex = 0;	

GXWChar gx_logger_lpDisplayBuf[ 120 ] = { 0 };


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
			setlocale ( LC_ALL, "Russian" );
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
			setlocale ( LC_ALL, "Russian" );
		}

		va_list ap;
		va_start ( ap, format );
		vwprintf ( format, ap );
		va_end ( ap );

		gx_lggr_ConsoleMutex->Release ();
	}
}
/*
GXVoid GXCALL GXSeeError ( LPTSTR lpszFunction, GXBool isDebugBox )
{ 
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError (); 

	FormatMessage	(
						FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM |
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dw,
						MAKELANGID ( LANG_NEUTRAL, SUBLANG_DEFAULT ),
						( LPTSTR )&lpMsgBuf,
						0, NULL 
					);

	swprintf_s ( gx_logger_lpDisplayBuf, 120, L"%s вывалилась с ошибкой %d: %s", lpszFunction, dw, lpMsgBuf );

	if ( isDebugBox )
	{
		GXDebugBox ( gx_logger_lpDisplayBuf );
		GXLogW ( L"%s\n", gx_logger_lpDisplayBuf );
	}

	LocalFree ( lpMsgBuf );
}
*/
