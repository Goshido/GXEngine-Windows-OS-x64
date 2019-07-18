// version 1.4

#ifndef GX_XINPUT
#define GX_XINPUT


#include <GXCommon/GXWarning.h>

GX_DISABLE_COMMON_WARNINGS

#include <XInput/XInput.h>

GX_RESTORE_WARNING_STATE


typedef DWORD ( WINAPI* XIXInputGetState ) ( DWORD dwUserIndex, XINPUT_STATE* pState );
typedef void ( WINAPI* XIXInputEnable ) ( BOOL enable );


struct GXXInputFunctions
{
    XIXInputGetState*       XInputGetState;
    XIXInputEnable*         XInputEnable;
};


#endif // GX_XINPUT
