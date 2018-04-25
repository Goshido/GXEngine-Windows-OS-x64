// version 1.5

#ifndef GX_LOGGER
#define GX_LOGGER


#include "GXTypes.h"
#include "GXDisable3rdPartyWarnings.h"
#include <locale.h>
#include <iostream>
#include "GXRestoreWarnings.h"


#define GXDebugBox(x) MessageBoxW ( 0, x, L"Дебаг-окно", MB_ICONEXCLAMATION )


GXVoid GXCALL GXLogInit ();
GXVoid GXCALL GXLogDestroy ();

GXVoid GXCDECLCALL GXLogA ( const GXMBChar* format, ... );
GXVoid GXCDECLCALL GXLogW ( const GXWChar* format, ... );


#endif // GX_LOGGER
