// version 1.5

#ifndef GX_UI_STATIC_TEXT
#define GX_UI_STATIC_TEXT


#include "GXWidget.h"
#include <GXCommon/GXStrings.h>


class GXUIStaticText;
typedef GXVoid ( GXUIStaticText::* GXUIStaticTextOnMessageHandler ) ( const GXVoid* data );

class GXUIStaticTextMessageHandlerNode final : public GXUIWidgetMessageHandlerNode
{
    private:
        GXUIStaticTextOnMessageHandler     _handler;

    public:
        GXUIStaticTextMessageHandlerNode ();

        // Special probe constructor.
        explicit GXUIStaticTextMessageHandlerNode ( eGXUIMessage message );
        ~GXUIStaticTextMessageHandlerNode () override;

        GXVoid HandleMassage ( const GXVoid* data ) override;

        GXVoid Init ( GXUIStaticText &staticText, eGXUIMessage message, GXUIStaticTextOnMessageHandler handler );

    private:
        GXUIStaticTextMessageHandlerNode ( const GXUIStaticTextMessageHandlerNode &other ) = delete;
        GXUIStaticTextMessageHandlerNode& operator = ( const GXUIStaticTextMessageHandlerNode &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

enum class eGXUITextAlignment : GXUByte
{
    Left,
    Right,
    Center
};

class GXUIStaticText final : public GXWidget
{
    private:
        GXColorRGB                          _textColor;
        eGXUITextAlignment                  _alignment;

        GXString                            _text;
        GXUIStaticTextMessageHandlerNode    _messageHandlers[ 4u ];

    public:
        explicit GXUIStaticText ( GXWidget* parent );
        ~GXUIStaticText () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

        GXVoid SetText ( const GXString &text );
        GXVoid SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
        GXVoid SetAlignment ( eGXUITextAlignment newAlignment );

        const GXString& GetText () const;
        const GXColorRGB& GetTextColor () const;
        eGXUITextAlignment GetAlignment () const;

    private:
        GXVoid InitMessageHandlers ();

        // Message handlers
        GXVoid OnClearText ( const GXVoid* data );
        GXVoid OnSetText ( const GXVoid* data );
        GXVoid OnSetTextAlignment ( const GXVoid* data );
        GXVoid OnSetTextColor ( const GXVoid* data );

        GXUIStaticText () = delete;
        GXUIStaticText ( const GXUIStaticText &other ) = delete;
        GXUIStaticText& operator = ( const GXUIStaticText &other ) = delete;
};


#endif // GX_UI_STATIC_TEXT
