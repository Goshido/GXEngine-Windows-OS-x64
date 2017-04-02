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
		~GXUIButton () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXVoid SetOnLeftMouseButtonCallback ( GXVoid* handler, PFNGXONMOUSEBUTTONPROC callback );

		GXVoid Enable ();
		GXVoid Disable ();
		GXVoid Redraw ();

		GXBool IsPressed () const;
		GXBool IsHighlighted () const;
		GXBool IsDisabled () const;
};


#endif //GX_UI_BUTTON
