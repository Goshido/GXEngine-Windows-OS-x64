//version 1.2

#include <GXEngine/GXWidget.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>


#define GX_WIDGET_RENDERER_RESIZE_EPSILON 0.25f


GXWidget::GXWidget ( GXWidget* parent, GXBool isNeedRegister )
{
	next = nullptr;
	prev = nullptr;

	this->parent = parent;
	childs = nullptr;

	isVisible = GX_TRUE;
	isDraggable = GX_FALSE;
	renderer = nullptr;

	GXAABB defaultBoundsLocal;
	GXSetAABBEmpty ( defaultBoundsLocal );
	GXAddVertexToAABB ( defaultBoundsLocal, -1.0f, -1.0f, -1.0f );
	GXAddVertexToAABB ( defaultBoundsLocal, 1.0f, 1.0f, 1.0f );
	UpdateBoundsWorld ( defaultBoundsLocal );

	isRegistered = isNeedRegister;
	if ( !isRegistered ) return;

	gx_ui_Mutex->Lock ();

	if ( parent )
		parent->AddChild ( this );
	else
		gx_ui_TouchSurface->RegisterWidget ( this );

	gx_ui_Mutex->Release ();
}

GXWidget::~GXWidget ()
{
	if ( !isRegistered ) return;

	gx_ui_Mutex->Lock ();

	if ( parent )
		parent->RemoveChild ( this );
	else
		gx_ui_TouchSurface->UnRegisterWidget ( this );

	gx_ui_Mutex->Release ();
}

GXVoid GXWidget::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_LMBDOWN:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( pos->x, pos->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_LMBDOWN, data );
		}
		break;

		case GX_MSG_LMBUP:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( pos->x, pos->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_LMBUP, data );
		}
		break;

		case GX_MSG_MMBDOWN:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( pos->x, pos->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_MMBDOWN, data );
		}
		break;

		case GX_MSG_MMBUP:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( pos->x, pos->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_MMBUP, data );
		}
		break;

		case GX_MSG_RMBDOWN:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( pos->x, pos->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_RMBDOWN, data );
		}
		break;

		case GX_MSG_RMBUP:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( pos->x, pos->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_RMBUP, data );
		}
		break;

		case GX_MSG_SCROLL:
		{
			const GXVec3* scrollData = (const GXVec3*)data;
			GXVec2 mousePosition = GXCreateVec2 ( scrollData->x, scrollData->y );
			GXWidget* dest = FindWidget ( scrollData->x, scrollData->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_SCROLL, data );
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( pos->x, pos->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_MOUSE_MOVE, data );
		}
		break;

		case GX_MSG_MOUSE_OVER:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( pos->x, pos->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_MOUSE_OVER, data );
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( pos->x, pos->y );
			if ( dest != this )
				dest->OnMessage ( GX_MSG_MOUSE_LEAVE, data );
		}
		break;

		case GX_MSG_RESIZE:
		{
			const GXAABB* newBoundsLocal = (const GXAABB*)data;
			UpdateBoundsWorld ( *newBoundsLocal );
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_SHOW:
			isVisible = GX_TRUE;
		break;

		case GX_MSG_HIDE:
			isVisible = GX_FALSE;
		break;

		case GX_MSG_FOREGROUND:
		{
			if ( parent )
			{
				GXWidgetIterator iterator;
				GXWidget* p = iterator.Init ( this );

				while ( p->parent )
					p = iterator.GetParent ();

				p->OnMessage ( GX_MSG_FOREGROUND, data );
				
			}
			else
			{
				gx_ui_TouchSurface->MoveWidgetToForeground ( this );
			}
		}
		break;

		case GX_MSG_REDRAW:
			if ( renderer )
				renderer->OnUpdate ();
		break;
	}
}

GXVoid GXWidget::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
	GXAABB newBounds;
	GXAddVertexToAABB ( newBounds, bottomLeftX, bottomLeftY, -10.0f );
	GXAddVertexToAABB ( newBounds, bottomLeftX + width, bottomLeftY + height, 10.0f );

	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_RESIZE, &newBounds, sizeof ( GXAABB ) );
}

