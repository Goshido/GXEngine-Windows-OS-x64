// version 1.9

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
    GXMessage*      next;
    GXWidget*       widget;
    eGXUIMessage    message;
    GXVoid*         data;
    GXUInt          size;
};

//---------------------------------------------------------------------------------------------------------------------

GXTouchSurface* GXTouchSurface::instance = nullptr;

GXTouchSurface& GXCALL GXTouchSurface::GetInstance ()
{
    if ( !instance )
        instance = new GXTouchSurface ();

    return *instance;
}

GXTouchSurface::~GXTouchSurface ()
{
    delete gx_ui_MessageBuffer;
    DeleteWidgets ();
    delete gx_ui_SmartLock;
    instance = nullptr;
}

GXVoid GXTouchSurface::OnLeftMouseButtonDown ( const GXVec2 &position )
{
    mousePosition = position;

    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::LMBDown, &position, sizeof ( GXVec2 ) );
        SendMessage ( lockedWidget, eGXUIMessage::Foreground, nullptr, 0u );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::LMBDown, &position, sizeof ( GXVec2 ) );
    SendMessage ( target, eGXUIMessage::Foreground, nullptr, 0u );
}

GXVoid GXTouchSurface::OnLeftMouseButtonUp ( const GXVec2 &position )
{
    mousePosition = position;

    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::LMBUp, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::LMBUp, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnMiddleMouseButtonDown ( const GXVec2 &position )
{
    mousePosition = position;

    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::MMBDown, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::MMBDown, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnMiddleMouseButtonUp ( const GXVec2 &position )
{
    mousePosition = position;

    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::MMBUp, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::MMBUp, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnRightMouseButtonDown ( const GXVec2 &position )
{
    mousePosition = position;

    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::RMBDown, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::RMBDown, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnRightMouseButtonUp ( const GXVec2 &position )
{
    mousePosition = position;

    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::RMBUp, &position, sizeof ( GXVec2 ) );
        return;
    }
    
    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::RMBUp, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnDoubleClick ( const GXVec2 &position )
{
    mousePosition = position;

    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::DoubleClick, &position, sizeof ( GXVec2 ) );
        SendMessage ( lockedWidget, eGXUIMessage::Foreground, nullptr, 0u );
        return;
    }
    
    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::DoubleClick, &position, sizeof ( GXVec2 ) );
    SendMessage ( target, eGXUIMessage::Foreground, nullptr, 0u );
}

GXVoid GXTouchSurface::OnScroll ( const GXVec2 &position, GXFloat scroll )
{
    mousePosition = position;

    if ( lockedWidget )
    {
        GXVec3 data ( position.GetX (), position.GetY (), scroll );
        SendMessage ( lockedWidget, eGXUIMessage::Scroll, &data, sizeof ( GXVec3 ) );
        return;
    }
    
    GXWidget* target = FindWidget ( position );

    if ( !target ) return;

    GXVec3 data ( position.GetX (), position.GetY (), scroll );
    SendMessage ( target, eGXUIMessage::Scroll, &data, sizeof ( GXVec3 ) );
}

GXVoid GXTouchSurface::OnMouseMove ( const GXVec2 &position )
{
    mousePosition = position;

    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::MouseMove, &position, sizeof ( GXVec2 ) );
        return;
    }

    GXWidget* target = FindWidget ( position );

    if ( target != mouseOverWidget )
    {
        if ( mouseOverWidget )
            SendMessage ( mouseOverWidget, eGXUIMessage::MouseLeave, &position, sizeof ( GXVec2 ) );

        mouseOverWidget = target;

        if ( target )
            SendMessage ( target, eGXUIMessage::MouseOver, &position, sizeof ( GXVec2 ) );
    }

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::MouseMove, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnKeyDown ( GXInt keyCode )
{
    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::KeyDown, &keyCode, sizeof ( GXInt ) );
        return;
    }

    GXWidget* target = FindWidget ( mousePosition );

    if ( target != mouseOverWidget )
    {
        if ( mouseOverWidget )
            SendMessage ( mouseOverWidget, eGXUIMessage::MouseLeave, &mousePosition, sizeof ( GXVec2 ) );

        mouseOverWidget = target;

        if ( target )
        {
            SendMessage ( target, eGXUIMessage::MouseOver, &mousePosition, sizeof ( GXVec2 ) );
        }
    }

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::KeyDown, &keyCode, sizeof ( GXInt ) );
}

