// version 1.10

#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXSmartLock.h>


#define GX_UI_MESSAGE_BUFFER_SIZE        1048576u    // 1 Mb

//---------------------------------------------------------------------------------------------------------------------

GXFloat             gx_ui_Scale = 1.0f;

GXSmartLock*        gx_ui_SmartLock = nullptr;
GXCircleBuffer*     gx_ui_MessageBuffer = nullptr;

struct GXMessage
{
    GXMessage*      _next;
    GXWidget*       _widget;
    eGXUIMessage    _message;
    GXVoid*         _data;
    GXUInt          _size;
};

//---------------------------------------------------------------------------------------------------------------------

GXTouchSurface* GXTouchSurface::_instance = nullptr;

GXTouchSurface& GXCALL GXTouchSurface::GetInstance ()
{
    if ( !_instance )
        _instance = new GXTouchSurface ();

    return *_instance;
}

GXTouchSurface::~GXTouchSurface ()
{
    delete gx_ui_MessageBuffer;
    DeleteWidgets ();
    delete gx_ui_SmartLock;
    _instance = nullptr;
}

GXVoid GXTouchSurface::OnLeftMouseButtonDown ( const GXVec2 &position )
{
    _mousePosition = position;

    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::LMBDown, &position, sizeof ( GXVec2 ) );
        SendMessage ( _lockedWidget, eGXUIMessage::Foreground, nullptr, 0u );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::LMBDown, &position, sizeof ( GXVec2 ) );
    SendMessage ( target, eGXUIMessage::Foreground, nullptr, 0u );
}

GXVoid GXTouchSurface::OnLeftMouseButtonUp ( const GXVec2 &position )
{
    _mousePosition = position;

    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::LMBUp, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::LMBUp, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnMiddleMouseButtonDown ( const GXVec2 &position )
{
    _mousePosition = position;

    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::MMBDown, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::MMBDown, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnMiddleMouseButtonUp ( const GXVec2 &position )
{
    _mousePosition = position;

    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::MMBUp, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::MMBUp, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnRightMouseButtonDown ( const GXVec2 &position )
{
    _mousePosition = position;

    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::RMBDown, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::RMBDown, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnRightMouseButtonUp ( const GXVec2 &position )
{
    _mousePosition = position;

    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::RMBUp, &position, sizeof ( GXVec2 ) );
        return;
    }
    
    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::RMBUp, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnDoubleClick ( const GXVec2 &position )
{
    _mousePosition = position;

    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::DoubleClick, &position, sizeof ( GXVec2 ) );
        SendMessage ( _lockedWidget, eGXUIMessage::Foreground, nullptr, 0u );
        return;
    }
    
    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::DoubleClick, &position, sizeof ( GXVec2 ) );
    SendMessage ( target, eGXUIMessage::Foreground, nullptr, 0u );
}

GXVoid GXTouchSurface::OnScroll ( const GXVec2 &position, GXFloat scroll )
{
    _mousePosition = position;

    if ( _lockedWidget )
    {
        GXVec3 data ( position.GetX (), position.GetY (), scroll );
        SendMessage ( _lockedWidget, eGXUIMessage::Scroll, &data, sizeof ( GXVec3 ) );
        return;
    }
    
    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    GXVec3 data ( position.GetX (), position.GetY (), scroll );
    SendMessage ( target, eGXUIMessage::Scroll, &data, sizeof ( GXVec3 ) );
}

GXVoid GXTouchSurface::OnMouseMove ( const GXVec2 &position )
{
    _mousePosition = position;

    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::MouseMove, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( target != _mouseOverWidget )
    {
        if ( _mouseOverWidget )
            SendMessage ( _mouseOverWidget, eGXUIMessage::MouseLeave, &position, sizeof ( GXVec2 ) );

        _mouseOverWidget = target;

        if ( target )
            SendMessage ( target, eGXUIMessage::MouseOver, &position, sizeof ( GXVec2 ) );
    }

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::MouseMove, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnKeyDown ( GXInt keyCode )
{
    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::KeyDown, &keyCode, sizeof ( GXInt ) );
        return;
    }

    GXWidget* target = FindWidget ( _mousePosition );

    if ( target != _mouseOverWidget )
    {
        if ( _mouseOverWidget )
            SendMessage ( _mouseOverWidget, eGXUIMessage::MouseLeave, &_mousePosition, sizeof ( GXVec2 ) );

        _mouseOverWidget = target;

        if ( target )
        {
            SendMessage ( target, eGXUIMessage::MouseOver, &_mousePosition, sizeof ( GXVec2 ) );
        }
    }

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::KeyDown, &keyCode, sizeof ( GXInt ) );
}

GXVoid GXTouchSurface::OnKeyUp ( GXInt keyCode )
{
    if ( _lockedWidget )
    {
        SendMessage ( _lockedWidget, eGXUIMessage::KeyUp, &keyCode, sizeof ( GXInt ) );
        return;
    }

    GXWidget* target = FindWidget ( _mousePosition );

    if ( target != _mouseOverWidget )
    {
        if ( _mouseOverWidget )
            SendMessage ( _mouseOverWidget, eGXUIMessage::MouseLeave, &_mousePosition, sizeof ( GXVec2 ) );

        _mouseOverWidget = target;

        if ( target )
        {
            SendMessage ( target, eGXUIMessage::MouseOver, &_mousePosition, sizeof ( GXVec2 ) );
        }
    }

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::KeyUp, &keyCode, sizeof ( GXInt ) );
}

