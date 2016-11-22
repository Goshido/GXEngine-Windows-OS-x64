//version 1.0

#include <GXEngineDLL/GXEngineDLL.h>
#include <GXEngineDLL/GXXInput.h>


GXDLLEXPORT GXVoid GXCALL GXXInputInit ( GXXInputFunctions &out )
{
	*out.XInputEnable = &XInputEnable;
	*out.XInputGetState = &XInputGetState;
}