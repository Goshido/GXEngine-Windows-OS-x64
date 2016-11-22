//version 1.0

#include <GXEngine/GXUIMenu.h>
#include <GXEngine/GXUIMessage.h>


struct GXUIMenuItem
{
	GXAABB		bounds;
	GXBool		isPressed;
	GXBool		isHighlighted;
};

struct GXUIMenuResizeItem
{
	GXUByte		item;
	GXFloat		width;
};

GXUIMenu::GXUIMenu (  GXWidget* parent, GXVoid* menuHandler, PFNGXONSHOWSUBMENUPROC onShowSubmenu ):
GXWidget ( parent ), items ( sizeof ( GXUIMenuItem ) )
{
	this->menuHandler = menuHandler;
	OnShowSubmenu = onShowSubmenu;
}

GXUIMenu::~GXUIMenu ()
{
	//NOTHING
}

GXVoid GXUIMenu::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_RESIZE:
		{
			GXUInt total = items.GetLength ();

			if ( total == 0 )
			{
				GXWidget::OnMessage ( message, data );

				if ( renderer )
					renderer->OnUpdate ();

				return;
			}

			const GXAABB* b = (const GXAABB*)data;

			GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( 0 );
			GXVec2 delta = GXCreateVec2 ( b->min.x - item->bounds.min.x, b->min.y - item->bounds.min.y );

			for ( GXUInt i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				item->bounds.min.x += delta.x;
				item->bounds.min.y += delta.y;

				item->bounds.max.x += delta.x;
				item->bounds.max.y += delta.y;
			}

			GXAABB bounds;
			GXSetAABBEmpty ( bounds );
			GXAddVertexToAABB ( bounds, item->bounds.min.x, item->bounds.min.y, item->bounds.min.z );

			if ( total > 1 )
				item = (GXUIMenuItem*)items.GetValue ( total - 1 );

			GXAddVertexToAABB ( bounds, item->bounds.max.x, item->bounds.max.y, item->bounds.max.z );

			UpdateBoundsWorld ( bounds );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MENU_ADD_ITEM:
		{
			GXUIMenuItem item;
			item.isHighlighted = GX_FALSE;
			item.isPressed = GX_FALSE;
			GXSetAABBEmpty ( item.bounds );

			GXUInt total = items.GetLength ();
			if ( total == 0 )
			{
				GXAddVertexToAABB ( item.bounds, boundsLocal.min.x, boundsLocal.min.y, boundsLocal.min.z );
				GXAddVertexToAABB ( item.bounds, boundsLocal.min.x + gx_ui_Scale * 1.5f, boundsLocal.max.y, boundsLocal.max.z );

				UpdateBoundsWorld ( item.bounds );
			}
			else
			{
				GXUIMenuItem* last = (GXUIMenuItem*)items.GetValue ( total - 1 );
				GXAddVertexToAABB ( item.bounds, last->bounds.max.x, last->bounds.min.y, last->bounds.min.z );
				GXAddVertexToAABB ( item.bounds, last->bounds.max.x + gx_ui_Scale * 1.5f, boundsLocal.max.y, last->bounds.max.z );

				GXUIMenuItem* first = (GXUIMenuItem*)items.GetValue ( 0 );
				GXAABB bounds;
				GXSetAABBEmpty ( bounds );
				GXAddVertexToAABB ( bounds, first->bounds.min.x, first->bounds.min.y, first->bounds.min.z );
				GXAddVertexToAABB ( bounds, item.bounds.max.x, item.bounds.max.y, item.bounds.max.z );

				UpdateBoundsWorld ( bounds );
			}

			items.SetValue ( total, &item );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MENU_RESIZE_ITEM:
		{
			const GXUIMenuResizeItem* ri = (const GXUIMenuResizeItem*)data;

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total < ri->item + 1 ) return;

			GXUIMenuItem* p = (GXUIMenuItem*)items.GetValue ( ri->item );
			GXFloat delta = ri->width - ( p->bounds.max.x - p->bounds.min.x );

			p->bounds.max.x += delta;

			for ( GXUByte i = ri->item + 1; i < total; i++ )
			{
				p = (GXUIMenuItem*)items.GetValue ( i );
				p->bounds.min.x += delta;
				p->bounds.max.x += delta;
			}

			p = (GXUIMenuItem*)items.GetValue ( total - 1 );
			boundsLocal.max.x = p->bounds.max.x;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = (const GXVec2*)data;

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			GXUIMenuItem* itemMouseOver = 0;
			GXUByte itemIndex = 0xFF;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				if ( GXIsOverlapedAABBVec3 ( item->bounds, pos->x, pos->y, 0.0f ) )
				{
					itemIndex = i;
					itemMouseOver = item;
					break;
				}
			}

			if ( itemMouseOver->isHighlighted ) return;

			itemMouseOver->isHighlighted = GX_TRUE;
			
			if ( itemIndex != 0 )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( itemIndex - 1 );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( itemIndex < total - 1 )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( itemIndex + 1 );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_REDRAW:
			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_LMBDOWN:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				if ( item->isHighlighted )
				{
					item->isPressed = GX_TRUE;
					OnShowSubmenu ( menuHandler, i );
					break;
				}
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LMBUP:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				if ( item->isHighlighted )
				{
					item->isPressed = GX_FALSE;
					break;
				}
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		default:
		break;
	}
}

