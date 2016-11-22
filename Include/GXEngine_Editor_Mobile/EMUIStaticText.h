#include "EMUI.h"
#include <GXEngine/GXUIStaticText.h>


class EMUIStaticText : public EMUI
{
	private:
		GXUIStaticText*		widget;

	public:
		EMUIStaticText ( EMUI* parent );
		virtual ~EMUIStaticText ();

		virtual GXWidget* GetWidget ();
		virtual GXVoid OnDrawMask ();

		GXVoid SetText ( const GXWChar* text );
		const GXWChar* GetText ();

		GXVoid SetAlingment ( eGXUITextAlignment alignment );
		eGXUITextAlignment GetAlignment ();

		GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );
};
