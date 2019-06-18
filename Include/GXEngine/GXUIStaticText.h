// version 1.4

#ifndef GX_UI_STATIC_TEXT
#define GX_UI_STATIC_TEXT


#include "GXWidget.h"


enum eGXUITextAlignment : GXUByte
{
    Left,
    Right,
    Center
};

class GXUIStaticText final : public GXWidget
{
    private:
        GXWChar*                _text;
        GXColorRGB              _textColor;
        eGXUITextAlignment      _alignment;

    public:
        explicit GXUIStaticText ( GXWidget* parent );
        ~GXUIStaticText () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

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
