// version 1.5

#include <GXEngine/GXUIDraggableArea.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>


#define DEFAULT_MINIMUM_WIDTH		0.7f
#define DEFAULT_MINIMUM_HEIGHT		1.5f

#define DEFAULT_BORDER_THICKNESS	0.2f // 2 mm
#define DEFAULT_HEADER_HEIGHT		0.9f // 9 mm

#define TOLERANCE_FACTOR			0.25f

//---------------------------------------------------------------------------------------------------------------------

GXUIDragableArea::GXUIDragableArea ( GXWidget* parent ):
    GXWidget ( parent ),
    _headerHeight ( DEFAULT_HEADER_HEIGHT * gx_ui_Scale ),
    _borderThickness ( DEFAULT_BORDER_THICKNESS * gx_ui_Scale ),
    _minimumSize ( DEFAULT_MINIMUM_WIDTH * gx_ui_Scale, DEFAULT_MINIMUM_HEIGHT * gx_ui_Scale ),
    _standartArrow ( LoadCursorW ( 0, IDC_ARROW ) ),
    _verticalArrow ( LoadCursorW ( 0, IDC_SIZENS ) ),
    _horizontalArrow ( LoadCursorW ( 0, IDC_SIZEWE ) ),
    _crossArrow ( LoadCursorW ( 0, IDC_SIZEALL ) ),
    _northwestSoutheastArrow ( LoadCursorW ( 0, IDC_SIZENWSE ) ),
    _northeastSouthwestArrow ( LoadCursorW ( 0, IDC_SIZENESW ) ),
    _lastMousePosition ( 0.0f, 0.0f ),
    _resizeMode ( eGXDraggableAreaResizeMode::None ),
    _onResize ( nullptr ),
    _context ( nullptr )
{
    _isDraggable = GX_TRUE;
    _currentCursor = &_standartArrow;
}

GXUIDragableArea::~GXUIDragableArea ()
{
    // NOTHING
}

