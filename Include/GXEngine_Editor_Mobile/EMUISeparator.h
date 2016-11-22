#ifndef EM_UI_SEPARATOR
#define EM_UI_SEPARATOR


#include "EMUI.h"


class EMUISeparator : public EMUI
{
	private:
		GXWidget*	widget;

	public:
		EMUISeparator ( EMUI* parent );
		~EMUISeparator () override;

		GXWidget* GetWidget () const override;
		GXVoid OnDrawMask () override;

		GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );
};


#endif //EM_UI_SEPARATOR
