// version 1.6

#ifndef GX_WIDGET
#define GX_WIDGET


#include <GXCommon/GXMath.h>
#include <GXEngine/GXUIMessage.h>


class GXWidgetRenderer;
class GXWidget
{
	friend class GXTouchSurface;
	friend class GXWidgetIterator;

	private:
		GXBool					isRegistered;

	protected:
		GXWidget*				next;
		GXWidget*				prev;

		GXWidget*				parent;
		GXWidget*				childs;

		GXBool					isVisible;
		GXBool					isDraggable;
		GXWidgetRenderer*		renderer;

		GXAABB					boundsWorld;
		GXAABB					boundsLocal;

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

		GXVoid SetRenderer ( GXWidgetRenderer* rendererObject );
		GXWidgetRenderer* GetRenderer () const;
		GXWidget* FindWidget ( const GXVec2 &position );	// return child widget or itself. Never nullptr

	protected:
		GXVoid UpdateBoundsWorld ( const GXAABB &newBoundsLocal );
		GXVoid OnDraw ();

	private:
		GXVoid AddChild ( GXWidget* child );
		GXVoid RemoveChild ( GXWidget* child );
		GXBool DoesChildExist ( GXWidget* child ) const;

		GXWidget () = delete;
		GXWidget ( const GXWidget &other ) = delete;
		GXWidget& operator = ( const GXWidget &other ) = delete;
};

class GXWidgetIterator
{
	private:
		GXWidget*	widget;

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

class GXWidgetRenderer
{
	friend class GXWidget;

	protected:
		GXWidget*	widget;

	private:
		GXAABB		oldBounds;

	public:
		explicit GXWidgetRenderer ( GXWidget* widget );
		virtual ~GXWidgetRenderer ();

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