GXVoid GXUIMenu::AddItem ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_MENU_ADD_ITEM, 0, 0 );
}

GXVoid GXUIMenu::ResizeItem ( GXUByte item, GXFloat width )
{
	GXUIMenuResizeItem ri;
	ri.item = item;
	ri.width = width;

	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_MENU_RESIZE_ITEM, &ri, sizeof ( GXUIMenuResizeItem ) );
}

GXUByte GXUIMenu::GetTotalItems ()
{
	return (GXUByte)items.GetLength ();
}

const GXAABB& GXUIMenu::GetItemBounds ( GXUByte item )
{
	GXUIMenuItem* p = (GXUIMenuItem*)items.GetValue ( item );
	return p->bounds;
}

GXVoid GXUIMenu::Redraw ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_REDRAW, 0, 0 );
}

GXBool GXUIMenu::IsItemPressed ( GXUByte item )
{
	GXUIMenuItem* p = (GXUIMenuItem*)items.GetValue ( item );
	return p->isPressed;
}

GXBool GXUIMenu::IsItemHighlighted ( GXUByte item )
{
	GXUIMenuItem* p = (GXUIMenuItem*)items.GetValue ( item );
	return p->isHighlighted;
}

//--------------------------------------------------------------------------

struct GXUISubmenuItem
{
	GXAABB						bounds;
	PFNGXONMENUITEMPROC			OnMouseButton;
	GXBool						isPressed;
	GXBool						isHighlighted;
};

GXUISubmenu::GXUISubmenu ( GXWidget* parent ):
GXWidget ( parent ), items ( sizeof ( GXUISubmenuItem ) )
{
	itemHeight = 0.5f * gx_ui_Scale;
}

GXUISubmenu::~GXUISubmenu ()
{
	//NOTHING
}