GXVoid GXTouchSurface::SendMessage ( GXWidget* widget, eGXUIMessage message, const GXVoid* data, GXUInt size )
{
    GXMessage* msg = static_cast<GXMessage*> ( gx_ui_MessageBuffer->Allocate ( sizeof ( GXMessage ) ) );
    msg->_next = nullptr;
    msg->_widget = widget;
    msg->_message = message;
    msg->_data = gx_ui_MessageBuffer->Allocate ( size );

    if ( size )
        memcpy ( msg->_data, data, size );

    msg->_size = size;

    gx_ui_SmartLock->AcquireExclusive ();

    if ( !_messages )
    {
        _messages = _lastMessage = msg;
    }
    else
    {
        _lastMessage->_next = msg;
        _lastMessage = msg;
    }

    gx_ui_SmartLock->ReleaseExclusive ();
}

GXVoid GXTouchSurface::ExecuteMessages ()
{
    while ( _messages )
    {
        GXMessage* msg = _messages;

        gx_ui_SmartLock->AcquireExclusive ();
        _messages = _messages->_next;
        gx_ui_SmartLock->ReleaseExclusive ();

        msg->_widget->OnMessage ( msg->_message, msg->_data );
    }
}

GXVoid GXTouchSurface::DrawWidgets ()
{
    gx_ui_SmartLock->AcquireShared ();
    DrawWidgets ( _widgetTail );
    gx_ui_SmartLock->ReleaseShared ();
}

GXVoid GXTouchSurface::MoveWidgetToForeground ( GXWidget* widget )
{
    if ( !widget || widget == _defaultWidget || widget == _widgetHead || widget->_parent ) return;

    UnRegisterWidget ( widget );
    
    widget->_previous = nullptr;
    widget->_next = _widgetHead;
    _widgetHead->_previous = widget;
    _widgetHead = widget;
}

GXVoid GXTouchSurface::LockCursor ( GXWidget* widget )
{
    _lockedWidget = widget;
}

GXWidget* GXTouchSurface::GetLockedCursorWidget ()
{
    return _lockedWidget;
}

GXVoid GXTouchSurface::ReleaseCursor ()
{
    _lockedWidget = nullptr;
}

GXVoid GXTouchSurface::RegisterWidget ( GXWidget* widget )
{
    if ( !widget || widget->_parent ) return;

    widget->_previous = _widgetTail;

    if ( _widgetTail )
        _widgetTail->_next = widget;
    else
        _widgetHead = widget;

    _widgetTail = widget;
}

GXVoid GXTouchSurface::UnRegisterWidget ( GXWidget* widget )
{
    if ( !widget || widget->_parent ) return;

    if ( widget->_next )
        widget->_next->_previous = widget->_previous;
    else
        _widgetTail = widget->_previous;

    if ( widget->_previous )
    {
        widget->_previous->_next = widget->_next;
        return;
    }

    _widgetHead = widget->_next;
}

const GXVec2& GXTouchSurface::GetMousePosition ()
{
    return _mousePosition;
}

GXVoid GXTouchSurface::SetDefaultWidget ( GXWidget* widget )
{
    _defaultWidget = widget;
}

GXTouchSurface::GXTouchSurface ()
{
    _messages = _lastMessage = nullptr;
    _widgetHead = _widgetTail = _mouseOverWidget = _lockedWidget = _defaultWidget = nullptr;

    gx_ui_SmartLock = new GXSmartLock ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXCircleBuffer" );
    gx_ui_MessageBuffer = new GXCircleBuffer ( GX_UI_MESSAGE_BUFFER_SIZE );
}

GXVoid GXTouchSurface::DeleteWidgets ()
{
    while ( _widgetHead )
    {
        UnRegisterWidget ( _widgetHead );
    }
}

GXWidget* GXTouchSurface::FindWidget ( const GXVec2 &position )
{
    GXVec3 v ( position.GetX (), position.GetY (), 0.0f );

    gx_ui_SmartLock->AcquireShared ();

    GXWidgetIterator iterator;
    GXWidget* p = iterator.Init ( _widgetHead );

    while ( p )
    {
        if ( p->IsVisible () && p->GetBoundsWorld ().IsOverlaped ( v ) )
        {
            gx_ui_SmartLock->ReleaseShared ();
            return p->FindWidget ( position );
        }

        p = iterator.GetNext ();
    }

    gx_ui_SmartLock->ReleaseShared ();

    return _defaultWidget;
}

GXVoid GXTouchSurface::DrawWidgets ( GXWidget* widget )
{
    GXWidgetIterator iterator;
    GXWidget* p = iterator.Init ( widget );

    while ( p )
    {
        if ( p->_isVisible )
            p->OnDraw ();

        p = iterator.GetPrevious ();
    }
}
