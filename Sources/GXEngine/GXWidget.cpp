// version 1.3

#include <GXEngine/GXWidget.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXLogger.h>


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

	defaultBoundsLocal.AddVertex ( -1.0f, -1.0f, -1.0f );
	defaultBoundsLocal.AddVertex ( 1.0f, 1.0f, 1.0f );
	UpdateBoundsWorld ( defaultBoundsLocal );

	isRegistered = isNeedRegister;
	if ( !isRegistered ) return;

	gx_ui_Mutex->Lock ();

	if ( parent )
		parent->AddChild ( this );
	else
		GXTouchSurface::GetInstance ().RegisterWidget ( this );

	gx_ui_Mutex->Release ();
}

GXWidget::~GXWidget ()
{
	if ( !isRegistered ) return;

	gx_ui_Mutex->Lock ();

	if ( parent )
		parent->RemoveChild ( this );
	else
		GXTouchSurface::GetInstance ().UnRegisterWidget ( this );

	gx_ui_Mutex->Release ();
}

GXVoid GXWidget::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_LMBDOWN:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( *pos );

			if ( dest != this )
				dest->OnMessage ( GX_MSG_LMBDOWN, data );
		}
		break;

		case GX_MSG_LMBUP:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( *pos );

			if ( dest != this )
				dest->OnMessage ( GX_MSG_LMBUP, data );
		}
		break;

		case GX_MSG_MMBDOWN:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( *pos );

			if ( dest != this )
				dest->OnMessage ( GX_MSG_MMBDOWN, data );
		}
		break;

		case GX_MSG_MMBUP:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( *pos );

			if ( dest != this )
				dest->OnMessage ( GX_MSG_MMBUP, data );
		}
		break;

		case GX_MSG_RMBDOWN:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( *pos );

			if ( dest != this )
				dest->OnMessage ( GX_MSG_RMBDOWN, data );
		}
		break;

		case GX_MSG_RMBUP:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( *pos );

			if ( dest != this )
				dest->OnMessage ( GX_MSG_RMBUP, data );
		}
		break;

		case GX_MSG_SCROLL:
		{
			const GXVec3* scrollData = (const GXVec3*)data;
			GXVec2 mousePosition ( scrollData->GetX (), scrollData->GetY () );
			GXWidget* dest = FindWidget ( mousePosition );

			if ( dest != this )
				dest->OnMessage ( GX_MSG_SCROLL, data );
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( *pos );

			if ( dest != this )
				dest->OnMessage ( GX_MSG_MOUSE_MOVE, data );
		}
		break;

		case GX_MSG_MOUSE_OVER:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( *pos );

			if ( dest != this )
				dest->OnMessage ( GX_MSG_MOUSE_OVER, data );
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			const GXVec2* pos = (const GXVec2*)data;
			GXWidget* dest = FindWidget ( *pos );

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
				GXTouchSurface::GetInstance ().MoveWidgetToForeground ( this );
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
	newBounds.AddVertex ( bottomLeftX, bottomLeftY, -1.0f );
	newBounds.AddVertex ( bottomLeftX + width, bottomLeftY + height, 1.0f );

	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_RESIZE, &newBounds, sizeof ( GXAABB ) );
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
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SHOW, nullptr, 0 );
}

GXVoid GXWidget::Refresh ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_REDRAW, nullptr, 0 );
}

GXVoid GXWidget::Hide ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_HIDE, nullptr, 0 );
}

GXVoid GXWidget::ToForeground ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_FOREGROUND, nullptr, 0 );
}

GXBool GXWidget::IsVisible () const
{
	return isVisible;
}

GXBool GXWidget::IsDraggable () const
{
	return isDraggable;
}

GXVoid GXWidget::SetRenderer ( GXWidgetRenderer* rendererObject )
{
	renderer = rendererObject;
}

GXWidgetRenderer* GXWidget::GetRenderer () const
{
	return renderer;
}

