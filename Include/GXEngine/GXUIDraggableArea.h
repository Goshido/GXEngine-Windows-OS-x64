// version 1.5

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
        GXFloat                             _headerHeight;
        GXFloat                             _borderThickness;

        GXVec2                              _minimumSize;

        HCURSOR                             _standartArrow;
        HCURSOR                             _verticalArrow;
        HCURSOR                             _horizontalArrow;
        HCURSOR                             _crossArrow;
        HCURSOR                             _northwestSoutheastArrow;
        HCURSOR                             _northeastSouthwestArrow;

        GXVec2                              _lastMousePosition;
        eGXDraggableAreaResizeMode          _resizeMode;
        PFNGXUIDRAGABLEAREAONRESIZEPROC     _onResize;
        GXVoid*                             _context;

        GXAABB                              _headerArea;
        GXAABB                              _clientArea;

        GXAABB                              _borderLeft;
        GXAABB                              _borderTop;
        GXAABB                              _borderRight;
        GXAABB                              _borderBottom;

        GXAABB                              _cornerTopLeft;
        GXAABB                              _cornerTopRight;
        GXAABB                              _cornerBottomLeft;
        GXAABB                              _cornerBottomRight;

        const HCURSOR*                      _currentCursor;

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

        GXVoid SetOnResizeCallback ( GXVoid* context, PFNGXUIDRAGABLEAREAONRESIZEPROC callback );

    private:
        eGXDraggableAreaResizeMode GetResizeMode ( const GXVec2 &mousePosition ) const;
        GXVoid UpdateAreas ();
        GXVoid UpdateCursor ( const GXVec2 &mousePosition );

        GXUIDragableArea () = delete;
        GXUIDragableArea ( const GXUIDragableArea &other ) = delete;
        GXUIDragableArea& operator = ( const GXUIDragableArea &other ) = delete;
};


#endif // GX_UI_DRAGGABLE_AREA
