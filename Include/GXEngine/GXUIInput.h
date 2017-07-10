//version 1.1

#ifndef GX_UI_INPUT
#define GX_UI_INPUT


#include "GXWidget.h"


class GXUIInput;
typedef GXVoid ( GXCALL* PFNGXUIINPUTONMOUSEMOVEPROC ) ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXUIINPUTONMOUSEBUTTONPROC ) ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXUIINPUTNONDOUBLECLICKPROC ) ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXUIINPUTNSCROLLPROC ) ( GXVoid* handler, GXUIInput* input, GXFloat scroll, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXUIINPUTNMOUSEOVERPROC ) ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXUIINPUTNMOUSELEAVEPROC ) ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXUIINPUTNONKEYPROC ) ( GXVoid* handler, GXUIInput* input, GXInt key );


class GXUIInput : public GXWidget
{
	private:
		PFNGXUIINPUTONMOUSEBUTTONPROC		OnLMBDown;
		PFNGXUIINPUTONMOUSEBUTTONPROC		OnLMBUp;
		PFNGXUIINPUTONMOUSEBUTTONPROC		OnMMBDown;
		PFNGXUIINPUTONMOUSEBUTTONPROC		OnMMBUp;
		PFNGXUIINPUTONMOUSEBUTTONPROC		OnRMBDown;
		PFNGXUIINPUTONMOUSEBUTTONPROC		OnRMBUp;

		PFNGXUIINPUTNONDOUBLECLICKPROC		OnDoubleClick;
		PFNGXUIINPUTNSCROLLPROC				OnMouseScroll;
		PFNGXUIINPUTONMOUSEMOVEPROC			OnMouseMove;
		PFNGXUIINPUTNMOUSEOVERPROC			OnMouseOver;
		PFNGXUIINPUTNMOUSELEAVEPROC			OnMouseLeave;

		PFNGXUIINPUTNONKEYPROC				OnKeyDown;
		PFNGXUIINPUTNONKEYPROC				OnKeyUp;

		GXVoid*								handler;

	public:
		explicit GXUIInput ( GXWidget* parent, GXBool isNeedRegister );
		~GXUIInput () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXVoid SetOnLeftMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback );
		GXVoid SetOnLeftMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback );
		GXVoid SetOnMiddleMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback );
		GXVoid SetOnMiddleMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback );
		GXVoid SetOnRightMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback );
		GXVoid SetOnRightMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback );

		GXVoid SetOnDoubleClickCallback ( PFNGXUIINPUTNONDOUBLECLICKPROC callback );
		GXVoid SetOnMouseMoveCallback ( PFNGXUIINPUTONMOUSEMOVEPROC callback );
		GXVoid SetOnMouseScrollCallback ( PFNGXUIINPUTNSCROLLPROC callback );
		GXVoid SetOnMouseOverCallback ( PFNGXUIINPUTNMOUSEOVERPROC callback );
		GXVoid SetOnMouseLeaveCallback ( PFNGXUIINPUTNMOUSELEAVEPROC callback );

		GXVoid SetOnKeyDownCallback ( PFNGXUIINPUTNONKEYPROC callback );
		GXVoid SetOnKeyUpCallback ( PFNGXUIINPUTNONKEYPROC callback );

		GXVoid SetHandler ( GXVoid* handler );
};


#endif //GX_UI_INPUT
