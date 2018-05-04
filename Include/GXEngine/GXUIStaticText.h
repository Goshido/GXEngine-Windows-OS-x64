// version 1.1

#ifndef GX_UI_STATIC_TEXT
#define GX_UI_STATIC_TEXT


#include "GXWidget.h"


enum eGXUITextAlignment : GXUByte
{
	Left,
	Right,
	Center
};


class GXUIStaticText : public GXWidget
{
	private:
		GXWChar*				text;
		GXColorRGB				textColor;
		eGXUITextAlignment		alignment;

	public:
		explicit GXUIStaticText ( GXWidget* parent );
		~GXUIStaticText () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXVoid SetText ( const GXWChar* newText );
		GXVoid SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		GXVoid SetAlignment ( eGXUITextAlignment newAlignment );

		const GXWChar* GetText () const;
		const GXColorRGB& GetTextColor () const;
		eGXUITextAlignment GetAlignment () const;
		
	private:
		GXUIStaticText () = delete;
		GXUIStaticText ( const GXUIStaticText &other ) = delete;
		GXUIStaticText& operator = ( const GXUIStaticText &other ) = delete;
};


#endif // GX_UI_STATIC_TEXT