GXVoid GXTouchSurface::OnKeyUp ( GXInt keyCode )
{
    if ( lockedWidget )
    {
        SendMessage ( lockedWidget, eGXUIMessage::KeyUp, &keyCode, sizeof ( GXInt ) );
        return;
    }

    GXWidget* target = FindWidget ( mousePosition );

    if ( target != mouseOverWidget )
    {
        if ( mouseOverWidget )
            SendMessage ( mouseOverWidget, eGXUIMessage::MouseLeave, &mousePosition, sizeof ( GXVec2 ) );

        mouseOverWidget = target;

        if ( target )
        {
            SendMessage ( target, eGXUIMessage::MouseOver, &mousePosition, sizeof ( GXVec2 ) );
        }
    }

    if ( !target ) return;

    SendMessage ( target, eGXUIMessage::KeyUp, &keyCode, sizeof ( GXInt ) );
}

GXVoid GXTouchSurface::SendMessage ( GXWidget* widget, eGXUIMessage message, const GXVoid* data, GXUInt size )
{
    GXMessage* msg = static_cast<GXMessage*> ( gx_ui_MessageBuffer->Allocate ( sizeof ( GXMessage ) ) );
    msg->next = nullptr;
    msg->widget = widget;
    msg->message = message;
    msg->data = gx_ui_MessageBuffer->Allocate ( size );

    if ( size )
        memcpy ( msg->data, data, size );

    msg->size = size;

    gx_ui_SmartLock->AcquireExlusive ();

    if ( !messages )
    {
        messages = lastMessage = msg;
    }
    else
    {
        lastMessage->next = msg;
        lastMessage = msg;
    }

    gx_ui_SmartLock->ReleaseExlusive ();
}

GXVoid GXTouchSurface::ExecuteMessages ()
{
    while ( messages )
    {
        GXMessage* msg = messages;

        gx_ui_SmartLock->AcquireExlusive ();
        messages = messages->next;
        gx_ui_SmartLock->ReleaseExlusive ();

        msg->widget->OnMessage ( msg->message, msg->data );
    }
}

GXVoid GXTouchSurface::DrawWidgets ()
{
    gx_ui_SmartLock->AcquireShared ();
    DrawWidgets ( widgetTail );
    gx_ui_SmartLock->ReleaseShared ();
}

GXVoid GXTouchSurface::MoveWidgetToForeground ( GXWidget* widget )
{
    if ( !widget || widget == defaultWidget || widget == widgetHead || widget->parent ) return;

    UnRegisterWidget ( widget );
    
    widget->prev = nullptr;
    widget->next = widgetHead;
    widgetHead->prev = widget;
    widgetHead = widget;
}

GXVoid GXTouchSurface::LockCursor ( GXWidget* widget )
{
    lockedWidget = widget;
}

GXWidget* GXTouchSurface::GetLockedCursorWidget ()
{
    return lockedWidget;
}

GXVoid GXTouchSurface::ReleaseCursor ()
{
    lockedWidget = nullptr;
}

GXVoid GXTouchSurface::RegisterWidget ( GXWidget* widget )
{
    if ( !widget || widget->parent ) return;

    widget->prev = widgetTail;

    if ( widgetTail )
        widgetTail->next = widget;
    else
        widgetHead = widget;

    widgetTail = widget;
}

GXVoid GXTouchSurface::UnRegisterWidget ( GXWidget* widget )
{
    if ( !widget || widget->parent ) return;

    if ( widget->next )
        widget->next->prev = widget->prev;
    else
        widgetTail = widget->prev;

    if ( widget->prev )
    {
        widget->prev->next = widget->next;
        return;
    }

    widgetHead = widget->next;
}

const GXVec2& GXTouchSurface::GetMousePosition ()
{
    return mousePosition;
}

GXVoid GXTouchSurface::SetDefaultWidget ( GXWidget* widget )
{
    defaultWidget = widget;
}

GXTouchSurface::GXTouchSurface ()
{
    messages = lastMessage = nullptr;
    widgetHead = widgetTail = mouseOverWidget = lockedWidget = defaultWidget = nullptr;

    gx_ui_SmartLock = new GXSmartLock ();
    gx_ui_MessageBuffer = new GXCircleBuffer ( GX_UI_MESSAGE_BUFFER_SIZE );
}

GXVoid GXTouchSurface::DeleteWidgets ()
{
    while ( widgetHead )
    {
        UnRegisterWidget ( widgetHead );
    }
}

GXWidget* GXTouchSurface::FindWidget ( const GXVec2 &position )
{
    GXVec3 v ( position.GetX (), position.GetY (), 0.0f );

    gx_ui_SmartLock->AcquireShared ();

    GXWidgetIterator iterator;
    GXWidget* p = iterator.Init ( widgetHead );

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

    return defaultWidget;
}

GXVoid GXTouchSurface::DrawWidgets ( GXWidget* widget )
{
    GXWidgetIterator iterator;
    GXWidget* p = iterator.Init ( widget );

    while ( p )
    {
        if ( p->isVisible )
            p->OnDraw ();

        p = iterator.GetPrevious ();
    }
}
