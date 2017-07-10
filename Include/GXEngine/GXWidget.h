//version 1.2

#ifndef GX_WIDGET
#define GX_WIDGET


#include <GXCommon/GXMath.h>


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

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid Resize ( GXFloat x, GXFloat y, GXFloat width, GXFloat height );
		const GXAABB& GetBoundsWorld () const;
		const GXAABB& GetBoundsLocal () const;

		GXVoid Show ();
		GXVoid Hide ();
		GXVoid ToForeground ();
		GXBool IsVisible () const;
		GXBool IsDraggable () const;

		GXVoid SetRenderer ( GXWidgetRenderer* renderer );
		GXWidgetRenderer* GetRenderer () const;
		GXWidget* FindWidget ( GXFloat x, GXFloat y );	//return child widget or itself. Never 0

	protected:
		GXVoid UpdateBoundsWorld ( const GXAABB &boundsLocal );
		GXVoid OnDraw ();

	private:
		GXVoid AddChild ( GXWidget* child );
		GXVoid RemoveChild ( GXWidget* child );
		GXBool DoesChildExist ( GXWidget* child ) const;
};

class GXWidgetIterator
{
	private:
		GXWidget*	widget;

	public:
		GXWidgetIterator ();
		~GXWidgetIterator ();

		GXWidget* Init ( GXWidget* widget );

		GXWidget* GetNext ();
		GXWidget* GetPrevious ();
		GXWidget* GetParent ();
		GXWidget* GetChilds ();
};

class GXWidgetRenderer
{
	friend class GXWidget;

	private:
		GXAABB		oldBounds;

	protected:
		GXWidget*	widget;

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
};

#endif //GX_WIDGET
