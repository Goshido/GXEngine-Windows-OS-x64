// version 1.7

#include <GXEngine/GXUIInput.h>
#include <GXEngine/GXUIMessage.h>


GXUIInputMessageHandlerNode::GXUIInputMessageHandlerNode ()
{
    // NOTHING
}

GXUIInputMessageHandlerNode::GXUIInputMessageHandlerNode ( eGXUIMessage message ):
    GXUIWidgetMessageHandlerNode ( message )
{
    // NOTHING
}

GXUIInputMessageHandlerNode::~GXUIInputMessageHandlerNode ()
{
    // NOTHING
}

GXVoid GXUIInputMessageHandlerNode::Init ( GXUIInput &input, eGXUIMessage message, GXUIInputOnMessageHandler handler )
{
    _message = message;
    _handler = handler;
    _widget = &input;
}

GXVoid GXUIInputMessageHandlerNode::HandleMassage ( const GXVoid* data )
{
    GXUIInput* input = static_cast<GXUIInput*> ( _widget );

    // Note this is C++ syntax for invoke class method.
    ( input->*_handler ) ( data );
}

//---------------------------------------------------------------------------------------------------------------------

GXUIInput::GXUIInput ( GXWidget* parent, GXBool isNeedRegister ):
    GXWidget ( parent, isNeedRegister ),
    _onLMBDown ( nullptr ),
    _onLMBUp ( nullptr ),
    _onMMBDown ( nullptr ),
    _onMMBUp ( nullptr ),
    _onRMBDown ( nullptr ),
    _onRMBUp ( nullptr ),
    _onDoubleClick ( nullptr ),
    _onMouseScroll ( nullptr ),
    _onMouseMove ( nullptr ),
    _onMouseOver ( nullptr ),
    _onMouseLeave ( nullptr ),
    _onKeyDown ( nullptr ),
    _onKeyUp ( nullptr ),
    _handler ( nullptr )
{
    InitMessageHandlers ();
}

GXUIInput::~GXUIInput ()
{
    // NOTHING
}

GXVoid GXUIInput::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    GXUIInputMessageHandlerNode probe ( message );
    GXAVLTreeNode* findResult = _messageHandlerTree.Find ( probe );

    if ( !findResult )
    {
        GXWidget::OnMessage ( message, data );
        return;
    }

    GXUIInputMessageHandlerNode* targetHandler = static_cast<GXUIInputMessageHandlerNode*> ( findResult );
    targetHandler->HandleMassage ( data );
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

GXVoid GXUIInput::InitMessageHandlers ()
{
    _messageHandlers[ 0u ].Init ( *this, eGXUIMessage::LMBDown, &GXUIInput::OnLMBDown );
    _messageHandlerTree.Add ( _messageHandlers[ 0u ] );

    _messageHandlers[ 1u ].Init ( *this, eGXUIMessage::LMBUp, &GXUIInput::OnLMBUp );
    _messageHandlerTree.Add ( _messageHandlers[ 1u ] );

    _messageHandlers[ 2u ].Init ( *this, eGXUIMessage::MMBDown, &GXUIInput::OnMMBDown );
    _messageHandlerTree.Add ( _messageHandlers[ 2u ] );

    _messageHandlers[ 3u ].Init ( *this, eGXUIMessage::MMBUp, &GXUIInput::OnMMBUp );
    _messageHandlerTree.Add ( _messageHandlers[ 3u ] );

    _messageHandlers[ 4u ].Init ( *this, eGXUIMessage::RMBDown, &GXUIInput::OnRMBDown );
    _messageHandlerTree.Add ( _messageHandlers[ 4u ] );

    _messageHandlers[ 5u ].Init ( *this, eGXUIMessage::RMBUp, &GXUIInput::OnRMBUp );
    _messageHandlerTree.Add ( _messageHandlers[ 5u ] );

    _messageHandlers[ 6u ].Init ( *this, eGXUIMessage::Scroll, &GXUIInput::OnScroll );
    _messageHandlerTree.Add ( _messageHandlers[ 6u ] );

    _messageHandlers[ 7u ].Init ( *this, eGXUIMessage::MouseMove, &GXUIInput::OnMouseMove );
    _messageHandlerTree.Add ( _messageHandlers[ 7u ] );

    _messageHandlers[ 8u ].Init ( *this, eGXUIMessage::MouseOver, &GXUIInput::OnMouseOver );
    _messageHandlerTree.Add ( _messageHandlers[ 8u ] );

    _messageHandlers[ 9u ].Init ( *this, eGXUIMessage::MouseLeave, &GXUIInput::OnMouseLeave );
    _messageHandlerTree.Add ( _messageHandlers[ 9u ] );

    _messageHandlers[ 10u ].Init ( *this, eGXUIMessage::DoubleClick, &GXUIInput::OnDoubleClick );
    _messageHandlerTree.Add ( _messageHandlers[ 10u ] );

    _messageHandlers[ 11u ].Init ( *this, eGXUIMessage::KeyDown, &GXUIInput::OnKeyDown );
    _messageHandlerTree.Add ( _messageHandlers[ 11u ] );

    _messageHandlers[ 12u ].Init ( *this, eGXUIMessage::KeyUp, &GXUIInput::OnKeyUp );
    _messageHandlerTree.Add ( _messageHandlers[ 12u ] );
}

