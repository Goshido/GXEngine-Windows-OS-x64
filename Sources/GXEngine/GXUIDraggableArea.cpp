//version 1.0

#include <GXEngine/GXUIDraggableArea.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>


#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_NONE								0
#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_DRAGGING							1
#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_LOCK_LEFT					2
#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_LOCK_RIGHT					3
#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_HEIGHT_LOCK_BOTTOM					4
#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_HEIGHT_LOCK_TOP					5
#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_TOP_LEFT		6
#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_TOP_RIGHT	7
#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_BOTTOM_LEFT	8
#define GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_BOTTOM_RIGHT	9

#define GX_UI_DRAGGABLE_AREA_DEFAULT_BORDER_THICKNESS						0.2f	// 2 mm
#define GX_UI_DRAGGABLE_AREA_DEFAULT_HEADER_HEIGHT							0.9f	// 9 mm

GXUIDragableArea::GXUIDragableArea ( GXWidget* parent ):
GXWidget ( parent )
{
	headerHeight = GX_UI_DRAGGABLE_AREA_DEFAULT_HEADER_HEIGHT * gx_ui_Scale;
	borderThickness = GX_UI_DRAGGABLE_AREA_DEFAULT_BORDER_THICKNESS * gx_ui_Scale;
	isDraggable = GX_TRUE;
	memset ( &lastMousePosition, 0, sizeof ( GXVec2 ) );
	OnResize = 0;
	handler = 0;
	resizeMode = GX_UI_DRAGGABLE_AREA_RESIZE_MODE_NONE;

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
				
			if ( resizeMode != GX_UI_DRAGGABLE_AREA_RESIZE_MODE_NONE && resizeMode != GX_UI_DRAGGABLE_AREA_RESIZE_MODE_NONE )
				gx_ui_TouchSurface->LockCursor ( this );
		}
		break;

		case GX_MSG_LMBUP:
		{
			resizeMode = GX_UI_DRAGGABLE_AREA_RESIZE_MODE_NONE;

			if ( gx_ui_TouchSurface->GetLockedCursorWidget () == this )
				gx_ui_TouchSurface->ReleaseCursor ();
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
				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_DRAGGING:
				{
					GXVec2 delta;
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					OnMessage ( GX_MSG_DRAG, &delta );

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );
				}
				break;

				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_LOCK_LEFT:
				{
					GXAABB bounds = boundsLocal;
					bounds.max.x += pos->x - lastMousePosition.x;
					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );

					if ( renderer )
						renderer->OnUpdate ();

					if ( OnResize )
						OnResize ( handler, this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_LOCK_RIGHT:
				{
					GXAABB bounds = boundsLocal;
					bounds.min.x += pos->x - lastMousePosition.x;
					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_HEIGHT_LOCK_BOTTOM:
				{
					GXAABB bounds = boundsLocal;
					bounds.max.y += pos->y - lastMousePosition.y;
					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );

					if ( renderer )
						renderer->OnUpdate ();

					if ( OnResize )
						OnResize ( handler, this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_HEIGHT_LOCK_TOP:
				{
					GXAABB bounds = boundsLocal;
					bounds.min.y += pos->y - lastMousePosition.y;
					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_BOTTOM_LEFT:
				{
					GXVec2 delta;
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.max.x += delta.x;
					bounds.max.y += delta.y;

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );

					if ( renderer )
						renderer->OnUpdate ();

					if ( OnResize )
						OnResize ( handler, this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_BOTTOM_RIGHT:
				{
					GXVec2 delta;
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.min.x += delta.x;
					bounds.max.y += delta.y;

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_TOP_LEFT:
				{
					GXVec2 delta;
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.max.x += delta.x;
					bounds.min.y += delta.y;

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_TOP_RIGHT:
				{
					GXVec2 delta;
					GXSubVec2Vec2 ( delta, *pos, lastMousePosition );

					GXAABB bounds = boundsLocal;
					bounds.min.x += delta.x;
					bounds.min.y += delta.y;

					UpdateBoundsWorld ( bounds );
					UpdateAreas ();

					memcpy ( &lastMousePosition, pos, sizeof ( GXVec2 ) );

					GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
					OnMessage ( GX_MSG_DRAG, &dragDelta );

					if ( OnResize )
						OnResize ( handler, this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
				}
				break;

				case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_NONE:
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
			UpdateBoundsWorld ( *bounds );
			UpdateAreas ();

			GXVec2 dragDelta = GXCreateVec2 ( 0.0f, 0.0f );
			OnMessage ( GX_MSG_DRAG, &dragDelta );

			if ( OnResize )
				OnResize ( handler, this, GXGetAABBWidth ( boundsLocal ), GXGetAABBHeight ( boundsLocal ) );
		}
		break;
		
		case GX_MSG_MOVABLE_AREA_SET_HEADER_HEIGHT:
		{
			const GXFloat* height = (const GXFloat*)data;
			headerHeight = *height;
			UpdateAreas ();
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOVABLE_AREA_SET_BORDER_THICKNESS:
		{
			const GXFloat* thickness = (const GXFloat*)data;
			borderThickness = *thickness;
			UpdateAreas ();
			if ( renderer )
				renderer->OnUpdate ();
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

GXVoid GXUIDragableArea::SetHeaderHeight ( GXFloat height )
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_MOVABLE_AREA_SET_HEADER_HEIGHT, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetHeaderHeight ()
{
	return headerHeight;
}

GXVoid GXUIDragableArea::SetBorderThickness ( GXFloat thickness )
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_MOVABLE_AREA_SET_BORDER_THICKNESS, &thickness, sizeof ( GXFloat ) );
}

GXFloat GXUIDragableArea::GetBorderThickness ()
{
	return borderThickness;
}

GXVoid GXUIDragableArea::SetOnResizeCallback ( GXVoid* handler, PFNGXUIDRAGABLEAREAONRESIZEPROC callback )
{
	this->handler = handler;
	OnResize = callback;
}

GXUByte GXUIDragableArea::GetResizeMode ( const GXVec2 &mousePosition )
{
	GXVec3 mouse3D = GXCreateVec3 ( mousePosition.x, mousePosition.y, 0.0f );

	if ( GXIsOverlapedAABBVec3 ( headerArea, mouse3D ) ) return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_DRAGGING;
	if ( GXIsOverlapedAABBVec3 ( clientArea, mouse3D ) ) return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_NONE;

	if ( GXIsOverlapedAABBVec3 ( cornerBottomRight, mouse3D ) ) return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_TOP_LEFT;
	if ( GXIsOverlapedAABBVec3 ( cornerTopLeft, mouse3D ) ) return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_BOTTOM_RIGHT;
	if ( GXIsOverlapedAABBVec3 ( cornerTopRight, mouse3D ) ) return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_BOTTOM_LEFT;
	if ( GXIsOverlapedAABBVec3 ( cornerBottomLeft, mouse3D ) ) return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_TOP_RIGHT;

	if ( GXIsOverlapedAABBVec3 ( borderRight, mouse3D ) ) return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_LOCK_LEFT;
	if ( GXIsOverlapedAABBVec3 ( borderLeft, mouse3D ) ) return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_LOCK_RIGHT;
	if ( GXIsOverlapedAABBVec3 ( borderBottom, mouse3D ) ) return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_HEIGHT_LOCK_TOP;

	return GX_UI_DRAGGABLE_AREA_RESIZE_MODE_HEIGHT_LOCK_BOTTOM;
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
	const HCURSOR* cursor = 0;

	switch ( GetResizeMode ( mousePosition ) )
	{
		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_LOCK_RIGHT:
		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_LOCK_LEFT:
			cursor = &horizontalArrow;
		break;

		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_HEIGHT_LOCK_TOP:
		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_HEIGHT_LOCK_BOTTOM:
			cursor = &verticalArrow;
		break;

		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_TOP_RIGHT:
		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_BOTTOM_LEFT:
			cursor = &northeastSouthwestArrow;
		break;

		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_TOP_LEFT:
		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_WIDTH_AND_HEIGHT_LOCK_BOTTOM_RIGHT:
			cursor = &northwestSoutheastArrow;
		break;

		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_DRAGGING:
			cursor = &crossArrow;
		break;

		case GX_UI_DRAGGABLE_AREA_RESIZE_MODE_NONE:
		default:
			cursor = &standartArrow;
		break;
	}

	if ( cursor == currentCursor ) return;

	currentCursor = cursor;
	SetCursor ( *currentCursor );
}
