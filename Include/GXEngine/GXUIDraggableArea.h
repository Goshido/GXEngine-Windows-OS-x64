//version 1.0

#ifndef GX_UI_DRAGGABLE_AREA
#define GX_UI_DRAGGABLE_AREA


#include "GXWidget.h"


class GXUIDragableArea;
typedef GXVoid ( GXCALL* PFNGXUIDRAGABLEAREAONRESIZEPROC ) ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height );

class GXUIDragableArea : public GXWidget
{
	private:
		GXFloat							headerHeight;
		GXFloat							borderThickness;

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

		HCURSOR							standartArrow;
		HCURSOR							verticalArrow;
		HCURSOR							horizontalArrow;
		HCURSOR							crossArrow;
		HCURSOR							northwestSoutheastArrow;
		HCURSOR							northeastSouthwestArrow;
		const HCURSOR*					currentCursor;

		GXVec2							lastMousePosition;
		GXUByte							resizeMode;
		PFNGXUIDRAGABLEAREAONRESIZEPROC	OnResize;
		GXVoid*							handler;

	public:
		GXUIDragableArea ( GXWidget* parent );
		~GXUIDragableArea () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXVoid SetHeaderHeight ( GXFloat height );
		GXFloat GetHeaderHeight () const;

		GXVoid SetBorderThickness ( GXFloat thickness );
		GXFloat GetBorderThickness () const;

		GXVoid SetOnResizeCallback ( GXVoid* handler, PFNGXUIDRAGABLEAREAONRESIZEPROC callback );

	private:
		GXUByte GetResizeMode ( const GXVec2 &mousePosition ) const;
		GXVoid UpdateAreas ();
		GXVoid UpdateCursor ( const GXVec2 &mousePosition );
};


#endif //GX_UI_DRAGGABLE_AREA
