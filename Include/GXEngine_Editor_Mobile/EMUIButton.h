#ifndef EM_UI_BUTTON
#define EM_UI_BUTTON


#include "EMUI.h"
#include <GXEngine/GXUIButton.h>


class EMUIButton : public EMUI
{
	private:
		GXUIButton*		widget;

	public:
		explicit EMUIButton ( EMUI* parent );
		~EMUIButton () override;

		GXWidget* GetWidget () const override;

		GXVoid Enable ();
		GXVoid Disable ();

		GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );

		GXVoid SetCaption ( const GXWChar* caption );

		GXVoid Show ();
		GXVoid Hide ();

		GXVoid SetOnLeftMouseButtonCallback ( GXVoid* handler, PFNGXONMOUSEBUTTONPROC callback );
};


#endif // EM_UI_BUTTON
