// version 1.11

#ifndef GX_WIDGET
#define GX_WIDGET


#include "GXUIMessage.h"
#include <GXCommon/GXMath.h>
#include <GXCommon/GXMemory.h>


class GXWidgetRenderer;
class GXWidget : public GXMemoryInspector
{
    friend class GXTouchSurface;
    friend class GXWidgetIterator;

    private:
        GXBool                  _isRegistered;

    protected:
        GXWidget*               _next;
        GXWidget*               _previous;

        GXWidget*               _parent;
        GXWidget*               _childs;

        GXBool                  _isVisible;
        GXBool                  _isDraggable;
        GXWidgetRenderer*       _renderer;

        GXAABB                  _boundsWorld;
        GXAABB                  _boundsLocal;

    public:
        explicit GXWidget ( GXWidget* parent, GXBool isNeedRegister = GX_TRUE );
        virtual ~GXWidget ();

        virtual GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data );

        GXVoid Resize ( GXFloat x, GXFloat y, GXFloat width, GXFloat height );
        const GXAABB& GetBoundsWorld () const;
        const GXAABB& GetBoundsLocal () const;

        GXVoid Show ();
        GXVoid Refresh ();
        GXVoid Hide ();
        GXVoid ToForeground ();
        GXBool IsVisible () const;
        GXBool IsDraggable () const;

        GXVoid SetRenderer ( GXWidgetRenderer* renderer );
        GXWidgetRenderer* GetRenderer () const;
        GXWidget* FindWidget ( const GXVec2 &position );    // return child widget or itself. Never nullptr

    protected:
        GXVoid UpdateBoundsWorld ( const GXAABB &boundsLocal );
        GXVoid OnDraw ();

    private:
        GXVoid AddChild ( GXWidget* child );
        GXVoid RemoveChild ( GXWidget* child );
        GXBool DoesChildExist ( GXWidget* child ) const;

        GXWidget () = delete;
        GXWidget ( const GXWidget &other ) = delete;
        GXWidget& operator = ( const GXWidget &other ) = delete;
};

class GXWidgetIterator final
{
    private:
        GXWidget*       widget;

    public:
        GXWidgetIterator ();
        ~GXWidgetIterator ();

        GXWidget* Init ( GXWidget* startWidget );

        GXWidget* GetNext ();
        GXWidget* GetPrevious ();
        GXWidget* GetParent ();
        GXWidget* GetChilds ();

    private:
        GXWidgetIterator ( const GXWidgetIterator &other ) = delete;
        GXWidgetIterator& operator = ( const GXWidgetIterator &other ) = delete;
};

class GXWidgetRenderer : public GXMemoryInspector
{
    friend class GXWidget;

    protected:
        GXWidget*       widget;

    private:
        GXAABB          oldBounds;

    public:
        explicit GXWidgetRenderer ( GXWidget* widgetObject );
        ~GXWidgetRenderer () override;

        GXVoid OnUpdate ();

    protected:
        virtual GXVoid OnRefresh ();
        virtual GXVoid OnDraw ();
        virtual GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height );
        virtual GXVoid OnMoved ( GXFloat x, GXFloat y );

    private:
        GXBool IsResized ();
        GXBool IsMoved ();

        GXWidgetRenderer () = delete;
        GXWidgetRenderer ( const GXWidgetRenderer &other ) = delete;
        GXWidgetRenderer& operator = ( const GXWidgetRenderer &other ) = delete;
};

#endif // GX_WIDGET
