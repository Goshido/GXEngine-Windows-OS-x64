// version 1.4

#ifndef GX_UI_DRAGGABLE_AREA
#define GX_UI_DRAGGABLE_AREA


#include "GXWidget.h"


class GXUIDragableArea;
typedef GXVoid ( GXCALL* PFNGXUIDRAGABLEAREAONRESIZEPROC ) ( GXVoid* handler, GXUIDragableArea& area, GXFloat width, GXFloat height );

enum eGXDraggableAreaResizeMode : GXUByte
{
	None,
	Dragging,
	WidthLockLeft,
	WidthLockRight,
	HeightLockBottom,
	HeightLockTop,
	WidthAndHeightLockTopLeft,
	WidthAndHeightLockTopRight,
	WidthAndHeightLockBottomLeft,
	WidthAndHeightLockBottomRight
};

class GXUIDragableArea final : public GXWidget
{
	private:
		GXFloat							headerHeight;
		GXFloat							borderThickness;

		GXVec2							minimumSize;

		HCURSOR							standartArrow;
		HCURSOR							verticalArrow;
		HCURSOR							horizontalArrow;
		HCURSOR							crossArrow;
		HCURSOR							northwestSoutheastArrow;
		HCURSOR							northeastSouthwestArrow;

		GXVec2							lastMousePosition;
		eGXDraggableAreaResizeMode		resizeMode;
		PFNGXUIDRAGABLEAREAONRESIZEPROC	OnResize;
		GXVoid*							handler;

		GXAABB							headerArea;
		GXAABB							clientArea;

		GXAABB							borderLeft;
		GXAABB							borderTop;
		GXAABB							borderRight;
		GXAABB							borderBottom;

		GXAABB							cornerTopLeft;
		GXAABB							cornerTopRight;
		GXAABB							cornerBottomLeft;
		GXAABB							cornerBottomRight;

		const HCURSOR*					currentCursor;

	public:
		explicit GXUIDragableArea ( GXWidget* parent );
		~GXUIDragableArea () override;

		GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

		GXVoid SetMinimumWidth ( GXFloat width );
		GXFloat GetMinimumWidth () const;

		GXVoid SetMinimumHeight ( GXFloat height );
		GXFloat GetMinimumHeight () const;

		GXVoid SetHeaderHeight ( GXFloat height );
		GXFloat GetHeaderHeight () const;

		GXVoid SetBorderThickness ( GXFloat thickness );
		GXFloat GetBorderThickness () const;

		GXVoid SetOnResizeCallback ( GXVoid* handlerObject, PFNGXUIDRAGABLEAREAONRESIZEPROC callback );

	private:
		eGXDraggableAreaResizeMode GetResizeMode ( const GXVec2 &mousePosition ) const;
		GXVoid UpdateAreas ();
		GXVoid UpdateCursor ( const GXVec2 &mousePosition );

		GXUIDragableArea () = delete;
		GXUIDragableArea ( const GXUIDragableArea &other ) = delete;
		GXUIDragableArea& operator = ( const GXUIDragableArea &other ) = delete;
};


#endif // GX_UI_DRAGGABLE_AREA