GXVoid GXUIDragableArea::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    if ( message == eGXUIMessage::LMBDown )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );

        _resizeMode = GetResizeMode ( *pos );
        memcpy ( &_lastMousePosition, pos, sizeof ( GXVec2 ) );

        if ( _resizeMode == eGXDraggableAreaResizeMode::None ) return;

        GXTouchSurface::GetInstance ().LockCursor ( this );
        return;
    }

    if ( message == eGXUIMessage::LMBUp )
    {
        _resizeMode = eGXDraggableAreaResizeMode::None;

        GXTouchSurface& touchSurface = GXTouchSurface::GetInstance ();

        if ( touchSurface.GetLockedCursorWidget () != this ) return;

        touchSurface.ReleaseCursor ();
        return;
    }

    if ( message == eGXUIMessage::MouseLeave )
    {
        SetCursor ( _standartArrow );
        _currentCursor = &_standartArrow;
        return;
    }

    if ( message == eGXUIMessage::MouseOver )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        UpdateCursor ( *pos );
        return;
    }

    if ( message == eGXUIMessage::MouseMove )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );

        switch ( _resizeMode )
        {
            case eGXDraggableAreaResizeMode::Dragging:
            {
                GXVec2 delta;
                delta.Substract ( *pos, _lastMousePosition );

                OnMessage ( eGXUIMessage::Drag, &delta );

                memcpy ( &_lastMousePosition, pos, sizeof ( GXVec2 ) );
            }
            break;

            case eGXDraggableAreaResizeMode::WidthLockLeft:
            {
                GXAABB bounds ( _boundsLocal );
                bounds.max.data[ 0 ] += pos->data[ 0 ] - _lastMousePosition.data[ 0 ];

                if ( bounds.GetWidth () < _minimumSize.GetX () )
                {
                    if ( fabs ( _boundsLocal.GetWidth () - _minimumSize.GetX () ) < TOLERANCE_FACTOR ) break;

                    bounds.max.SetX ( bounds.min.GetX () + _minimumSize.GetX () );
                    _lastMousePosition.SetX ( _boundsWorld.min.GetX () + _minimumSize.GetX () );
                }
                else
                {
                    _lastMousePosition.SetX ( pos->GetX () );
                }

                UpdateBoundsWorld ( bounds );
                UpdateAreas ();

                if ( _renderer )
                    _renderer->OnUpdate ();

                if ( _onResize )
                    _onResize ( _context, *this, _boundsLocal.GetWidth (), _boundsLocal.GetHeight () );
            }
            break;

            case eGXDraggableAreaResizeMode::WidthLockRight:
            {
                GXAABB bounds ( _boundsLocal );
                bounds.min.data[ 0 ] += pos->data[ 0 ] - _lastMousePosition.data[ 0 ];

                if ( bounds.GetWidth () < _minimumSize.GetX () )
                {
                    if ( fabs ( _boundsLocal.GetWidth () - _minimumSize.GetX () ) < TOLERANCE_FACTOR ) break;

                    bounds.min.SetX ( bounds.max.GetX () - _minimumSize.GetX () );
                    _lastMousePosition.SetX ( _boundsWorld.max.GetX () - _minimumSize.GetX () );
                }
                else
                {
                    _lastMousePosition.SetX ( pos->GetX () );
                }

                UpdateBoundsWorld ( bounds );
                UpdateAreas ();

                GXVec2 dragDelta ( 0.0f, 0.0f );
                OnMessage ( eGXUIMessage::Drag, &dragDelta );

                if ( _onResize )
                    _onResize ( _context, *this, _boundsLocal.GetWidth (), _boundsLocal.GetHeight () );
            }
            break;

            case eGXDraggableAreaResizeMode::HeightLockBottom:
            {
                GXAABB bounds ( _boundsLocal );
                bounds.max.data[ 1 ] += pos->data[ 1 ] - _lastMousePosition.data[ 1 ];

                if ( bounds.GetHeight () < _minimumSize.GetY () )
                {
                    if ( fabs ( _boundsLocal .GetHeight ()- _minimumSize.GetY () ) < TOLERANCE_FACTOR ) break;

                    bounds.max.SetY ( bounds.min.GetY () + _minimumSize.GetY () );
                    _lastMousePosition.SetY ( _boundsWorld.min.GetY () + _minimumSize.GetY () );
                }
                else
                {
                    _lastMousePosition.SetY ( pos->GetY () );
                }

                UpdateBoundsWorld ( bounds );
                UpdateAreas ();

                if ( _renderer )
                    _renderer->OnUpdate ();

                if ( _onResize )
                    _onResize ( _context, *this, _boundsLocal.GetWidth (), _boundsLocal.GetHeight () );
            }
            break;

            case eGXDraggableAreaResizeMode::HeightLockTop:
            {
                GXAABB bounds ( _boundsLocal );
                bounds.min.data[ 1 ] += pos->data[ 1 ] - _lastMousePosition.data[ 1 ];

                if ( bounds.GetHeight () < _minimumSize.GetY () )
                {
                    if ( fabs ( _boundsLocal.GetHeight () - _minimumSize.GetY () ) < TOLERANCE_FACTOR ) break;

                    bounds.min.SetY ( bounds.max.GetY () - _minimumSize.GetY () );
                    _lastMousePosition.SetY ( _boundsWorld.max.GetY () - _minimumSize.GetY () );
                }
                else
                {
                    _lastMousePosition.SetY ( pos->GetY () );
                }

                UpdateBoundsWorld ( bounds );
                UpdateAreas ();

                GXVec2 dragDelta ( 0.0f, 0.0f );
                OnMessage ( eGXUIMessage::Drag, &dragDelta );

                if ( _onResize )
                    _onResize ( _context, *this, _boundsLocal.GetWidth (), _boundsLocal.GetHeight () );
            }
            break;

            case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomLeft:
            {
                GXVec2 delta;
                delta.Substract ( *pos, _lastMousePosition );

                GXAABB bounds ( _boundsLocal );
                bounds.max.data[ 0 ] += delta.data[ 0 ];
                bounds.max.data[ 1 ] += delta.data[ 1 ];

                GXFloat width = bounds.GetWidth ();
                GXFloat heigth = bounds.GetHeight ();

                if ( width < _minimumSize.GetX () && heigth < _minimumSize.GetY () ) break;

                if ( width < _minimumSize.GetX () )
                {
                    bounds.max.SetX ( _boundsLocal.min.GetX () + _minimumSize.GetX () );
                    _lastMousePosition.SetX ( _boundsWorld.min.GetX () + _minimumSize.GetX () );
                }
                else
                {
                    _lastMousePosition.SetX ( pos->GetX () );
                }

                if ( heigth < _minimumSize.GetY () )
                {
                    bounds.max.SetY ( _boundsLocal.min.GetY () + _minimumSize.GetY () );
                    _lastMousePosition.SetY ( _boundsWorld.min.GetY () + _minimumSize.GetY () );
                }
                else
                {
                    _lastMousePosition.SetY ( pos->GetY () );
                }

                UpdateBoundsWorld ( bounds );
                UpdateAreas ();

                if ( _renderer )
                    _renderer->OnUpdate ();

                if ( _onResize )
                    _onResize ( _context, *this, _boundsLocal.GetWidth (), _boundsLocal.GetHeight () );
            }
            break;

            case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomRight:
            {
                GXVec2 delta;
                delta.Substract ( *pos, _lastMousePosition );

                GXAABB bounds ( _boundsLocal );
                bounds.min.data[ 0 ] += delta.data[ 0 ];
                bounds.max.data[ 1 ] += delta.data[ 1 ];

                GXFloat width = bounds.GetWidth ();
                GXFloat heigth = bounds.GetHeight ();

                if ( width < _minimumSize.GetX () && heigth < _minimumSize.GetY () ) break;

                if ( width < _minimumSize.GetX () )
                {
                    bounds.min.SetX ( _boundsLocal.max.GetX () - _minimumSize.GetX () );
                    _lastMousePosition.SetX ( _boundsWorld.max.GetX () - _minimumSize.GetX () );
                }
                else
                {
                    _lastMousePosition.SetX ( pos->GetX () );
                }

                if ( heigth < _minimumSize.GetY () )
                {
                    bounds.max.SetY ( _boundsLocal.min.GetY () + _minimumSize.GetY () );
                    _lastMousePosition.SetY ( _boundsWorld.min.GetY () + _minimumSize.GetY () );
                }
                else
                {
                    _lastMousePosition.SetY ( pos->GetY () );
                }

                UpdateBoundsWorld ( bounds );
                UpdateAreas ();

                GXVec2 dragDelta ( 0.0f, 0.0f );
                OnMessage ( eGXUIMessage::Drag, &dragDelta );

                if ( _onResize )
                    _onResize ( _context, *this, _boundsLocal.GetWidth (), _boundsLocal.GetHeight () );
            }
            break;

            case eGXDraggableAreaResizeMode::WidthAndHeightLockTopLeft:
            {
                GXVec2 delta;
                delta.Substract ( *pos, _lastMousePosition );

                GXAABB bounds ( _boundsLocal );
                bounds.max.data[ 0 ] += delta.data[ 0 ];
                bounds.min.data[ 1 ] += delta.data[ 1 ];

                GXFloat width = bounds.GetWidth ();
                GXFloat heigth = bounds.GetHeight ();

                if ( width < _minimumSize.GetX () && heigth < _minimumSize.GetY () ) break;

                if ( width < _minimumSize.GetX () )
                {
                    bounds.max.SetX ( _boundsLocal.min.GetX () + _minimumSize.GetX () );
                    _lastMousePosition.SetX ( _boundsWorld.min.GetX () + _minimumSize.GetX () );
                }
                else
                {
                    _lastMousePosition.SetX ( pos->GetX () );
                }

                if ( heigth < _minimumSize.GetY () )
                {
                    bounds.min.SetY ( _boundsLocal.max.GetY () - _minimumSize.GetY () );
                    _lastMousePosition.SetY ( _boundsWorld.max.GetY () - _minimumSize.GetY () );
                }
                else
                {
                    _lastMousePosition.SetY ( pos->GetY () );
                }

                UpdateBoundsWorld ( bounds );
                UpdateAreas ();

                GXVec2 dragDelta ( 0.0f, 0.0f );
                OnMessage ( eGXUIMessage::Drag, &dragDelta );

                if ( _onResize )
                    _onResize ( _context, *this, _boundsLocal.GetWidth (), _boundsLocal.GetHeight () );
            }
            break;

            case eGXDraggableAreaResizeMode::WidthAndHeightLockTopRight:
            {
                GXVec2 delta;
                delta.Substract ( *pos, _lastMousePosition );

                GXAABB bounds ( _boundsLocal );
                bounds.min.data[ 0 ] += delta.data[ 0 ];
                bounds.min.data[ 1 ] += delta.data[ 1 ];

                GXFloat width = bounds.GetWidth ();
                GXFloat heigth = bounds.GetHeight ();

                if ( width < _minimumSize.GetX () && heigth < _minimumSize.GetY () ) break;

                if ( width < _minimumSize.GetX () )
                {
                    bounds.min.SetX ( _boundsLocal.max.GetX () - _minimumSize.GetX () );
                    _lastMousePosition.SetX ( _boundsWorld.max.GetX () - _minimumSize.GetX () );
                }
                else
                {
                    _lastMousePosition.SetX ( pos->GetX () );
                }

                if ( heigth < _minimumSize.GetY () )
                {
                    bounds.min.SetY ( _boundsLocal.max.GetY () - _minimumSize.GetY () );
                    _lastMousePosition.SetY ( _boundsWorld.max.GetY () - _minimumSize.GetY () );
                }
                else
                {
                    _lastMousePosition.SetY ( pos->GetY () );
                }

                UpdateBoundsWorld ( bounds );
                UpdateAreas ();

                GXVec2 dragDelta ( 0.0f, 0.0f );
                OnMessage ( eGXUIMessage::Drag, &dragDelta );

                if ( _onResize )
                    _onResize ( _context, *this, _boundsLocal.GetWidth (), _boundsLocal.GetHeight () );
            }
            break;

            case eGXDraggableAreaResizeMode::None:
                UpdateCursor ( *pos );
            break;

            default:
                // NOTHING
            break;
        }

        return;
    }

    if ( message == eGXUIMessage::Resize )
    {
        const GXAABB* bounds = static_cast<const GXAABB*> ( data );

        GXAABB correctedBounds;
        correctedBounds.AddVertex ( bounds->min );

        GXVec3 maxPoint ( bounds->max );
        GXFloat length = bounds->GetWidth ();

        if ( length < _minimumSize.GetX () )
            maxPoint.SetX ( bounds->min.GetX () + length );

        length = bounds->GetHeight ();

        if ( length < _minimumSize.GetY () )
            maxPoint.SetY ( bounds->min.GetY () + length );

        correctedBounds.AddVertex ( maxPoint );

        UpdateBoundsWorld ( correctedBounds );
        UpdateAreas ();

        GXVec2 dragDelta ( 0.0f, 0.0f );
        OnMessage ( eGXUIMessage::Drag, &dragDelta );

        if ( _onResize )
            _onResize ( _context, *this, _boundsLocal.GetWidth (), _boundsLocal.GetHeight () );

        return;
    }
        
    if ( message == eGXUIMessage::DraggableAreaSetHeaderHeght )
    {
        const GXFloat* height = static_cast<const GXFloat*> ( data );
        _headerHeight = *height;

        UpdateAreas ();

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::DraggableAreaSetBorderThickness )
    {
        const GXFloat* thickness = static_cast<const GXFloat*> ( data );
        _borderThickness = *thickness;

        UpdateAreas ();

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::DraggableAreaSetMinimumWidth )
    {
        const GXFloat* width = static_cast<const GXFloat*> ( data );
        _minimumSize.SetX ( *width );

        if ( _boundsLocal.GetWidth () >= _minimumSize.GetX () ) return;

        Resize ( _boundsLocal.min.GetX (), _boundsLocal.min.GetY (), _minimumSize.GetX (), _boundsLocal.GetHeight () );

        return;
    }

    if ( message == eGXUIMessage::DraggableAreaSetMinimumHeight )
    {
        const GXFloat* height = static_cast<const GXFloat*> ( data );
        _minimumSize.SetY ( *height );

        if ( _boundsLocal.GetHeight () >= _minimumSize.GetY () ) return;

        Resize ( _boundsLocal.min.GetX (), _boundsLocal.min.GetY (), _boundsLocal.GetWidth (), _minimumSize.GetY () );
        return;
    }

    if ( message == eGXUIMessage::Drag )
    {
        const GXVec2* delta = static_cast<const GXVec2*> ( data );

        GXAABB newBounds;
        newBounds.AddVertex ( _boundsLocal.min.GetX () + delta->GetX (), _boundsLocal.min.GetY () + delta->GetY (), _boundsLocal.min.GetZ () );
        newBounds.AddVertex ( _boundsLocal.max.GetX () + delta->GetX (), _boundsLocal.max.GetY () + delta->GetY (), _boundsLocal.max.GetZ () );

        UpdateBoundsWorld ( newBounds );
        UpdateAreas ();

        if ( _renderer )
            _renderer->OnUpdate ();

        GXWidgetIterator iterator;
        GXWidget* p = iterator.Init ( _childs );

        while ( p )
        {
            if ( p->IsDraggable () )
            {
                p->OnMessage ( eGXUIMessage::Drag, data );
            }
            else
            {
                const GXAABB& bounds = p->GetBoundsLocal ();
                memcpy ( &newBounds, &bounds, sizeof ( GXAABB ) );
                p->OnMessage ( eGXUIMessage::Resize, &newBounds );
            }

            p = iterator.GetNext ();
        }

        return;
    }

    GXWidget::OnMessage ( message, data );
}

