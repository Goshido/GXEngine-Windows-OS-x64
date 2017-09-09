//version 1.0

#ifndef GX_XINPUT
#define GX_XINPUT


#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <XInput/XInput.h>
#include <GXCommon/GXRestoreWarnings.h>


typedef DWORD ( WINAPI* PFNXINPUTGETSTATEPROC ) ( DWORD dwUserIndex, XINPUT_STATE* pState );
typedef void ( WINAPI* PFNXINPUTENABLEPROC ) ( BOOL enable );


struct GXXInputFunctions
{
	PFNXINPUTGETSTATEPROC*	XInputGetState;
	PFNXINPUTENABLEPROC*	XInputEnable;
};


#endif //GX_XINPUT
