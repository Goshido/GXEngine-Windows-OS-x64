// version 1.9

#ifndef GX_LOGGER
#define GX_LOGGER


#include <GXCommon/GXStrings.h>

GX_DISABLE_COMMON_WARNINGS

#include <locale.h>
#include <iostream>

GX_RESTORE_WARNING_STATE


GXVoid GXCALL GXWarningBox ( GXString message );

GXVoid GXCALL GXLogInit ();
GXVoid GXCALL GXLogDestroy ();

GXVoid GXCDECLCALL GXLogA ( const GXMBChar* format, ... );
GXVoid GXCDECLCALL GXLogW ( const GXWChar* format, ... );


#endif // GX_LOGGER
