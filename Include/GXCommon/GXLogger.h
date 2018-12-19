// version 1.8

#ifndef GX_LOGGER
#define GX_LOGGER


#include "GXTypes.h"

GX_DISABLE_COMMON_WARNINGS

#include <locale.h>
#include <iostream>

GX_RESTORE_WARNING_STATE


#define GXDebugBox(x) MessageBoxW ( 0, x, L"Дебаг-окно", MB_ICONEXCLAMATION )


GXVoid GXCALL GXLogInit ();
GXVoid GXCALL GXLogDestroy ();

GXVoid GXCDECLCALL GXLogA ( const GXMBChar* format, ... );
GXVoid GXCDECLCALL GXLogW ( const GXWChar* format, ... );


#endif // GX_LOGGER
