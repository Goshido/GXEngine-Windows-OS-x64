//version 1.2

#ifndef GX_TOUCH_SURFACE
#define GX_TOUCH_SURFACE


#include "GXWidget.h"


struct GXMessage;
class GXTouchSurface
{
	private:
		GXMessage*					messages;
		GXMessage*					lastMessage;

		GXWidget*					widgetHead;
		GXWidget*					widgetTail;

		GXWidget*					mouseOverWidget;
		GXWidget*					lockedWidget;
		GXWidget*					defaultWidget;

		GXVec2						mousePosition;

		static GXTouchSurface*		instance;

	public:
		~GXTouchSurface ();

		GXVoid OnLeftMouseButtonDown ( const GXVec2 &position );
		GXVoid OnLeftMouseButtonUp ( const GXVec2 &position );
		GXVoid OnMiddleMouseButtonDown ( const GXVec2 &position );
		GXVoid OnMiddleMouseButtonUp ( const GXVec2 &position );
		GXVoid OnRightMouseButtonDown ( const GXVec2 &position );
		GXVoid OnRightMouseButtonUp ( const GXVec2 &position );
		GXVoid OnDoubleClick ( const GXVec2 &position );

		GXVoid OnScroll ( const GXVec2 &position, GXFloat scroll );
		GXVoid OnMouseMove ( const GXVec2 &position );

		GXVoid SendMessage ( GXWidget* widget, GXUInt message, const GXVoid* data, GXUInt size );
		GXVoid ExecuteMessages ();

		GXVoid DrawWidgets ();
		GXVoid MoveWidgetToForeground ( GXWidget* widget );

		GXVoid LockCursor ( GXWidget* destination );			//For draggable and resizable widgets
		GXWidget* GetLockedCursorWidget ();
		GXVoid ReleaseCursor ();

		GXVoid RegisterWidget ( GXWidget* widget );
		GXVoid UnRegisterWidget ( GXWidget* widget );

		const GXVec2& GetMousePosition ();

		GXVoid SetDefaultWidget ( GXWidget* widget );

		static GXTouchSurface* GXCALL GetInstance ();

	private:
		explicit GXTouchSurface ();

		GXVoid DeleteWidgets ();
		GXWidget* FindWidget ( GXFloat x, GXFloat y );
		GXVoid DrawWidgets ( GXWidget* widget );
};


#endif //GX_TOUCH_SURFACE
