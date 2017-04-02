//version 1.0

#ifndef GX_UI_INPUT
#define GX_UI_INPUT


#include "GXWidget.h"


class GXUIInput;
typedef GXVoid ( GXCALL* PFNGXUIINPUTONMOUSEPOSITIONPROC ) ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXUIINPUTONMOUSESCROLLPROC ) ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y, GXFloat scroll );

class GXUIInput : public GXWidget
{
	private:
		PFNGXUIINPUTONMOUSEPOSITIONPROC		onLMBDownCallback;
		PFNGXUIINPUTONMOUSEPOSITIONPROC		onLMBUpCallback;
		PFNGXUIINPUTONMOUSEPOSITIONPROC		onMMBDownCallback;
		PFNGXUIINPUTONMOUSEPOSITIONPROC		onMMBUpCallback;
		PFNGXUIINPUTONMOUSEPOSITIONPROC		onRMBDownCallback;
		PFNGXUIINPUTONMOUSEPOSITIONPROC		onRMBUpCallback;

		PFNGXUIINPUTONMOUSEPOSITIONPROC		onDoubleClickCallback;
		PFNGXUIINPUTONMOUSESCROLLPROC		onMouseScrollCallback;
		PFNGXUIINPUTONMOUSEPOSITIONPROC		onMouseMoveCallback;

		GXVoid*								handler;

	public:
		GXUIInput ( GXWidget* parent, GXBool isNeedRegister );
		~GXUIInput () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXVoid SetOnLeftMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback );
		GXVoid SetOnLeftMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback );
		GXVoid SetOnMiddleMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback );
		GXVoid SetOnMiddleMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback );
		GXVoid SetOnRightMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback );
		GXVoid SetOnRightMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback );

		GXVoid SetOnDoubleClickCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback );
		GXVoid SetOnMouseMoveCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback );
		GXVoid SetOnMouseScrollCallback ( PFNGXUIINPUTONMOUSESCROLLPROC callback );

		GXVoid SetHandler ( GXVoid* handler );
};


#endif //GX_UI_INPUT