GXVoid GXUIDragableArea::SetMinimumWidth ( GXFloat width )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::DraggableAreaSetMinimumWidth, &width, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetMinimumWidth () const
{
    return _minimumSize.GetX ();
}

GXVoid GXUIDragableArea::SetMinimumHeight ( GXFloat height )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::DraggableAreaSetMinimumHeight, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetMinimumHeight () const
{
    return _minimumSize.GetY ();
}

GXVoid GXUIDragableArea::SetHeaderHeight ( GXFloat height )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::DraggableAreaSetHeaderHeght, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetHeaderHeight () const
{
    return _headerHeight;
}

GXVoid GXUIDragableArea::SetBorderThickness ( GXFloat thickness )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::DraggableAreaSetBorderThickness, &thickness, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetBorderThickness () const
{
    return _borderThickness;
}

GXVoid GXUIDragableArea::SetOnResizeCallback ( GXVoid* context, PFNGXUIDRAGABLEAREAONRESIZEPROC callback )
{
    _context = context;
    _onResize = callback;
}

eGXDraggableAreaResizeMode GXUIDragableArea::GetResizeMode ( const GXVec2 &mousePosition ) const
{
    GXVec3 mouse3D ( mousePosition.GetX (), mousePosition.GetY (), 0.0f );

    if ( _headerArea.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::Dragging;
    if ( _clientArea.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::None;

    if ( _cornerBottomRight.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockTopLeft;
    if ( _cornerTopLeft.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockBottomRight;
    if ( _cornerTopRight.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockBottomLeft;
    if ( _cornerBottomLeft.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockTopRight;

    if ( _borderRight.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthLockLeft;
    if ( _borderLeft.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthLockRight;
    if ( _borderBottom.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::HeightLockTop;

    return eGXDraggableAreaResizeMode::HeightLockBottom;
}

GXVoid GXUIDragableArea::UpdateAreas ()
{
    _cornerTopLeft.Empty ();
    _cornerTopRight.Empty ();
    _cornerBottomLeft.Empty ();
    _cornerBottomRight.Empty ();

    _cornerBottomLeft.AddVertex ( _boundsLocal.min );
    _cornerBottomLeft.AddVertex ( _boundsLocal.min.GetX () + _borderThickness, _boundsLocal.min.GetY () + _borderThickness, _boundsLocal.max.GetZ () );

    _cornerTopRight.AddVertex ( _boundsLocal.max );
    _cornerTopRight.AddVertex ( _boundsLocal.max.GetX () - _borderThickness, _boundsLocal.max.GetY () - _borderThickness, _boundsLocal.min.GetZ () );

    _cornerTopLeft.AddVertex ( _boundsLocal.min.GetX (), _boundsLocal.max.GetY () - _borderThickness, _boundsLocal.min.GetZ () );
    _cornerTopLeft.AddVertex ( _boundsLocal.min.GetX () + _borderThickness, _boundsLocal.max.GetY (), _boundsLocal.max.GetZ () );

    _cornerBottomRight.AddVertex ( _boundsLocal.max.GetX () - _borderThickness, _boundsLocal.min.GetY (),  _boundsLocal.min.GetZ () );
    _cornerBottomRight.AddVertex ( _boundsLocal.max.GetX (), _boundsLocal.min.GetY () + _borderThickness, _boundsLocal.max.GetZ () );

    _borderBottom.Empty ();
    _borderTop.Empty ();
    _borderLeft.Empty ();
    _borderRight.Empty ();

    _borderLeft.AddVertex ( _boundsLocal.min.GetX (), _boundsLocal.min.GetY () + _borderThickness, _boundsLocal.min.GetZ () );
    _borderLeft.AddVertex ( _boundsLocal.min.GetX () + _borderThickness, _boundsLocal.max.GetY () - _borderThickness, _boundsLocal.max.GetZ () );

    _borderTop.AddVertex ( _boundsLocal.min.GetX () + _borderThickness, _boundsLocal.max.GetY () - _borderThickness, _boundsLocal.min.GetZ () );
    _borderTop.AddVertex ( _boundsLocal.max.GetX () - _borderThickness, _boundsLocal.max.GetY (), _boundsLocal.max.GetZ () );

    _borderRight.AddVertex ( _boundsLocal.max.GetX () - _borderThickness, _boundsLocal.min.GetY () + _borderThickness, _boundsLocal.min.GetZ () );
    _borderRight.AddVertex ( _boundsLocal.max.GetX (), _boundsLocal.max.GetY () - _borderThickness, _boundsLocal.max.GetZ () );

    _borderBottom.AddVertex ( _boundsLocal.min.GetX () + _borderThickness, _boundsLocal.min.GetY (), _boundsLocal.min.GetZ () );
    _borderBottom.AddVertex ( _boundsLocal.max.GetX () - _borderThickness, _boundsLocal.min.GetY () + _borderThickness, _boundsLocal.max.GetZ () );

    _headerArea.Empty ();
    _headerArea.AddVertex ( _boundsLocal.min.GetX () + _borderThickness, _boundsLocal.max.GetY () - _borderThickness - _headerHeight, _boundsLocal.min.GetZ () );
    _headerArea.AddVertex ( _boundsLocal.max.GetX () - _borderThickness, _boundsLocal.max.GetY () - _borderThickness, _boundsLocal.max.GetZ () );

    _clientArea.Empty ();
    _clientArea.AddVertex ( _boundsLocal.min.GetX () + _borderThickness, _boundsLocal.min.GetY () + _borderThickness, _boundsLocal.min.GetZ () );
    _clientArea.AddVertex ( _boundsLocal.max.GetX () - _borderThickness, _boundsLocal.max.GetY () - _borderThickness - _headerHeight, _boundsLocal.max.GetZ () );
}

GXVoid GXUIDragableArea::UpdateCursor ( const GXVec2 &mousePosition )
{
    const HCURSOR* cursor = nullptr;

    switch ( GetResizeMode ( mousePosition ) )
    {
        case eGXDraggableAreaResizeMode::WidthLockRight:
        case eGXDraggableAreaResizeMode::WidthLockLeft:
            cursor = &_horizontalArrow;
        break;

        case eGXDraggableAreaResizeMode::HeightLockTop:
        case eGXDraggableAreaResizeMode::HeightLockBottom:
            cursor = &_verticalArrow;
        break;

        case eGXDraggableAreaResizeMode::WidthAndHeightLockTopRight:
        case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomLeft:
            cursor = &_northeastSouthwestArrow;
        break;

        case eGXDraggableAreaResizeMode::WidthAndHeightLockTopLeft:
        case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomRight:
            cursor = &_northwestSoutheastArrow;
        break;

        case eGXDraggableAreaResizeMode::Dragging:
            cursor = &_crossArrow;
        break;

        case eGXDraggableAreaResizeMode::None:
        default:
            cursor = &_standartArrow;
        break;
    }

    if ( cursor == _currentCursor ) return;

    _currentCursor = cursor;
    SetCursor ( *_currentCursor );
}
