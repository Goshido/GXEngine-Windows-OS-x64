// version 1.2

#ifndef GX_XINPUT
#define GX_XINPUT


#include <GXCommon/GXWarning.h>

GX_DISABLE_COMMON_WARNINGS

#include <XInput/XInput.h>

GX_RESTORE_WARNING_STATE


typedef DWORD ( WINAPI* PFNXINPUTGETSTATEPROC ) ( DWORD dwUserIndex, XINPUT_STATE* pState );
typedef void ( WINAPI* PFNXINPUTENABLEPROC ) ( BOOL enable );


struct GXXInputFunctions
{
	PFNXINPUTGETSTATEPROC*	XInputGetState;
	PFNXINPUTENABLEPROC*	XInputEnable;
};


#endif // GX_XINPUT
