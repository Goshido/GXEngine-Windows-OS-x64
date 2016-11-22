//version 1.0

#ifndef GX_UI_BUTTON
#define GX_UI_BUTTON


#include "GXWidget.h"
#include "GXUICommon.h"


class GXUIButton;
typedef GXVoid ( GXCALL* PFNGXONMOUSEBUTTONPROC ) ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state );


class GXUIButton : public GXWidget
{
	private:
		PFNGXONMOUSEBUTTONPROC		OnLeftMouseButton;
		GXVoid*						handler;

		GXBool						isPressed;
		GXBool						isHighlighted;
		GXBool						isDisabled;

	public:
		GXUIButton ( GXWidget* parent );
		virtual ~GXUIButton ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid SetOnLeftMouseButtonCallback ( GXVoid* handler, PFNGXONMOUSEBUTTONPROC callback );

		GXVoid Enable ();
		GXVoid Disable ();
		GXVoid Redraw ();

		GXBool IsPressed ();
		GXBool IsHighlighted ();
		GXBool IsDisabled ();
};


#endif //GX_UI_BUTTON
