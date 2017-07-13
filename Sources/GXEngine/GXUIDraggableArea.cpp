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
	minimumSize = GXCreateVec2 ( DEFAULT_MINIMUM_WIDTH * gx_ui_Scale, DEFAULT_MINIMUM_HEIGHT * gx_ui_Scale );
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
				const GXVec2* pos = (const GXVec2*)data;
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
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					OnMessage ( GX_MSG_DRAG, &delta );

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthLockLeft:
				{
					GXAABB bounds = boundsLocal;
					bounds.max.x += pos->x - lastMousePosition.x;

					if ( GXGetAABBWidth ( bounds ) < minimumSize.x )
					{
						if ( fabs ( GXGetAABBWidth ( boundsLocal ) - minimumSize.x ) < TOLERANCE_FACTOR ) break;

						bounds.max.x = bounds.min.x + minimumSize.x;
						lastMousePosition.x = boundsWorld.min.x + minimumSize.x;
					}
					else
					{
						lastMousePosition.x = pos->x;
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					if ( renderer )
						renderer->OnUpdate ();

					if ( OnResize )
						OnResize ( handler, *this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthLockRight:
				{
					GXAABB bounds = boundsLocal;
					bounds.min.x += pos->x - lastMousePosition.x;

					if ( GXGetAABBWidth ( bounds ) < minimumSize.x )
					{
						if ( fabs ( GXGetAABBWidth ( boundsLocal ) - minimumSize.x ) < TOLERANCE_FACTOR ) break;

						bounds.min.x = bounds.max.x - minimumSize.x;
						lastMousePosition.x = boundsWorld.max.x - minimumSize.x;
					}
					else
					{
						lastMousePosition.x = pos->x;
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case eGXDraggableAreaResizeMode::HeightLockBottom:
				{
					GXAABB bounds = boundsLocal;
					bounds.max.y += pos->y - lastMousePosition.y;

					if ( GXGetAABBHeight ( bounds ) < minimumSize.y )
					{
						if ( fabs ( GXGetAABBHeight ( boundsLocal ) - minimumSize.y ) < TOLERANCE_FACTOR ) break;

						bounds.max.y = bounds.min.y + minimumSize.y;
						lastMousePosition.y = boundsWorld.min.y + minimumSize.y;
					}
					else
					{
						lastMousePosition.y = pos->y;
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					if ( renderer )
						renderer->OnUpdate ();

					if ( OnResize )
						OnResize ( handler, *this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case eGXDraggableAreaResizeMode::HeightLockTop:
				{
					GXAABB bounds = boundsLocal;
					bounds.min.y += pos->y - lastMousePosition.y;

					if ( GXGetAABBHeight ( bounds ) < minimumSize.y )
					{
						if ( fabs ( GXGetAABBHeight ( boundsLocal ) - minimumSize.y ) < TOLERANCE_FACTOR ) break;

						bounds.min.y = bounds.max.y - minimumSize.y;
						lastMousePosition.y = boundsWorld.max.y - minimumSize.y;
					}
					else
					{
						lastMousePosition.y = pos->y;
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomLeft:
				{
					GXVec2 delta;
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.max.x += delta.x;
					bounds.max.y += delta.y;

					GXFloat width = GXGetAABBWidth ( bounds );
					GXFloat heigth = GXGetAABBHeight ( bounds );

					if ( width < minimumSize.x && heigth < minimumSize.y ) break;

					if ( width < minimumSize.x )
					{
						bounds.max.x = boundsLocal.min.x + minimumSize.x;
						lastMousePosition.x = boundsWorld.min.x + minimumSize.x;
					}
					else
					{
						lastMousePosition.x = pos->x;
					}

					if ( heigth < minimumSize.y )
					{
						bounds.max.y = boundsLocal.min.y + minimumSize.y;
						lastMousePosition.y = boundsWorld.min.y + minimumSize.y;
					}
					else
					{
						lastMousePosition.y = pos->y;
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					if ( renderer )
						renderer->OnUpdate ();

					if ( OnResize )
						OnResize ( handler, *this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthAndHeightLockBottomRight:
				{
					GXVec2 delta;
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.min.x += delta.x;
					bounds.max.y += delta.y;

					GXFloat width = GXGetAABBWidth ( bounds );
					GXFloat heigth = GXGetAABBHeight ( bounds );

					if ( width < minimumSize.x && heigth < minimumSize.y ) break;

					if ( width < minimumSize.x )
					{
						bounds.min.x = boundsLocal.max.x - minimumSize.x;
						lastMousePosition.x = boundsWorld.max.x - minimumSize.x;
					}
					else
					{
						lastMousePosition.x = pos->x;
					}

					if ( heigth < minimumSize.y )
					{
						bounds.max.y = boundsLocal.min.y + minimumSize.y;
						lastMousePosition.y = boundsWorld.min.y + minimumSize.y;
					}
					else
					{
						lastMousePosition.y = pos->y;
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthAndHeightLockTopLeft:
				{
					GXVec2 delta;
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.max.x += delta.x;
					bounds.min.y += delta.y;

					GXFloat width = GXGetAABBWidth ( bounds );
					GXFloat heigth = GXGetAABBHeight ( bounds );

					if ( width < minimumSize.x && heigth < minimumSize.y ) break;

					if ( width < minimumSize.x )
					{
						bounds.max.x = boundsLocal.min.x + minimumSize.x;
						lastMousePosition.x = boundsWorld.min.x + minimumSize.x;
					}
					else
					{
						lastMousePosition.x = pos->x;
					}

					if ( heigth < minimumSize.y )
					{
						bounds.min.y = boundsLocal.max.y - minimumSize.y;
						lastMousePosition.y = boundsWorld.max.y - minimumSize.y;
					}
					else
					{
						lastMousePosition.y = pos->y;
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case eGXDraggableAreaResizeMode::WidthAndHeightLockTopRight:
				{
					GXVec2 delta;
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.min.x += delta.x;
					bounds.min.y += delta.y;

					GXFloat width = GXGetAABBWidth ( bounds );
					GXFloat heigth = GXGetAABBHeight ( bounds );

					if ( width < minimumSize.x && heigth < minimumSize.y ) break;

					if ( width < minimumSize.x )
					{
						bounds.min.x = boundsLocal.max.x - minimumSize.x;
						lastMousePosition.x = boundsWorld.max.x - minimumSize.x;
					}
					else
					{
						lastMousePosition.x = pos->x;
					}

					if ( heigth < minimumSize.y )
					{
						bounds.min.y = boundsLocal.max.y - minimumSize.y;
						lastMousePosition.y = boundsWorld.max.y - minimumSize.y;
					}
					else
					{
						lastMousePosition.y = pos->y;
					}

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, *this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
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
			GXAddVertexToAABB ( correctedBounds, bounds->min );

			GXVec3 maxPoint = bounds->max;
			GXFloat length = GXGetAABBWidth ( *bounds );

			if ( length < minimumSize.x )
				maxPoint.x = bounds->min.x + length;

			length = GXGetAABBHeight ( *bounds );

			if ( length < minimumSize.y )
				maxPoint.y = bounds->min.y + length;

			GXAddVertexToAABB ( correctedBounds, maxPoint );

			UpdateBoundsWorld ( correctedBounds );
			UpdateAreas ();

			GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
			OnMessage ( GX_MSG_DRAG, &dragDelta );

			if ( OnResize )
				OnResize ( handler, *this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
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
			minimumSize.x = *width;

			if ( GXGetAABBWidth ( boundsLocal ) >= minimumSize.x ) break;

			Resize ( boundsLocal.min.x, boundsLocal.min.y, minimumSize.x, GXGetAABBHeight ( boundsLocal ) );
		}
		break;

		case GX_MSG_DRAGGABLE_AREA_SET_MINIMUM_HEIGHT:
		{
			const GXFloat* height = (const GXFloat*)data;
			minimumSize.y = *height;

			if ( GXGetAABBHeight ( boundsLocal ) >= minimumSize.y ) break;

			Resize ( boundsLocal.min.x, boundsLocal.min.y, GXGetAABBWidth ( boundsLocal ), minimumSize.y );
		}
		break;

		case GX_MSG_DRAG:
		{
			const GXVec2* delta = (const GXVec2*)data;

			GXAABB newBounds;
			GXAddVertexToAABB ( newBounds, boundsLocal.min.x + delta->x, boundsLocal.min.y + delta->y, boundsLocal.min.z );
			GXAddVertexToAABB ( newBounds, boundsLocal.max.x + delta->x, boundsLocal.max.y + delta->y, boundsLocal.max.z );

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
	return minimumSize.x;
}

GXVoid GXUIDragableArea::SetMinimumHeight ( GXFloat height )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_DRAGGABLE_AREA_SET_MINIMUM_HEIGHT, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetMinimumHeight () const
{
	return minimumSize.y;
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

GXVoid GXUIDragableArea::SetOnResizeCallback ( GXVoid* handler, PFNGXUIDRAGABLEAREAONRESIZEPROC callback )
{
	this->handler = handler;
	OnResize = callback;
}

eGXDraggableAreaResizeMode GXUIDragableArea::GetResizeMode ( const GXVec2 &mousePosition ) const
{
	GXVec3 mouse3D = GXCreateVec3 ( mousePosition.x, mousePosition.y, 0.0f );

	if ( GXIsOverlapedAABBVec3 ( headerArea, mouse3D ) ) return eGXDraggableAreaResizeMode::Dragging;
	if ( GXIsOverlapedAABBVec3 ( clientArea, mouse3D ) ) return eGXDraggableAreaResizeMode::None;

	if ( GXIsOverlapedAABBVec3 ( cornerBottomRight, mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockTopLeft;
	if ( GXIsOverlapedAABBVec3 ( cornerTopLeft, mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockBottomRight;
	if ( GXIsOverlapedAABBVec3 ( cornerTopRight, mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockBottomLeft;
	if ( GXIsOverlapedAABBVec3 ( cornerBottomLeft, mouse3D ) ) return eGXDraggableAreaResizeMode::WidthAndHeightLockTopRight;

	if ( GXIsOverlapedAABBVec3 ( borderRight, mouse3D ) ) return eGXDraggableAreaResizeMode::WidthLockLeft;
	if ( GXIsOverlapedAABBVec3 ( borderLeft, mouse3D ) ) return eGXDraggableAreaResizeMode::WidthLockRight;
	if ( GXIsOverlapedAABBVec3 ( borderBottom, mouse3D ) ) return eGXDraggableAreaResizeMode::HeightLockTop;

	return eGXDraggableAreaResizeMode::HeightLockBottom;
}

GXVoid GXUIDragableArea::UpdateAreas ()
{
	GXSetAABBEmpty ( cornerTopLeft );
	GXSetAABBEmpty ( cornerTopRight );
	GXSetAABBEmpty ( cornerBottomLeft );
	GXSetAABBEmpty ( cornerBottomRight );

	GXAddVertexToAABB ( cornerBottomLeft,  boundsLocal.min );
	GXAddVertexToAABB ( cornerBottomLeft,  boundsLocal.min.x + borderThickness,  boundsLocal.min.y + borderThickness,  boundsLocal.max.z );

	GXAddVertexToAABB ( cornerTopRight,  boundsLocal.max );
	GXAddVertexToAABB ( cornerTopRight,  boundsLocal.max.x - borderThickness,  boundsLocal.max.y - borderThickness,  boundsLocal.min.z );

	GXAddVertexToAABB ( cornerTopLeft,  boundsLocal.min.x,  boundsLocal.max.y - borderThickness,  boundsLocal.min.z );
	GXAddVertexToAABB ( cornerTopLeft,  boundsLocal.min.x + borderThickness,  boundsLocal.max.y,  boundsLocal.max.z );

	GXAddVertexToAABB ( cornerBottomRight,  boundsLocal.max.x - borderThickness,  boundsLocal.min.y,  boundsLocal.min.z );
	GXAddVertexToAABB ( cornerBottomRight,  boundsLocal.max.x,  boundsLocal.min.y + borderThickness,  boundsLocal.max.z );

	GXSetAABBEmpty ( borderBottom );
	GXSetAABBEmpty ( borderTop );
	GXSetAABBEmpty ( borderLeft );
	GXSetAABBEmpty ( borderRight );

	GXAddVertexToAABB ( borderLeft,  boundsLocal.min.x,  boundsLocal.min.y + borderThickness,  boundsLocal.min.z );
	GXAddVertexToAABB ( borderLeft,  boundsLocal.min.x + borderThickness,  boundsLocal.max.y - borderThickness,  boundsLocal.max.z );

	GXAddVertexToAABB ( borderTop,  boundsLocal.min.x + borderThickness,  boundsLocal.max.y - borderThickness,  boundsLocal.min.z );
	GXAddVertexToAABB ( borderTop,  boundsLocal.max.x - borderThickness,  boundsLocal.max.y,  boundsLocal.max.z );

	GXAddVertexToAABB ( borderRight,  boundsLocal.max.x - borderThickness,  boundsLocal.min.y + borderThickness,  boundsLocal.min.z );
	GXAddVertexToAABB ( borderRight,  boundsLocal.max.x,  boundsLocal.max.y - borderThickness,  boundsLocal.max.z );

	GXAddVertexToAABB ( borderBottom,  boundsLocal.min.x + borderThickness,  boundsLocal.min.y,  boundsLocal.min.z );
	GXAddVertexToAABB ( borderBottom,  boundsLocal.max.x - borderThickness,  boundsLocal.min.y + borderThickness,  boundsLocal.max.z );

	GXSetAABBEmpty ( headerArea );

	GXAddVertexToAABB ( headerArea,  boundsLocal.min.x + borderThickness,  boundsLocal.max.y - borderThickness - headerHeight,  boundsLocal.min.z );
	GXAddVertexToAABB ( headerArea,  boundsLocal.max.x - borderThickness,  boundsLocal.max.y - borderThickness,  boundsLocal.max.z );

	GXSetAABBEmpty ( clientArea );
	GXAddVertexToAABB ( clientArea,  boundsLocal.min.x + borderThickness,  boundsLocal.min.y + borderThickness,  boundsLocal.min.z );
	GXAddVertexToAABB ( clientArea,  boundsLocal.max.x - borderThickness,  boundsLocal.max.y - borderThickness - headerHeight,  boundsLocal.max.z );
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
