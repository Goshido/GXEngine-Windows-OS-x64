//version 1.1

#include <GXEngine/GXUIDraggableArea.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>


#define DEFAULT_MINIMUM_WIDTH		0.7f
#define DEFAULT_MINIMUM_HEIGHT		1.5f

#define DEFAULT_BORDER_THICKNESS	0.2f //2 mm
#define DEFAULT_HEADER_HEIGHT		0.9f //9 mm

#define TOLERANCE_FACTOR			0.25f


GXUIDragableArea::GXUIDragableArea ( GXWidget* parent ):
GXWidget ( parent )
{
	headerHeight = DEFAULT_HEADER_HEIGHT * gx_ui_Scale;
	borderThickness = DEFAULT_BORDER_THICKNESS * gx_ui_Scale;
	minimumSize.Init ( DEFAULT_MINIMUM_WIDTH * gx_ui_Scale, DEFAULT_MINIMUM_HEIGHT * gx_ui_Scale );
	isDraggable = GX_TRUE;
	memset ( &lastMousePosition, 0, sizeof ( GXVec2 ) );
	OnResize = nullptr;
	handler = nullptr;
	resizeMode = eGXDraggableAreaResizeMode::None;

	standartArrow = LoadCursorW ( 0, IDC_ARROW );
	verticalArrow = LoadCursorW ( 0, IDC_SIZENS );
	horizontalArrow = LoadCursorW ( 0, IDC_SIZEWE );
	crossArrow = LoadCursorW ( 0, IDC_SIZEALL );
	northwestSoutheastArrow = LoadCursorW ( 0, IDC_SIZENWSE );
	northeastSouthwestArrow = LoadCursorW ( 0, IDC_SIZENESW );
	currentCursor = &standartArrow;
}

GXUIDragableArea::~GXUIDragableArea ()
{
	//NOTHING
}

