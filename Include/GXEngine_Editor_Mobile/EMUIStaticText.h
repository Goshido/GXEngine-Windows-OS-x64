#ifndef EM_UI_STATIC_TEXT
#define EM_UI_STATIC_TEXT


#include "EMUI.h"
#include <GXEngine/GXUIStaticText.h>


class EMUIStaticText : public EMUI
{
	private:
		GXUIStaticText*		widget;

	public:
		explicit EMUIStaticText ( EMUI* parent );
		~EMUIStaticText () override;

		GXWidget* GetWidget () const override;
		GXVoid OnDrawMask () override;

		GXVoid SetText ( const GXWChar* text );
		const GXWChar* GetText () const;

		GXVoid SetAlingment ( eGXUITextAlignment alignment );
		eGXUITextAlignment GetAlignment () const;

		GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );
};


#endif //EM_UI_STATIC_TEXT
