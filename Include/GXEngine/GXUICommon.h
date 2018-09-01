// version 1.1

#ifndef GX_UI_COMMON
#define GX_UI_COMMON


#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXSmartLock.h>
#include <GXCommon/GXMemory.h>


enum class eGXMouseButtonState : GXUByte
{
	Down,
	Up
};


typedef GXVoid ( GXCALL* PFNGXONMOUSESCROLLPROC ) ( GXFloat scroll, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXONMOUSEMOVEPROC ) ( GXFloat x, GXFloat y );


extern GXFloat			gx_ui_Scale;		// rough pixels in centimeter

extern GXSmartLock*		gx_ui_SmartLock;
extern GXCircleBuffer*	gx_ui_MessageBuffer;


#endif // GX_UI_COMMON