GXVoid GXUIDragableArea::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_LMBDOWN:
		{
			const GXVec2* pos = (const GXVec2*)data;

			resizeMode = GetResizeMode ( *pos );
			memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );
				
			if ( resizeMode != eGXDraggableAreaResizeMode::None )
				GXTouchSurface::GetInstance ().LockCursor ( this );
		}
		break;

		case GX_MSG_LMBUP:
		{
			resizeMode = eGXDraggableAreaResizeMode::None;

			GXTouchSurface& touchSurface = GXTouchSurface::GetInstance ();

			if ( touchSurface.GetLockedCursorWidget () == this )
				touchSurface.ReleaseCursor ();
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			SetCursor ( standartArrow );
			currentCursor = &standartArrow;
		}
		break;

		case GX_MSG_MOUSE_OVER:
		{
			const GXVec2* pos = (const GXVec2*)data;
			UpdateCursor ( *pos );
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = (const GXVec2*)data;

			switch ( resizeMode )
			{
				case eGXDraggableAreaResizeMode::Dragging:
				{
					GXVec2 delta;
					delta.Substract ( *pos, lastMousePosition );

					OnMessage ( GX_MSG_DRAG, &delta );

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthLockLeft:
				{
					GXAABB bounds = boundsLocal;
					bounds.max.data[ 0 ] += pos->data[ 0 ] - lastMousePosition.data[ 0 ];

					if ( bounds.GetWidth () < minimumSize.GetX () )
					{
						if ( fabs ( boundsLocal.GetWidth () - minimumSize.GetX () ) < TOLERANCE_FACTOR ) break;

						bounds.max.SetX ( bounds.min.GetX () + minimumSize.GetX () );
						lastMousePosition.SetX ( boundsWorld.min.GetX () + minimumSize.GetX () );
					}
					else
					{
						lastMousePosition.SetX ( pos->GetX () );
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					if ( renderer )
						renderer->OnUpdate ();

					if ( OnResize )
						OnResize ( handler, *this, boundsLocal.GetWidth (), boundsLocal.GetHeight () );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthLockRight:
				{
					GXAABB bounds = boundsLocal;
					bounds.min.data[ 0 ] += pos->data[ 0 ] - lastMousePosition.data[ 0 ];

					if ( bounds.GetWidth () < minimumSize.GetX () )
					{
						if ( fabs ( boundsLocal.GetWidth () - minimumSize.GetX () ) < TOLERANCE_FACTOR ) break;

						bounds.min.SetX ( bounds.max.GetX () - minimumSize.GetX () );
						lastMousePosition.SetX ( boundsWorld.max.GetX () - minimumSize.GetX () );
					}
					else
					{
						lastMousePosition.SetX ( pos->GetX () );
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, boundsLocal.GetWidth (), boundsLocal.GetHeight () );
				}
				break;

				case eGXDraggableAreaResizeMode::HeightLockBottom:
				{
					GXAABB bounds = boundsLocal;
					bounds.max.data[ 1 ] += pos->data[ 1 ] - lastMousePosition.data[ 1 ];

					if ( bounds.GetHeight () < minimumSize.GetY () )
					{
						if ( fabs ( boundsLocal .GetHeight ()- minimumSize.GetY () ) < TOLERANCE_FACTOR ) break;

						bounds.max.SetY ( bounds.min.GetY () + minimumSize.GetY () );
						lastMousePosition.SetY ( boundsWorld.min.GetY () + minimumSize.GetY () );
					}
					else
					{
						lastMousePosition.SetY ( pos->GetY () );
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					if ( renderer )
						renderer->OnUpdate ();

					if ( OnResize )
						OnResize ( handler, *this, boundsLocal.GetWidth (), boundsLocal.GetHeight () );
				}
				break;

				case eGXDraggableAreaResizeMode::HeightLockTop:
				{
					GXAABB bounds = boundsLocal;
					bounds.min.data[ 1 ] += pos->data[ 1 ] - lastMousePosition.data[ 1 ];

					if ( bounds.GetHeight () < minimumSize.GetY () )
					{
						if ( fabs ( boundsLocal.GetHeight () - minimumSize.GetY () ) < TOLERANCE_FACTOR ) break;

						bounds.min.SetY ( bounds.max.GetY () - minimumSize.GetY () );
						lastMousePosition.SetY ( boundsWorld.max.GetY () - minimumSize.GetY () );
					}
					else
					{
						lastMousePosition.SetY ( pos->GetY () );
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, boundsLocal.GetWidth (), boundsLocal.GetHeight () );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomLeft:
				{
					GXVec2 delta;
					delta.Substract ( *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.max.data[ 0 ] += delta.data[ 0 ];
					bounds.max.data[ 1 ] += delta.data[ 1 ];

					GXFloat width = bounds.GetWidth ();
					GXFloat heigth = bounds.GetHeight ();

					if ( width < minimumSize.GetX () && heigth < minimumSize.GetY () ) break;

					if ( width < minimumSize.GetX () )
					{
						bounds.max.SetX ( boundsLocal.min.GetX () + minimumSize.GetX () );
						lastMousePosition.SetX ( boundsWorld.min.GetX () + minimumSize.GetX () );
					}
					else
					{
						lastMousePosition.SetX ( pos->GetX () );
					}

					if ( heigth < minimumSize.GetY () )
					{
						bounds.max.SetY ( boundsLocal.min.GetY () + minimumSize.GetY () );
						lastMousePosition.SetY ( boundsWorld.min.GetY () + minimumSize.GetY () );
					}
					else
					{
						lastMousePosition.SetY ( pos->GetY () );
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					if ( renderer )
						renderer->OnUpdate ();

					if ( OnResize )
						OnResize ( handler, *this, boundsLocal.GetWidth (), boundsLocal.GetHeight () );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomRight:
				{
					GXVec2 delta;
					delta.Substract ( *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.min.data[ 0 ] += delta.data[ 0 ];
					bounds.max.data[ 1 ] += delta.data[ 1 ];

					GXFloat width = bounds.GetWidth ();
					GXFloat heigth = bounds.GetHeight ();

					if ( width < minimumSize.GetX () && heigth < minimumSize.GetY () ) break;

					if ( width < minimumSize.GetX () )
					{
						bounds.min.SetX ( boundsLocal.max.GetX () - minimumSize.GetX () );
						lastMousePosition.SetX ( boundsWorld.max.GetX () - minimumSize.GetX () );
					}
					else
					{
						lastMousePosition.SetX ( pos->GetX () );
					}

					if ( heigth < minimumSize.GetY () )
					{
						bounds.max.SetY ( boundsLocal.min.GetY () + minimumSize.GetY () );
						lastMousePosition.SetY ( boundsWorld.min.GetY () + minimumSize.GetY () );
					}
					else
					{
						lastMousePosition.SetY ( pos->GetY () );
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, boundsLocal.GetWidth (), boundsLocal.GetHeight () );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthAndHeightLockTopLeft:
				{
					GXVec2 delta;
					delta.Substract ( *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.max.data[ 0 ] += delta.data[ 0 ];
					bounds.min.data[ 1 ] += delta.data[ 1 ];

					GXFloat width = bounds.GetWidth ();
					GXFloat heigth = bounds.GetHeight ();

					if ( width < minimumSize.GetX () && heigth < minimumSize.GetY () ) break;

					if ( width < minimumSize.GetX () )
					{
						bounds.max.SetX ( boundsLocal.min.GetX () + minimumSize.GetX () );
						lastMousePosition.SetX ( boundsWorld.min.GetX () + minimumSize.GetX () );
					}
					else
					{
						lastMousePosition.SetX ( pos->GetX () );
					}

					if ( heigth < minimumSize.GetY () )
					{
						bounds.min.SetY ( boundsLocal.max.GetY () - minimumSize.GetY () );
						lastMousePosition.SetY ( boundsWorld.max.GetY () - minimumSize.GetY () );
					}
					else
					{
						lastMousePosition.SetY ( pos->GetY () );
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, boundsLocal.GetWidth (), boundsLocal.GetHeight () );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthAndHeightLockTopRight:
				{
					GXVec2 delta;
					delta.Substract ( *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.min.data[ 0 ] += delta.data[ 0 ];
					bounds.min.data[ 1 ] += delta.data[ 1 ];

					GXFloat width = bounds.GetWidth ();
					GXFloat heigth = bounds.GetHeight ();

					if ( width < minimumSize.GetX () && heigth < minimumSize.GetY () ) break;

					if ( width < minimumSize.GetX () )
					{
						bounds.min.SetX ( boundsLocal.max.GetX () - minimumSize.GetX () );
						lastMousePosition.SetX ( boundsWorld.max.GetX () - minimumSize.GetX () );
					}
					else
					{
						lastMousePosition.SetX ( pos->GetX () );
					}

					if ( heigth < minimumSize.GetY () )
					{
						bounds.min.SetY ( boundsLocal.max.GetY () - minimumSize.GetY () );
						lastMousePosition.SetY ( boundsWorld.max.GetY () - minimumSize.GetY () );
					}
					else
					{
						lastMousePosition.SetY ( pos->GetY () );
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, boundsLocal.GetWidth (), boundsLocal.GetHeight () );
				}
				break;

				case eGXDraggableAreaResizeMode::None:
					UpdateCursor ( *pos );
				break;

				default:
					//NOTHING
				break;
			}
		}
		break;

		case GX_MSG_RESIZE:
		{
			const GXAABB* bounds = (const GXAABB*)data;

			GXAABB correctedBounds;
			correctedBounds.AddVertex ( bounds->min );

			GXVec3 maxPoint = bounds->max;
			GXFloat length = bounds->GetWidth ();

			if ( length < minimumSize.GetX () )
				maxPoint.SetX ( bounds->min.GetX () + length );

			length = bounds->GetHeight ();

			if ( length < minimumSize.GetY () )
				maxPoint.SetY ( bounds->min.GetY () + length );

			correctedBounds.AddVertex ( maxPoint );

			UpdateBoundsWorld ( correctedBounds );
			UpdateAreas ();

			GXVec2 dragDelta ( 0.0f, 0.0f );
			OnMessage ( GX_MSG_DRAG, &dragDelta );

			if ( OnResize )
				OnResize ( handler, *this, boundsLocal.GetWidth (), boundsLocal.GetHeight () );
		}
		break;
		
		case GX_MSG_DRAGGABLE_AREA_SET_HEADER_HEIGHT:
		{
			const GXFloat* height = (const GXFloat*)data;
			headerHeight = *height;

			UpdateAreas ();

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_DRAGGABLE_AREA_SET_BORDER_THICKNESS:
		{
			const GXFloat* thickness = (const GXFloat*)data;
			borderThickness = *thickness;

			UpdateAreas ();

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_DRAGGABLE_AREA_SET_MINIMUM_WIDTH:
		{
			const GXFloat* width = (const GXFloat*)data;
			minimumSize.SetX ( *width );

			if ( boundsLocal.GetWidth () >= minimumSize.GetX () ) break;

			Resize ( boundsLocal.min.GetX (), boundsLocal.min.GetY (), minimumSize.GetX (), boundsLocal.GetHeight () );
		}
		break;

		case GX_MSG_DRAGGABLE_AREA_SET_MINIMUM_HEIGHT:
		{
			const GXFloat* height = (const GXFloat*)data;
			minimumSize.SetY ( *height );

			if ( boundsLocal.GetHeight () >= minimumSize.GetY () ) break;

			Resize ( boundsLocal.min.GetX (), boundsLocal.min.GetY (), boundsLocal.GetWidth (), minimumSize.GetY () );
		}
		break;

		case GX_MSG_DRAG:
		{
			const GXVec2* delta = (const GXVec2*)data;

			GXAABB newBounds;
			newBounds.AddVertex ( boundsLocal.min.GetX () + delta->GetX (), boundsLocal.min.GetY () + delta->GetY (), boundsLocal.min.GetZ () );
			newBounds.AddVertex ( boundsLocal.max.GetX () + delta->GetX (), boundsLocal.max.GetY () + delta->GetY (), boundsLocal.max.GetZ () );

			UpdateBoundsWorld ( newBounds );
			UpdateAreas ();

			if ( renderer )
				renderer->OnUpdate ();

			GXWidgetIterator iterator;
			GXWidget* p = iterator.Init ( childs );

			while ( p )
			{
				if ( p->IsDraggable () )
				{
					p->OnMessage ( GX_MSG_DRAG, data );
				}
				else
				{
					const GXAABB& bounds = p->GetBoundsLocal ();
					memcpy ( &newBounds, &bounds, sizeof ( GXAABB ) );
					p->OnMessage ( GX_MSG_RESIZE, &newBounds );
				}

				p = iterator.GetNext ();
			}
		}
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUIDragableArea::SetMinimumWidth ( GXFloat width )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_DRAGGABLE_AREA_SET_MINIMUM_WIDTH, &width, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetMinimumWidth () const
{
	return minimumSize.GetX ();
}

GXVoid GXUIDragableArea::SetMinimumHeight ( GXFloat height )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_DRAGGABLE_AREA_SET_MINIMUM_HEIGHT, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetMinimumHeight () const
{
	return minimumSize.GetY ();
}

GXVoid GXUIDragableArea::SetHeaderHeight ( GXFloat height )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_DRAGGABLE_AREA_SET_HEADER_HEIGHT, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetHeaderHeight () const
{
	return headerHeight;
}

GXVoid GXUIDragableArea::SetBorderThickness ( GXFloat thickness )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_DRAGGABLE_AREA_SET_BORDER_THICKNESS, &thickness, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetBorderThickness () const
{
	return borderThickness;
}

GXVoid GXUIDragableArea::SetOnResizeCallback ( GXVoid* handlerObject, PFNGXUIDRAGABLEAREAONRESIZEPROC callback )
{
	handler = handlerObject;
	OnResize = callback;
}

eGXDraggableAreaResizeMode GXUIDragableArea::GetResizeMode ( const GXVec2 &mousePosition ) const
{
	GXVec3 mouse3D ( mousePosition.GetX (), mousePosition.GetY (), 0.0f );

	if ( headerArea.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::Dragging;
	if ( clientArea.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::None;

	if ( cornerBottomRight.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockTopLeft;
	if ( cornerTopLeft.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockBottomRight;
	if ( cornerTopRight.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockBottomLeft;
	if ( cornerBottomLeft.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockTopRight;

	if ( borderRight.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthLockLeft;
	if ( borderLeft.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::WidthLockRight;
	if ( borderBottom.IsOverlaped ( mouse3D ) ) return eGXDraggableAreaResizeMode::HeightLockTop;

	return eGXDraggableAreaResizeMode::HeightLockBottom;
}

GXVoid GXUIDragableArea::UpdateAreas ()
{
	cornerTopLeft.Empty ();
	cornerTopRight.Empty ();
	cornerBottomLeft.Empty ();
	cornerBottomRight.Empty ();

	cornerBottomLeft.AddVertex ( boundsLocal.min );
	cornerBottomLeft.AddVertex ( boundsLocal.min.GetX () + borderThickness, boundsLocal.min.GetY () + borderThickness, boundsLocal.max.GetZ () );

	cornerTopRight.AddVertex ( boundsLocal.max );
	cornerTopRight.AddVertex ( boundsLocal.max.GetX () - borderThickness, boundsLocal.max.GetY () - borderThickness, boundsLocal.min.GetZ () );

	cornerTopLeft.AddVertex ( boundsLocal.min.GetX (), boundsLocal.max.GetY () - borderThickness, boundsLocal.min.GetZ () );
	cornerTopLeft.AddVertex ( boundsLocal.min.GetX () + borderThickness, boundsLocal.max.GetY (), boundsLocal.max.GetZ () );

	cornerBottomRight.AddVertex ( boundsLocal.max.GetX () - borderThickness, boundsLocal.min.GetY (),  boundsLocal.min.GetZ () );
	cornerBottomRight.AddVertex ( boundsLocal.max.GetX (), boundsLocal.min.GetY () + borderThickness, boundsLocal.max.GetZ () );

	borderBottom.Empty ();
	borderTop.Empty ();
	borderLeft.Empty ();
	borderRight.Empty ();

	borderLeft.AddVertex ( boundsLocal.min.GetX (), boundsLocal.min.GetY () + borderThickness, boundsLocal.min.GetZ () );
	borderLeft.AddVertex ( boundsLocal.min.GetX () + borderThickness, boundsLocal.max.GetY () - borderThickness, boundsLocal.max.GetZ () );

	borderTop.AddVertex ( boundsLocal.min.GetX () + borderThickness, boundsLocal.max.GetY () - borderThickness, boundsLocal.min.GetZ () );
	borderTop.AddVertex ( boundsLocal.max.GetX () - borderThickness, boundsLocal.max.GetY (), boundsLocal.max.GetZ () );

	borderRight.AddVertex ( boundsLocal.max.GetX () - borderThickness, boundsLocal.min.GetY () + borderThickness, boundsLocal.min.GetZ () );
	borderRight.AddVertex ( boundsLocal.max.GetX (), boundsLocal.max.GetY () - borderThickness, boundsLocal.max.GetZ () );

	borderBottom.AddVertex ( boundsLocal.min.GetX () + borderThickness, boundsLocal.min.GetY (), boundsLocal.min.GetZ () );
	borderBottom.AddVertex ( boundsLocal.max.GetX () - borderThickness, boundsLocal.min.GetY () + borderThickness, boundsLocal.max.GetZ () );

	headerArea.Empty ();
	headerArea.AddVertex ( boundsLocal.min.GetX () + borderThickness, boundsLocal.max.GetY () - borderThickness - headerHeight, boundsLocal.min.GetZ () );
	headerArea.AddVertex ( boundsLocal.max.GetX () - borderThickness, boundsLocal.max.GetY () - borderThickness, boundsLocal.max.GetZ () );

	clientArea.Empty ();
	clientArea.AddVertex ( boundsLocal.min.GetX () + borderThickness, boundsLocal.min.GetY () + borderThickness, boundsLocal.min.GetZ () );
	clientArea.AddVertex ( boundsLocal.max.GetX () - borderThickness, boundsLocal.max.GetY () - borderThickness - headerHeight, boundsLocal.max.GetZ () );
}

GXVoid GXUIDragableArea::UpdateCursor ( const GXVec2 &mousePosition )
{
	const HCURSOR* cursor = nullptr;

	switch ( GetResizeMode ( mousePosition ) )
	{
		case eGXDraggableAreaResizeMode::WidthLockRight:
		case eGXDraggableAreaResizeMode::WidthLockLeft:
			cursor = &horizontalArrow;
		break;

		case eGXDraggableAreaResizeMode::HeightLockTop:
		case eGXDraggableAreaResizeMode::HeightLockBottom:
			cursor = &verticalArrow;
		break;

		case eGXDraggableAreaResizeMode::WidthAndHeightLockTopRight:
		case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomLeft:
			cursor = &northeastSouthwestArrow;
		break;

		case eGXDraggableAreaResizeMode::WidthAndHeightLockTopLeft:
		case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomRight:
			cursor = &northwestSoutheastArrow;
		break;

		case eGXDraggableAreaResizeMode::Dragging:
			cursor = &crossArrow;
		break;

		case eGXDraggableAreaResizeMode::None:
		default:
			cursor = &standartArrow;
		break;
	}

	if ( cursor == currentCursor ) return;

	currentCursor = cursor;
	SetCursor ( *currentCursor );
}
