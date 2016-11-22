//version 1.0

#ifndef GX_XINPUT
#define GX_XINPUT


#include <XInput/XInput.h>


typedef DWORD ( WINAPI* PFNXINPUTGETSTATEPROC ) ( DWORD dwUserIndex, XINPUT_STATE* pState );
typedef void ( WINAPI* PFNXINPUTENABLEPROC ) ( BOOL enable );


struct GXXInputFunctions
{
	PFNXINPUTGETSTATEPROC*	XInputGetState;
	PFNXINPUTENABLEPROC*	XInputEnable;
};


#endif //GX_XINPUT
