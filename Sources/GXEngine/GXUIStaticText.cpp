// version 1.5

#include <GXEngine/GXUIStaticText.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>
#include <GXCommon/GXStrings.h>


#define DEFAULT_UI_ALIGNMENT    eGXUITextAlignment::Left

#define DEFAULT_TEXT_COLOR_R    115u
#define DEFAULT_TEXT_COLOR_G    185u
#define DEFAULT_TEXT_COLOR_B    0u
#define DEFAULT_TEXT_COLOR_A    255u

//---------------------------------------------------------------------------------------------------------------------

GXUIStaticTextMessageHandlerNode::GXUIStaticTextMessageHandlerNode ()
{
    // NOTHING
}

GXUIStaticTextMessageHandlerNode::GXUIStaticTextMessageHandlerNode ( eGXUIMessage message ):
    GXUIWidgetMessageHandlerNode ( message )
{
    // NOTHING
}

GXUIStaticTextMessageHandlerNode::~GXUIStaticTextMessageHandlerNode ()
{
    // NOTHING
}

GXVoid GXUIStaticTextMessageHandlerNode::Init ( GXUIStaticText &staticText, eGXUIMessage message, GXUIStaticTextOnMessageHandler handler )
{
    _message = message;
    _handler = handler;
    _widget = &staticText;
}

GXVoid GXUIStaticTextMessageHandlerNode::HandleMassage ( const GXVoid* data )
{
    GXUIStaticText* staticText = static_cast<GXUIStaticText*> ( _widget );

    // Note this is C++ syntax for invoke class method.
    ( staticText->*_handler ) ( data );
}

//---------------------------------------------------------------------------------------------------------------------

GXUIStaticText::GXUIStaticText ( GXWidget* parent ):
    GXWidget ( parent ),
    _textColor ( static_cast<GXUByte> ( DEFAULT_TEXT_COLOR_R ), static_cast<GXUByte> ( DEFAULT_TEXT_COLOR_G ), static_cast<GXUByte> ( DEFAULT_TEXT_COLOR_B ), static_cast<GXUByte> ( DEFAULT_TEXT_COLOR_A ) ),
    _alignment ( DEFAULT_UI_ALIGNMENT )
{
    InitMessageHandlers ();
}

GXUIStaticText::~GXUIStaticText ()
{
    // NOTHING 
}

GXVoid GXUIStaticText::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    GXUIStaticTextMessageHandlerNode probe ( message );
    GXAVLTreeNode* findResult = _messageHandlerTree.Find ( probe );

    if ( !findResult )
    {
        GXWidget::OnMessage ( message, data );
        return;
    }

    GXUIStaticTextMessageHandlerNode* targetHandler = static_cast<GXUIStaticTextMessageHandlerNode*> ( findResult );
    targetHandler->HandleMassage ( data );
}

GXVoid GXUIStaticText::SetText ( const GXString &text )
{
    if ( text.IsNull () )
    {
        GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ClearText, nullptr, 0u );
        return;
    }

    // Kung-Fu: Task is to create pesistent GXString in raw memory.
    // So will be used placement new. This prevents calling destructor for GXString object.
    // The ownership will be transmitted manualy in OnSetText. The temp object will be destructed in OnSetText.

    GXUByte memory[ sizeof ( GXString ) ];
    const GXString* string = ::new ( memory ) GXString ( text );
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::SetText, string, static_cast<GXUInt> ( sizeof ( GXString ) ) );
}

GXVoid GXUIStaticText::SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    GXColorRGB c;
    c.From ( red, green, blue, alpha );
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::SetTextColor, &c, sizeof ( GXColorRGB ) );
}

GXVoid GXUIStaticText::SetAlignment ( eGXUITextAlignment newAlignment )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::SetTextAlignment, &newAlignment, sizeof ( eGXUITextAlignment ) );
}

const GXString& GXUIStaticText::GetText () const
{
    return _text;
}

const GXColorRGB& GXUIStaticText::GetTextColor () const
{
    return _textColor;
}

eGXUITextAlignment GXUIStaticText::GetAlignment () const
{
    return _alignment;
}

GXVoid GXUIStaticText::InitMessageHandlers ()
{
    _messageHandlers[ 0u ].Init ( *this, eGXUIMessage::ClearText, &GXUIStaticText::OnClearText );
    _messageHandlerTree.Add ( _messageHandlers[ 0u ] );

    _messageHandlers[ 1u ].Init ( *this, eGXUIMessage::SetText, &GXUIStaticText::OnSetText );
    _messageHandlerTree.Add ( _messageHandlers[ 1u ] );

    _messageHandlers[ 2u ].Init ( *this, eGXUIMessage::SetTextAlignment, &GXUIStaticText::OnSetTextAlignment );
    _messageHandlerTree.Add ( _messageHandlers[ 2u ] );

    _messageHandlers[ 3u ].Init ( *this, eGXUIMessage::SetTextColor, &GXUIStaticText::OnSetTextColor );
    _messageHandlerTree.Add ( _messageHandlers[ 3u ] );
}

GXVoid GXUIStaticText::OnClearText ( const GXVoid* /*data*/ )
{
    _text.Clear ();

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIStaticText::OnSetText ( const GXVoid* data )
{
    const GXString* newText = static_cast<const GXString*> ( data );
    _text = *newText;

    // Clean up placement new stuff.
    // see GXUIStaticText::SetText method.
    newText->~GXString ();

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIStaticText::OnSetTextAlignment ( const GXVoid* data )
{
    const eGXUITextAlignment* newAlignment = static_cast<const eGXUITextAlignment*> ( data );
    _alignment = *newAlignment;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIStaticText::OnSetTextColor ( const GXVoid* data )
{
    memcpy ( &_textColor, data, sizeof ( GXVec4 ) );

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}
