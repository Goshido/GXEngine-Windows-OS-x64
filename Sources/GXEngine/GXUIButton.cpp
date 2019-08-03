// version 1.5

#include <GXEngine/GXUIButton.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>


GXUIButton::GXUIButton ( GXWidget* parent ):
    GXWidget ( parent ),
    _onLeftMouseButton ( nullptr ),
    _context ( nullptr ),
    _isPressed ( GX_FALSE ),
    _isHighlighted ( GX_FALSE ),
    _isDisabled ( GX_FALSE )
{
    // NOTHING
}

GXUIButton::~GXUIButton ()
{
    // NOTHING
}

GXVoid GXUIButton::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    if ( message == eGXUIMessage::LMBDown )
    {
        if ( _isDisabled ) return;

        _isPressed = GX_TRUE;

        if ( _renderer )
            _renderer->OnUpdate ();

        if ( !_onLeftMouseButton ) return;

        const GXVec2* position = static_cast<const GXVec2*> ( data );
        _onLeftMouseButton ( _context, *this, position->GetX (), position->GetY (), eGXMouseButtonState::Down );

        return;
    }

    if ( message == eGXUIMessage::LMBUp )
    {
        if ( _isDisabled ) return;

        _isPressed = GX_FALSE;

        if ( _renderer )
            _renderer->OnUpdate ();

        if ( !_onLeftMouseButton ) return;

        const GXVec2* position = static_cast<const GXVec2*> ( data );
        _onLeftMouseButton ( _context, *this, position->GetX (), position->GetY (), eGXMouseButtonState::Up );

        return;
    }

    if ( message == eGXUIMessage::MouseOver )
    {
        if ( _isDisabled ) return;

        _isHighlighted = GX_TRUE;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::MouseLeave )
    {
        if ( _isDisabled ) return;

        _isHighlighted = GX_FALSE;
        _isPressed = GX_FALSE;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::Enable )
    {
        if ( !_isDisabled ) return;

        _isDisabled = GX_FALSE;
        _isHighlighted = GX_FALSE;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::Disable )
    {
        if ( _isDisabled ) return;

        _isDisabled = GX_TRUE;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::Redraw )
    {
        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    GXWidget::OnMessage ( message, data );
}

GXVoid GXUIButton::SetOnLeftMouseButtonCallback ( GXVoid* context, GXUIButtonOnMouseButtonHandler callback )
{
    _context = context;
    _onLeftMouseButton = callback;
}

GXVoid GXUIButton::Enable ()
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Enable, nullptr, 0u );
}

GXVoid GXUIButton::Disable ()
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Disable, nullptr, 0u );
}

GXVoid GXUIButton::Redraw ()
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Redraw, nullptr, 0u );
}

GXBool GXUIButton::IsPressed () const
{
    return _isPressed;
}

GXBool GXUIButton::IsHighlighted () const
{
    return _isHighlighted;
}

GXBool GXUIButton::IsDisabled () const
{
    return _isDisabled;
}
