// version 1.10

#ifndef GX_TOUCH_SURFACE
#define GX_TOUCH_SURFACE


#include "GXWidget.h"


struct GXMessage;
class GXTouchSurface final
{
    private:
        GXMessage*                  _messages;
        GXMessage*                  _lastMessage;

        GXWidget*                   _widgetHead;
        GXWidget*                   _widgetTail;

        GXWidget*                   _mouseOverWidget;
        GXWidget*                   _lockedWidget;
        GXWidget*                   _defaultWidget;

        GXVec2                      _mousePosition;

        static GXTouchSurface*      _instance;

    public:
        static GXTouchSurface& GXCALL GetInstance ();
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

        GXVoid OnKeyDown ( GXInt keyCode );
        GXVoid OnKeyUp ( GXInt keyCode );

        GXVoid SendMessage ( GXWidget* widget, eGXUIMessage message, const GXVoid* data, GXUInt size );
        GXVoid ExecuteMessages ();

        GXVoid DrawWidgets ();
        GXVoid MoveWidgetToForeground ( GXWidget* widget );

        GXVoid LockCursor ( GXWidget* widget );            // For draggable and resizable widgets
        GXWidget* GetLockedCursorWidget ();
        GXVoid ReleaseCursor ();

        GXVoid RegisterWidget ( GXWidget* widget );
        GXVoid UnRegisterWidget ( GXWidget* widget );

        const GXVec2& GetMousePosition ();

        GXVoid SetDefaultWidget ( GXWidget* widget );

    private:
        GXTouchSurface ();

        GXVoid DeleteWidgets ();
        GXWidget* FindWidget ( const GXVec2 &position );
        GXVoid DrawWidgets ( GXWidget* widget );

        GXTouchSurface ( const GXTouchSurface &other ) = delete;
        GXTouchSurface& operator = ( const GXTouchSurface &other ) = delete;
};


#endif // GX_TOUCH_SURFACE