const GXAABB& GXWidget::GetBoundsWorld () const
{
	return boundsWorld;
}

const GXAABB& GXWidget::GetBoundsLocal () const
{
	return boundsLocal;
}

GXVoid GXWidget::Show ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_SHOW, nullptr, 0 );
}

GXVoid GXWidget::Hide ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_HIDE, nullptr, 0 );
}

GXVoid GXWidget::ToForeground ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_FOREGROUND, nullptr, 0 );
}

GXBool GXWidget::IsVisible () const
{
	return isVisible;
}

GXBool GXWidget::IsDraggable () const
{
	return isDraggable;
}

GXVoid GXWidget::SetRenderer ( GXWidgetRenderer* renderer )
{
	this->renderer = renderer;
}

GXWidgetRenderer* GXWidget::GetRenderer () const
{
	return renderer;
}

GXWidget* GXWidget::FindWidget ( GXFloat x, GXFloat y )
{
	gx_ui_Mutex->Lock ();

	GXWidgetIterator iterator;
	GXWidget* p = iterator.Init ( childs );
	while ( p )
	{
		if ( p->IsVisible () && GXIsOverlapedAABBVec3 ( p->GetBoundsWorld (), x, y, 0.0f ) )
		{
			gx_ui_Mutex->Release ();
			return p;
		}

		p = iterator.GetNext ();
	}

	gx_ui_Mutex->Release ();

	return this;
}

GXVoid GXWidget::OnDraw ()
{
	if ( !isVisible ) return;

	if ( renderer )
		renderer->OnDraw ();

	GXWidgetIterator iterator;
	GXWidget* p = iterator.Init ( childs );
	while ( p )
	{
		p->OnDraw ();
		p = iterator.GetNext ();
	}
}

GXVoid GXWidget::UpdateBoundsWorld ( const GXAABB &boundsLocal )
{
	memcpy ( &this->boundsLocal, &boundsLocal, sizeof ( GXAABB ) );

	if ( parent )
	{
		const GXAABB& originWorld = parent->GetBoundsWorld ();
		GXSetAABBEmpty ( boundsWorld );
		GXAddVertexToAABB ( boundsWorld, originWorld.min.x + boundsLocal.min.x, originWorld.min.y + boundsLocal.min.y, originWorld.min.z );
		GXAddVertexToAABB ( boundsWorld, originWorld.min.x + boundsLocal.max.x, originWorld.min.y + boundsLocal.max.y, originWorld.max.z );
	}
	else
	{
		memcpy ( &boundsWorld, &boundsLocal, sizeof ( GXAABB ) );
	}
}

GXVoid GXWidget::AddChild ( GXWidget* child )
{
	if ( DoesChildExist ( child ) ) return;

	child->next = childs;
	child->prev = nullptr;

	if ( childs )
		childs->prev = child;

	childs = child;
}

GXVoid GXWidget::RemoveChild ( GXWidget* child )
{
	if ( !DoesChildExist ( child ) ) return;

	if ( child->next ) child->next->prev = child->prev;
	if ( child->prev )
		child->prev->next = child->next;
	else
		childs = child->next;
}

GXBool GXWidget::DoesChildExist ( GXWidget* child ) const
{
	for ( GXWidget* p = childs; p; p = p->next )
	{
		if ( p == child )
			return GX_TRUE;
	}

	return GX_FALSE;
}

//--------------------------------------------------------------------------

GXWidgetIterator::GXWidgetIterator ()
{
	widget = nullptr;
}

GXWidgetIterator::~GXWidgetIterator ()
{
	// NOTHING
}

GXWidget* GXWidgetIterator::Init ( GXWidget* widget )
{
	this->widget = widget;
	return widget;
}

GXWidget* GXWidgetIterator::GetNext ()
{
	if ( !widget ) return nullptr;

	widget = widget->next;
	return widget;
}