GXVoid GXUISubmenu::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_RESIZE:
		{
			GXUByte total = items.GetLength ();
			if ( total == 0 )
			{
				const GXAABB* newBounds = (const GXAABB*)data;
				itemHeight = newBounds->max.y - newBounds->min.y;

				GXWidget::OnMessage ( message, data );

				if ( renderer )
					renderer->OnUpdate ();

				return;
			}

			const GXAABB* b = (const GXAABB*)data;

			GXUISubmenuItem* bottomItem = (GXUISubmenuItem*)items.GetValue ( total - 1 );
			GXVec2 delta = GXCreateVec2 ( b->min.x - bottomItem->bounds.min.x, b->min.y - bottomItem->bounds.min.y );

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( i );
				item->bounds.min.x += delta.x;
				item->bounds.min.y += delta.y;

				item->bounds.max.x += delta.x;
				item->bounds.max.y += delta.y;
			}

			GXAABB bounds;
			GXSetAABBEmpty ( bounds );
			GXAddVertexToAABB ( bounds, bottomItem->bounds.min.x, bottomItem->bounds.min.y, bottomItem->bounds.min.z );

			if ( total > 1 )
			{
				GXUISubmenuItem* topItem = (GXUISubmenuItem*)items.GetValue ( 0 );
				GXAddVertexToAABB ( bounds, topItem->bounds.max.x, topItem->bounds.max.y, topItem->bounds.max.z );
			}
			else
				GXAddVertexToAABB ( bounds, bottomItem->bounds.max.x, bottomItem->bounds.max.y, bottomItem->bounds.max.z );

			UpdateBoundsWorld ( bounds );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_SUBMENU_ADD_ITEM:
		{
			GXUISubmenuItem item;
			item.isHighlighted = item.isPressed = GX_FALSE;
			item.OnMouseButton = *( (PFNGXONMENUITEMPROC*)data );

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 )
			{
				GXAddVertexToAABB ( item.bounds, boundsLocal.min.x, boundsLocal.min.y, boundsLocal.min.z );
				GXAddVertexToAABB ( item.bounds, boundsLocal.max.x, boundsLocal.max.y, boundsLocal.max.z );
			}
			else
			{
				GXUISubmenuItem* i = (GXUISubmenuItem*)items.GetValue ( total - 1 );
				GXAddVertexToAABB ( item.bounds, i->bounds.min.x, i->bounds.min.y - itemHeight, i->bounds.min.z );
				GXAddVertexToAABB ( item.bounds, i->bounds.max.x, i->bounds.min.y, i->bounds.max.z );
				
				GXAABB bounds = boundsLocal;
				bounds.min.x = item.bounds.min.x;
				bounds.min.y = item.bounds.min.y;

				UpdateBoundsWorld ( bounds );
			}

			items.SetValue ( total, &item );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_SUBMENU_SET_HEIGHT:
		{
			const GXFloat* height = (const GXFloat*)data;
			itemHeight = *height;

			GXAABB bounds = boundsLocal;

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 )
			{
				bounds.max.y = bounds.min.y + itemHeight;
				UpdateBoundsWorld ( bounds );
				return;
			}

			GXFloat y = bounds.min.y;
			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( total - i - 1 );
				item->bounds.min.y = y;
				y += itemHeight;
				item->bounds.max.y = y;
			}

			GXUISubmenuItem* topItem = (GXUISubmenuItem*)items.GetValue ( 0 );
			bounds.max.y = topItem->bounds.max.y;
			UpdateBoundsWorld ( bounds );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			const GXVec2* pos = (const GXVec2*)data;

			GXUISubmenuItem* itemMouseOver = 0;
			GXUByte itemIndex;

			for ( itemIndex = 0; itemIndex < total; itemIndex++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( itemIndex );
				if ( GXIsOverlapedAABBVec3 ( item->bounds, pos->x, pos->y, 0.0f ) )
				{
					itemMouseOver = item;
					break;
				}
			}

			if ( itemMouseOver->isHighlighted ) return;

			itemMouseOver->isHighlighted = GX_TRUE;
			
			if ( itemIndex != 0 )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( itemIndex - 1 );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( itemIndex < total - 1 )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( itemIndex + 1 );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			Hide ();

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( i );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LMBDOWN:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( i );
				if ( item->isHighlighted )
				{
					item->isPressed = GX_TRUE;
					break;
				}
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LMBUP:
		{
			Hide ();

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( i );
				if ( item->isHighlighted )
				{
					item->isPressed = GX_FALSE;
					if ( item->OnMouseButton )
					{
						const GXVec2* pos = (const GXVec2*)data;
						item->OnMouseButton ( pos->x, pos->y, GX_MOUSE_BUTTON_UP );
					}
					break;
				}
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_REDRAW:
			if ( renderer )
				renderer->OnUpdate ();
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUISubmenu::AddItem ( PFNGXONMENUITEMPROC callback )
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_SUBMENU_ADD_ITEM, &callback, sizeof ( PFNGXONMENUITEMPROC ) );
}

GXVoid GXUISubmenu::SetItemHeight ( GXFloat height )
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_SUBMENU_SET_HEIGHT, &height, sizeof ( GXFloat ) );
}

GXUByte GXUISubmenu::GetTotalItems ()
{
	return (GXUByte)items.GetLength ();
}

GXFloat GXUISubmenu::GetItemHeight ()
{
	return itemHeight;
}

GXVoid GXUISubmenu::Redraw ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_REDRAW, 0, 0 );
}

GXBool GXUISubmenu::IsItemPressed ( GXUByte item )
{
	GXUISubmenuItem* i = (GXUISubmenuItem*)items.GetValue ( item );
	return i->isPressed;
}

GXBool GXUISubmenu::IsItemHighlighted ( GXUByte item )
{
	GXUISubmenuItem* i = (GXUISubmenuItem*)items.GetValue ( item );
	return i->isHighlighted;
}
