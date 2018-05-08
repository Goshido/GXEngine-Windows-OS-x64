// version 1.2

#ifndef GX_UI_BUTTON
#define GX_UI_BUTTON


#include "GXWidget.h"
#include "GXUICommon.h"


class GXUIButton;
typedef GXVoid ( GXCALL* PFNGXONMOUSEBUTTONPROC ) ( GXVoid* handler, GXUIButton& button, GXFloat x, GXFloat y, eGXMouseButtonState state );


class GXUIButton : public GXWidget
{
	private:
		PFNGXONMOUSEBUTTONPROC		OnLeftMouseButton;
		GXVoid*						handler;

		GXBool						isPressed;
		GXBool						isHighlighted;
		GXBool						isDisabled;

	public:
		explicit GXUIButton ( GXWidget* parent );
		~GXUIButton () override;

		GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

		GXVoid SetOnLeftMouseButtonCallback ( GXVoid* handlerObject, PFNGXONMOUSEBUTTONPROC callback );

		GXVoid Enable ();
		GXVoid Disable ();
		GXVoid Redraw ();

		GXBool IsPressed () const;
		GXBool IsHighlighted () const;
		GXBool IsDisabled () const;

	private:
		GXUIButton () = delete;
		GXUIButton ( const GXUIButton &other ) = delete;
		GXUIButton& operator = ( const GXUIButton &other ) = delete;
};


#endif // GX_UI_BUTTON