GXWidget* GXWidgetIterator::GetPrevious ()
{
	if ( !widget ) return nullptr;

	widget = widget->prev;
	return widget;
}

GXWidget* GXWidgetIterator::GetParent ()
{
	if ( !widget ) return 0;

	widget = widget->parent;
	return widget;
}

GXWidget* GXWidgetIterator::GetChilds ()
{
	if ( !widget ) return 0;

	widget = widget->childs;
	return widget;
}

//--------------------------------------------------------------------------

GXWidgetRenderer::GXWidgetRenderer ( GXWidget* widget )
{
	this->widget = widget;
	GXSetAABBEmpty ( oldBounds );
}

GXWidgetRenderer::~GXWidgetRenderer ()
{
	//NOTHING
}

GXVoid GXWidgetRenderer::OnUpdate ()
{
	if ( IsResized () )
	{
		const GXAABB& boundsWorld = widget->GetBoundsWorld ();

		GXUShort width = (GXUShort)GXGetAABBWidth ( boundsWorld );
		GXUShort height = (GXUShort)GXGetAABBHeight ( boundsWorld );

		GXVec3 center;
		GXGetAABBCenter ( center, boundsWorld );
		GXRenderer* renderer = GXRenderer::GetInstance ();
		center.x -= 0.5f * renderer->GetWidth ();
		center.y -= 0.5f * renderer->GetHeight ();

		OnResized ( center.x, center.y, width, height );
		OnRefresh ();
	}
	else if ( IsMoved () )
	{
		GXVec3 center;
		GXGetAABBCenter ( center, widget->GetBoundsWorld () );
		GXRenderer* renderer = GXRenderer::GetInstance ();
		center.x -= 0.5f * renderer->GetWidth ();
		center.y -= 0.5f * renderer->GetHeight ();
		OnMoved ( center.x, center.y );
	}
	else
	{
		OnRefresh ();
	}
}

GXVoid GXWidgetRenderer::OnRefresh ()
{
	//NOTHING
}

GXVoid GXWidgetRenderer::OnDraw ()
{
	//NOTHING
}

GXVoid GXWidgetRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	//NOTHING
}

GXVoid GXWidgetRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	//NOTHING
}

GXBool GXWidgetRenderer::IsResized ()
{
	if ( !widget ) return GX_FALSE;

	const GXAABB& bounds = widget->GetBoundsWorld ();
	if ( fabs ( GXGetAABBWidth ( oldBounds ) - GXGetAABBWidth ( bounds ) ) < GX_WIDGET_RENDERER_RESIZE_EPSILON )
	{
		if ( fabs ( GXGetAABBHeight ( oldBounds ) - GXGetAABBHeight ( bounds ) ) >= GX_WIDGET_RENDERER_RESIZE_EPSILON )
		{
			memcpy ( &oldBounds, &bounds, sizeof ( GXAABB ) );
			return GX_TRUE;
		}
		else
		{
			return GX_FALSE;
		}
	}
	else
	{
		memcpy ( &oldBounds, &bounds, sizeof ( GXAABB ) );
		return GX_TRUE;
	}
}

GXBool GXWidgetRenderer::IsMoved ()
{
	if ( !widget ) return GX_FALSE;

	const GXAABB& bounds = widget->GetBoundsWorld ();

	if ( fabs ( bounds.min.x - oldBounds.min.x ) < GX_WIDGET_RENDERER_RESIZE_EPSILON )
	{
		if ( fabs ( bounds.min.y - oldBounds.min.y ) >= GX_WIDGET_RENDERER_RESIZE_EPSILON )
		{
			memcpy ( &oldBounds, &bounds, sizeof ( GXAABB ) );
			return GX_TRUE;
		}
		else
		{
			return GX_FALSE;
		}
	}
	else
	{
		memcpy ( &oldBounds, &bounds, sizeof ( GXAABB ) );
		return GX_TRUE;
	}
}
