//version 1.4

#include <GXEngineDLL/GXEngineDLL.h>
#include <GXEngineDLL/GXXInput.h>


GXDLLEXPORT GXVoid GXCALL GXXInputInit ( GXXInputFunctions &out )
{
    *out.XInputEnable = &XInputEnable;
    *out.XInputGetState = &XInputGetState;
}
