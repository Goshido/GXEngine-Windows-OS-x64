// version 1.5

#include <GXEngine/GXUIInput.h>
#include <GXEngine/GXUIMessage.h>


GXUIInput::GXUIInput ( GXWidget* parent, GXBool isNeedRegister ):
    GXWidget ( parent, isNeedRegister ),
    _onLMBDown ( nullptr ),
    _onLMBUp ( nullptr ),
    _onMMBDown ( nullptr ),
    _onMMBUp ( nullptr ),
    _onRMBDown ( nullptr ),
    _onRMBUp ( nullptr ),
    _onDoubleClick ( nullptr ),
    _onMouseMove ( nullptr ),
    _onMouseScroll ( nullptr ),
    _onMouseOver ( nullptr ),
    _onMouseLeave ( nullptr ),
    _onKeyDown ( nullptr ),
    _onKeyUp ( nullptr ),
    _handler ( nullptr )
{
    // NOTHING
}

GXUIInput::~GXUIInput ()
{
    // NOTHING
}

GXVoid GXUIInput::OnMessage ( eGXUIMessage message, const GXVoid* data )
{

    if ( message == eGXUIMessage::LMBDown )
    {
        if ( !_onLMBDown ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onLMBDown ( _handler, *this, v->GetX (), v->GetY () );
        return;
    }

    if ( message == eGXUIMessage::LMBUp )
    {
        if ( !_onLMBUp ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onLMBUp ( _handler, *this, v->GetX (), v->GetY () );

        return;
    }
    
    if ( message == eGXUIMessage::MMBDown )
    {
        if ( !_onMMBDown ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onMMBDown ( _handler, *this, v->GetX (), v->GetY () );
        return;
    }

    if ( message == eGXUIMessage::MMBUp )
    {
        if ( !_onMMBUp ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onMMBUp ( _handler, *this, v->GetX (), v->GetY () );
        return;
    }

    if ( message == eGXUIMessage::RMBDown )
    {
        if ( !_onRMBDown ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onRMBDown ( _handler, *this, v->GetX (), v->GetY () );
        return;
    }

    if ( message == eGXUIMessage::RMBUp )
    {
        if ( !_onRMBUp ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onRMBUp ( _handler, *this, v->GetX (), v->GetY () );
        return;
    }

    if ( message == eGXUIMessage::Scroll )
    {
        if ( !_onMouseScroll ) return;

        const GXVec3* v = static_cast<const GXVec3*> ( data );
        _onMouseScroll ( _handler, *this, v->GetX (), v->GetY (), v->GetZ () );
        return;
    }

    if ( message == eGXUIMessage::MouseMove )
    {
        if ( !_onMouseMove ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onMouseMove ( _handler, *this, v->GetX (), v->GetY () );
        return;
    }

    if ( message == eGXUIMessage::MouseOver )
    {
        if ( !_onMouseOver ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onMouseOver ( _handler, *this, v->GetX (), v->GetY () );
        return;
    }

    if ( message == eGXUIMessage::MouseLeave )
    {
        if ( !_onMouseLeave ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onMouseLeave ( _handler, *this, v->GetX (), v->GetY () );
        return;
    }

    if ( message == eGXUIMessage::DoubleClick )
    {
        if ( !_onDoubleClick ) return;

        const GXVec2* v = static_cast<const GXVec2*> ( data );
        _onDoubleClick ( _handler, *this, v->GetX (), v->GetY () );
        return;
    }

    if ( message == eGXUIMessage::KeyDown )
    {
        if ( !_onKeyDown ) return;

        const GXInt* keyCode = static_cast<const GXInt*> ( data );
        _onKeyDown ( _handler, *this, *keyCode );
        return;
    }

    if ( message == eGXUIMessage::KeyUp )
    {
        if ( !_onKeyUp ) return;

        const GXInt* keyCode = static_cast<const GXInt*> ( data );
        _onKeyUp ( _handler, *this, *keyCode );
        return;
    }

    GXWidget::OnMessage ( message, data );
}

GXVoid GXUIInput::SetOnLeftMouseButtonDownCallback ( GXUIInputOnMouseButtonHandler callback )
{
    _onLMBDown = callback;
}

GXVoid GXUIInput::SetOnLeftMouseButtonUpCallback ( GXUIInputOnMouseButtonHandler callback )
{
    _onLMBUp = callback;
}

GXVoid GXUIInput::SetOnMiddleMouseButtonDownCallback ( GXUIInputOnMouseButtonHandler callback )
{
    _onMMBDown = callback;
}

GXVoid GXUIInput::SetOnMiddleMouseButtonUpCallback ( GXUIInputOnMouseButtonHandler callback )
{
    _onMMBUp = callback;
}

GXVoid GXUIInput::SetOnRightMouseButtonDownCallback ( GXUIInputOnMouseButtonHandler callback )
{
    _onRMBDown = callback;
}

GXVoid GXUIInput::SetOnRightMouseButtonUpCallback ( GXUIInputOnMouseButtonHandler callback )
{
    _onRMBUp = callback;
}

GXVoid GXUIInput::SetOnDoubleClickCallback ( GXUIInputOnDoubleClickHandler callback )
{
    _onDoubleClick = callback;
}

GXVoid GXUIInput::SetOnMouseMoveCallback ( GXUIInputOnMouseMoveHandler callback )
{
    _onMouseMove = callback;
}

GXVoid GXUIInput::SetOnMouseScrollCallback ( GXUIInputOnScrollHandler callback )
{
    _onMouseScroll = callback;
}

GXVoid GXUIInput::SetOnMouseOverCallback ( GXUIInputOnMouseOverHandler callback )
{
    _onMouseOver = callback;
}

GXVoid GXUIInput::SetOnMouseLeaveCallback ( GXUIInputOnMouseLeaveHandler callback )
{
    _onMouseLeave = callback;
}

GXVoid GXUIInput::SetOnKeyDownCallback ( GXUIInputOnKeyHandler callback )
{
    _onKeyDown = callback;
}

GXVoid GXUIInput::SetOnKeyUpCallback ( GXUIInputOnKeyHandler callback )
{
    _onKeyUp = callback;
}

GXVoid GXUIInput::SetHandler ( GXVoid* handler )
{
    _handler = handler;
}
