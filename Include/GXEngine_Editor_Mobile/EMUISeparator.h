#ifndef EM_UI_SEPARATOR
#define EM_UI_SEPARATOR


#include "EMUI.h"


class EMUISeparator : public EMUI
{
	private:
		GXWidget*	widget;

	public:
		EMUISeparator ( EMUI* parent );
		virtual ~EMUISeparator ();

		virtual GXWidget* GetWidget ();
		virtual GXVoid OnDrawMask ();

		GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );
};


#endif //EM_UI_SEPARATOR
