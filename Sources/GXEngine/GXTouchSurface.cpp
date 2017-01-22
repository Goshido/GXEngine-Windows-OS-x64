//version 1.2

#include <GXEngine/GXTouchSurface.h>
#include <GXEngine/GXUIMessage.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXMutex.h>
#include <GXCommon/GXLogger.h>


GXFloat				gx_ui_Scale = 1.0f;

GXMutex*			gx_ui_Mutex = nullptr;
GXCircleBuffer*		gx_ui_MessageBuffer = nullptr;

#define GX_UI_MESSAGE_BUFFER_SIZE		1048576		//1 Mb


struct GXMessage
{
	GXMessage*		next;
	GXWidget*		widget;
	GXUInt			message;
	GXVoid*			data;
	GXUInt			size;
};

GXTouchSurface* GXTouchSurface::instance = nullptr;

GXTouchSurface::~GXTouchSurface ()
{
	delete gx_ui_MessageBuffer;
	DeleteWidgets ();
	delete gx_ui_Mutex;
	instance = nullptr;
}

GXVoid GXTouchSurface::OnLeftMouseButtonDown ( const GXVec2 &position )
{
	mousePosition = position;

	if ( lockedWidget )
	{
		SendMessage ( lockedWidget, GX_MSG_LMBDOWN, &position, sizeof ( GXVec2 ) );
		SendMessage ( lockedWidget, GX_MSG_FOREGROUND, nullptr, 0 );
	}
	else
	{
		GXWidget* target = FindWidget ( position.x, position.y );
		if ( target )
		{
			SendMessage ( target, GX_MSG_LMBDOWN, &position, sizeof ( GXVec2 ) );
			SendMessage ( target, GX_MSG_FOREGROUND, nullptr, 0 );
		}
	}
}

GXVoid GXTouchSurface::OnLeftMouseButtonUp ( const GXVec2 &position )
{
	mousePosition = position;

	if ( lockedWidget )
	{
		SendMessage ( lockedWidget, GX_MSG_LMBUP, &position, sizeof ( GXVec2 ) );
	}
	else
	{
		GXWidget* target = FindWidget ( position.x, position.y );
		if ( target )
			SendMessage ( target, GX_MSG_LMBUP, &position, sizeof ( GXVec2 ) );
	}
}

GXVoid GXTouchSurface::OnMiddleMouseButtonDown ( const GXVec2 &position )
{
	mousePosition = position;

	if ( lockedWidget )
	{
		SendMessage ( lockedWidget, GX_MSG_MMBDOWN, &position, sizeof ( GXVec2 ) );
	}
	else
	{
		GXWidget* target = FindWidget ( position.x, position.y );
		if ( target )
			SendMessage ( target, GX_MSG_MMBDOWN, &position, sizeof ( GXVec2 ) );
	}
}

GXVoid GXTouchSurface::OnMiddleMouseButtonUp ( const GXVec2 &position )
{
	mousePosition = position;

	if ( lockedWidget )
	{
		SendMessage ( lockedWidget, GX_MSG_MMBUP, &position, sizeof ( GXVec2 ) );
	}
	else
	{
		GXWidget* target = FindWidget ( position.x, position.y );
		if ( target )
			SendMessage ( target, GX_MSG_MMBUP, &position, sizeof ( GXVec2 ) );
	}
}

GXVoid GXTouchSurface::OnRightMouseButtonDown ( const GXVec2 &position )
{
	mousePosition = position;

	if ( lockedWidget )
	{
		SendMessage ( lockedWidget, GX_MSG_RMBDOWN, &position, sizeof ( GXVec2 ) );
	}
	else
	{
		GXWidget* target = FindWidget ( position.x, position.y );
		if ( target )
			SendMessage ( target, GX_MSG_RMBDOWN, &position, sizeof ( GXVec2 ) );
	}
}

GXVoid GXTouchSurface::OnRightMouseButtonUp ( const GXVec2 &position )
{
	mousePosition = position;

	if ( lockedWidget )
	{
		SendMessage ( lockedWidget, GX_MSG_RMBUP, &position, sizeof ( GXVec2 ) );
	}
	else
	{
		GXWidget* target = FindWidget ( position.x, position.y );
		if ( target )
			SendMessage ( target, GX_MSG_RMBUP, &position, sizeof ( GXVec2 ) );
	}
}