GXWidget* GXWidget::FindWidget ( const GXVec2 &position )
{
	gx_ui_Mutex->Lock ();

	GXWidgetIterator iterator;
	GXWidget* p = iterator.Init ( childs );

	while ( p )
	{
		if ( p->IsVisible () && p->GetBoundsWorld ().IsOverlaped ( position.GetX (), position.GetY (), 0.0f ) )
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

GXVoid GXWidget::UpdateBoundsWorld ( const GXAABB &newBoundsLocal )
{
	memcpy ( &boundsLocal, &newBoundsLocal, sizeof ( GXAABB ) );

	if ( parent )
	{
		const GXAABB& originWorld = parent->GetBoundsWorld ();
		boundsWorld.Empty ();
		boundsWorld.AddVertex ( originWorld.min.GetX () + boundsLocal.min.GetX (), originWorld.min.GetY () + boundsLocal.min.GetY (), originWorld.min.GetZ () );
		boundsWorld.AddVertex ( originWorld.min.GetX () + boundsLocal.max.GetX (), originWorld.min.GetY () + boundsLocal.max.GetY (), originWorld.max.GetZ () );
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

GXWidget* GXWidgetIterator::Init ( GXWidget* startWidget )
{
	widget = startWidget;
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
	if ( !widget ) return nullptr;

	widget = widget->parent;
	return widget;
}

GXWidget* GXWidgetIterator::GetChilds ()
{
	if ( !widget ) return nullptr;

	widget = widget->childs;
	return widget;
}

//--------------------------------------------------------------------------

GXWidgetRenderer::GXWidgetRenderer ( GXWidget* widget )
{
	this->widget = widget;
	oldBounds.Empty ();
}

GXWidgetRenderer::~GXWidgetRenderer ()
{
	// NOTHING
}

GXVoid GXWidgetRenderer::OnUpdate ()
{
	if ( IsResized () )
	{
		const GXAABB& boundsWorld = widget->GetBoundsWorld ();

		GXUShort width = (GXUShort)boundsWorld.GetWidth ();
		GXUShort height = (GXUShort)boundsWorld.GetHeight ();

		GXVec3 center;
		boundsWorld.GetCenter ( center );
		GXRenderer& renderer = GXRenderer::GetInstance ();
		center.data[ 0 ] -= 0.5f * renderer.GetWidth ();
		center.data[ 1 ] -= 0.5f * renderer.GetHeight ();

		OnResized ( center.GetX (), center.GetY (), width, height );
		OnRefresh ();
	}
	else if ( IsMoved () )
	{
		GXVec3 center;
		widget->GetBoundsWorld ().GetCenter ( center );
		GXRenderer& renderer = GXRenderer::GetInstance ();
		center.data[ 0 ] -= 0.5f * renderer.GetWidth ();
		center.data[ 1 ] -= 0.5f * renderer.GetHeight ();

		OnMoved ( center.GetX (), center.GetY () );
	}
	else
	{
		OnRefresh ();
	}
}

GXVoid GXWidgetRenderer::OnRefresh ()
{
	// NOTHING
}

GXVoid GXWidgetRenderer::OnDraw ()
{
	// NOTHING
}

GXVoid GXWidgetRenderer::OnResized ( GXFloat /*x*/, GXFloat /*y*/, GXUShort /*width*/, GXUShort /*height*/ )
{
	// NOTHING
}

GXVoid GXWidgetRenderer::OnMoved ( GXFloat /*x*/, GXFloat /*y*/ )
{
	// NOTHING
}

GXBool GXWidgetRenderer::IsResized ()
{
	if ( !widget ) return GX_FALSE;

	const GXAABB& bounds = widget->GetBoundsWorld ();
	if ( fabs ( oldBounds.GetWidth () - bounds.GetWidth () ) < GX_WIDGET_RENDERER_RESIZE_EPSILON )
	{
		if ( fabs ( oldBounds.GetHeight () - bounds.GetHeight () ) >= GX_WIDGET_RENDERER_RESIZE_EPSILON )
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

	if ( fabs ( bounds.min.GetX () - oldBounds.min.GetX () ) < GX_WIDGET_RENDERER_RESIZE_EPSILON )
	{
		if ( fabs ( bounds.min.GetY () - oldBounds.min.GetY () ) >= GX_WIDGET_RENDERER_RESIZE_EPSILON )
		{
			oldBounds = bounds;
			return GX_TRUE;
		}
		else
		{
			return GX_FALSE;
		}
	}
	else
	{
		oldBounds = bounds;
		return GX_TRUE;
	}
}