GXVoid GXUIInput::OnLMBDown ( const GXVoid* data )
{
    if ( !_onLMBDown ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onLMBDown ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ] );
}

GXVoid GXUIInput::OnLMBUp ( const GXVoid* data )
{
    if ( !_onLMBUp ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onLMBUp ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ] );
}

GXVoid GXUIInput::OnMMBDown ( const GXVoid* data )
{
    if ( !_onMMBDown ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onMMBDown ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ] );
}

GXVoid GXUIInput::OnMMBUp ( const GXVoid* data )
{
    if ( !_onMMBUp ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onMMBUp ( _handler, *this, v->_data[ 0u ], v->_data[ 0u ] );
}

GXVoid GXUIInput::OnRMBDown ( const GXVoid* data )
{
    if ( !_onRMBDown ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onRMBDown ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ] );
}

GXVoid GXUIInput::OnRMBUp ( const GXVoid* data )
{
    if ( !_onRMBUp ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onRMBUp ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ] );
}

GXVoid GXUIInput::OnScroll ( const GXVoid* data )
{
    if ( !_onMouseScroll ) return;

    const GXVec3* v = static_cast<const GXVec3*> ( data );
    _onMouseScroll ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ], v->_data[ 2u ] );
}

GXVoid GXUIInput::OnMouseMove ( const GXVoid* data )
{
    if ( !_onMouseMove ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onMouseMove ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ] );
}

GXVoid GXUIInput::OnMouseOver ( const GXVoid* data )
{
    if ( !_onMouseOver ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onMouseOver ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ] );
}

GXVoid GXUIInput::OnMouseLeave ( const GXVoid* data )
{
    if ( !_onMouseLeave ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onMouseLeave ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ] );
}

GXVoid GXUIInput::OnDoubleClick ( const GXVoid* data )
{
    if ( !_onDoubleClick ) return;

    const GXVec2* v = static_cast<const GXVec2*> ( data );
    _onDoubleClick ( _handler, *this, v->_data[ 0u ], v->_data[ 1u ] );
}

GXVoid GXUIInput::OnKeyDown ( const GXVoid* data )
{
    if ( !_onKeyDown ) return;

    const GXInt* keyCode = static_cast<const GXInt*> ( data );
    _onKeyDown ( _handler, *this, *keyCode );
}

GXVoid GXUIInput::OnKeyUp ( const GXVoid* data )
{
    if ( !_onKeyUp ) return;

    const GXInt* keyCode = static_cast<const GXInt*> ( data );
    _onKeyUp ( _handler, *this, *keyCode );
}
