#ifndef EM_UI_EDIT_BOX
#define EM_UI_EDIT_BOX


#include <GXEngine/GXUIEditBox.h>
#include "EMUI.h"


class EMUIEditBox : public EMUI
{
	private:
		GXUIEditBox*	widget;

	public:
		EMUIEditBox ( EMUI* parent );
		virtual ~EMUIEditBox ();

		virtual GXWidget* GetWidget ();

		GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );

		GXVoid SetText ( const GXWChar* text );
		const GXWChar* GetText ();

		GXVoid SetAlignment ( eGXUITextAlignment alignment );
		eGXUITextAlignment GetAlignment ();

		GXVoid SetFont ( const GXWChar* fontFile, GXUShort fontSize );
		GXFont* GetFont ();
};


#endif //EM_UI_EDIT_BOX