GXVoid GXTouchSurface::OnDoubleClick ( const GXVec2 &position )
{
	mousePosition = position;

	if ( lockedWidget )
	{
		SendMessage ( lockedWidget, GX_MSG_DOUBLE_CLICK, &position, sizeof ( GXVec2 ) );
		SendMessage ( lockedWidget, GX_MSG_FOREGROUND, nullptr, 0 );
	}
	else
	{
		GXWidget* target = FindWidget ( position.x, position.y );
		if ( target )
		{
			SendMessage ( target, GX_MSG_DOUBLE_CLICK, &position, sizeof ( GXVec2 ) );
			SendMessage ( target, GX_MSG_FOREGROUND, nullptr, 0 );
		}
	}
}

GXVoid GXTouchSurface::OnScroll ( const GXVec2 &position, GXFloat scroll )
{
	mousePosition = position;

	if ( lockedWidget )
	{
		GXVec3 data = GXCreateVec3 ( position.x, position.y, scroll );
		SendMessage ( lockedWidget, GX_MSG_SCROLL, &data, sizeof ( GXVec3 ) );
	}
	else
	{
		GXWidget* target = FindWidget ( position.x, position.y );
		if ( target )
		{
			GXVec3 data = GXCreateVec3 ( position.x, position.y, scroll );
			SendMessage ( target, GX_MSG_SCROLL, &data, sizeof ( GXVec3 ) );
		}
	}
}

GXVoid GXTouchSurface::OnMouseMove ( const GXVec2 &position )
{
	mousePosition = position;

	if ( lockedWidget )
	{
		SendMessage ( lockedWidget, GX_MSG_MOUSE_MOVE, &position, sizeof ( GXVec2 ) );
		return;
	}

	GXWidget* target = FindWidget ( position.x, position.y );

	if ( target != mouseOverWidget )
	{
		if ( mouseOverWidget )
			SendMessage ( mouseOverWidget, GX_MSG_MOUSE_LEAVE, &position, sizeof ( GXVec2 ) );

		mouseOverWidget = target;

		if ( target )
			SendMessage ( target, GX_MSG_MOUSE_OVER, &position, sizeof ( GXVec2 ) );
	}

	if ( !target ) return;

	SendMessage ( target, GX_MSG_MOUSE_MOVE, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::SendMessage ( GXWidget* widget, GXUInt message, const GXVoid* data, GXUInt size )
{
	GXMessage* msg = (GXMessage*)gx_ui_MessageBuffer->Allocate ( sizeof ( GXMessage ) );
	msg->next = nullptr;
	msg->widget = widget;
	msg->message = message;
	msg->data = gx_ui_MessageBuffer->Allocate ( size );
	if ( size )
		memcpy ( msg->data, data, size );
	msg->size = size;

	gx_ui_Mutex->Lock ();

	if ( !messages )
		messages = lastMessage = msg;
	else
	{
		lastMessage->next = msg;
		lastMessage = msg;
	}

	gx_ui_Mutex->Release ();

}

GXVoid GXTouchSurface::ExecuteMessages ()
{
	while ( messages )
	{
		GXMessage* msg = messages;

		gx_ui_Mutex->Lock ();
		messages = messages->next;
		gx_ui_Mutex->Release ();

		msg->widget->OnMessage ( msg->message, msg->data );
	}
}

GXVoid GXTouchSurface::DrawWidgets ()
{
	gx_ui_Mutex->Lock ();
	DrawWidgets ( widgetTail );
	gx_ui_Mutex->Release ();
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

GXVoid GXTouchSurface::LockCursor ( GXWidget* destination )
{
	lockedWidget = destination;
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
		widget->prev->next = widget->next;
	else
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

GXTouchSurface* GXCALL GXTouchSurface::GetInstance ()
{
	if ( !instance )
		instance = new GXTouchSurface ();

	return instance;
}

GXTouchSurface::GXTouchSurface ()
{
	messages = lastMessage = nullptr;
	widgetHead = widgetTail = mouseOverWidget = lockedWidget = defaultWidget = nullptr;

	gx_ui_Mutex = new GXMutex ();
	gx_ui_MessageBuffer = new GXCircleBuffer ( GX_UI_MESSAGE_BUFFER_SIZE );
}

GXVoid GXTouchSurface::DeleteWidgets ()
{
	while ( widgetHead )
		UnRegisterWidget ( widgetHead );
}

GXWidget* GXTouchSurface::FindWidget ( GXFloat x, GXFloat y )
{
	GXVec3 v = GXCreateVec3 ( x, y, 0.0f );

	gx_ui_Mutex->Lock ();

	GXWidgetIterator iterator;
	GXWidget* p = iterator.Init ( widgetHead );
	while ( p )
	{
		if ( p->IsVisible () && GXIsOverlapedAABBVec3 ( p->GetBoundsWorld (), v ) )
		{
			gx_ui_Mutex->Release ();
			return p->FindWidget ( x, y );
		}

		p = iterator.GetNext ();
	}

	gx_ui_Mutex->Release ();

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