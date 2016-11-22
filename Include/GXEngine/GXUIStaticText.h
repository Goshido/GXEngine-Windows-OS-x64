//version 1.0

#ifndef GX_UI_STATIC_TEXT
#define GX_UI_STATIC_TEXT


#include "GXWidget.h"


enum eGXUITextAlignment : GXUByte
{
	GX_UI_TEXT_ALIGNMENT_LEFT,
	GX_UI_TEXT_ALIGNMENT_RIGHT,
	GX_UI_TEXT_ALIGNMENT_CENTER
};


class GXUIStaticText : public GXWidget
{
	private:
		GXWChar*					text;
		eGXUITextAlignment	alignment;

	public:
		GXUIStaticText ( GXWidget* parent );
		virtual ~GXUIStaticText ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid SetText ( const GXWChar* text );
		GXVoid SetAlignment ( eGXUITextAlignment alignment );

		const GXWChar* GetText ();
		eGXUITextAlignment GetAlignment ();
};


#endif //GX_UI_STATIC_TEXT
