//version 1.0

#ifndef GX_UI_COMMON
#define GX_UI_COMMON


#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXMutex.h>
#include <GXCommon/GXMemory.h>


enum eGXMouseButtonState : GXUByte
{
	GX_MOUSE_BUTTON_DOWN,
	GX_MOUSE_BUTTON_UP
};


typedef GXVoid ( GXCALL* PFNGXONMOUSESCROLLPROC ) ( GXFloat scroll, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXONMOUSEMOVEPROC ) ( GXFloat x, GXFloat y );


extern GXFloat gx_ui_Scale;		//rough pixels in centimeter

extern GXMutex* gx_ui_Mutex;
extern GXCircleBuffer* gx_ui_MessageBuffer;


#endif //GX_UI_COMMON
