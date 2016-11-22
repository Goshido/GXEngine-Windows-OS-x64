//version 1.0

#include <GXEngine/GXUIListBox.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine_Editor_Mobile/EMUIFileListBox.h>
#include <float.h>


#define GX_UI_DEFAULT_ITEM_HEIGHT		0.55f


struct GXUIListBoxRawItem
{
	GXUIListBoxRawItem*			next;
	GXVoid*						data;
};


GXUIListBox::GXUIListBox ( GXWidget* parent, PFNGXUILISTBOXITEMDESTRUCTORPROC itemDestructor ):
GXWidget ( parent )
{
	itemHead = itemTail = 0;
	itemHeight = GX_UI_DEFAULT_ITEM_HEIGHT * gx_ui_Scale;
	totalItems = 0;
	DestroyItem = itemDestructor;
	itemSelectedHandler = 0;
	itemDoubleClickedHandler = 0;
	OnItemSelected = 0;
	OnItemDoubleClicked = 0;

	viewportOffset = 0.0f;
	viewportSize = 1.0f;
}

GXUIListBox::~GXUIListBox ()
{
	while ( itemHead )
	{
		GXUIListBoxItem* p = itemHead;
		itemHead = itemHead->next;
		DestroyItem ( p->data );
		delete p;
	}
}

GXVoid GXUIListBox::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_MOUSE_OVER:
		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = (const GXVec2*)data;

			GXBool isNeedRenderUpdate = ResetHighlight ( *pos );
			GXUIListBoxItem* item = FindItem ( *pos );

			if ( isNeedRenderUpdate && renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			for ( GXUIListBoxItem* p = itemHead; p; p = p->next )
			{
				if ( p->isHighlighted )
				{
					p->isHighlighted = GX_FALSE;

					if ( renderer )
						renderer->OnUpdate ();

					break;
				}
			}
		}
		break;

		case GX_MSG_SCROLL:
		{
			const GXVec3* scroll = (const GXVec3*)data;

			if ( IsAbleToScroll () )
			{
				GXFloat step = 1.0f / (GXFloat)totalItems;
				GXFloat offset = GXClampf ( viewportOffset - scroll->z * step, 0.0f, 1.0f - viewportSize );

				viewportOffset = offset;

				GXVec2 pos = GXCreateVec2 ( scroll->x, scroll->y );
				OnMessage ( GX_MSG_MOUSE_MOVE, &pos );
			}
		}
		break;

		case GX_MSG_LMBDOWN:
		{
			GXUIListBoxItem* item = GetHighlightedItem ();
			GXUIListBoxItem* selected = 0;
			for ( selected = itemHead; selected; selected = selected->next )
			{
				if ( selected->isSelected )
					break;
			}

			if ( selected != item )
			{
				if ( selected ) selected->isSelected = GX_FALSE;
				if ( item ) item->isSelected = GX_TRUE;

				if ( OnItemSelected )
					OnItemSelected ( itemSelectedHandler, this, item->data );

				if ( renderer )
					renderer->OnUpdate ();
			}
		}
		break;

		case GX_MSG_DOUBLE_CLICK:
		{
			GXUIListBoxItem* item = GetHighlightedItem ();

			if ( item )
			{
				if ( OnItemDoubleClicked )
					OnItemDoubleClicked ( itemDoubleClickedHandler, this, item->data );
			}
		}
		break;

		case GX_MSG_LIST_BOX_ADD_ITEM:
		{
			GXVoid** itemData = (GXVoid**)data;

			GXUIListBoxItem* item = new GXUIListBoxItem ();
			item->data = *itemData;
			item->isSelected = item->isHighlighted = GX_FALSE;

			item->next = 0;
			item->prev = itemTail;
			if ( itemTail )
				itemTail->next = item;
			else
				itemHead = item;

			itemTail = item;

			totalItems++;
			viewportSize = GXGetAABBHeight ( boundsLocal ) / GetTotalHeight ();
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LIST_BOX_ADD_ITEMS:
		{
			GXUIListBoxRawItem** list = (GXUIListBoxRawItem**)data;
			for ( GXUIListBoxRawItem* p = *list; p; p = p->next )
			{
				GXUIListBoxItem* item = new GXUIListBoxItem ();
				item->data = p->data;
				item->isSelected = item->isHighlighted = GX_FALSE;

				EMUIFileListBoxItem* itm = (EMUIFileListBoxItem*)p->data;

				item->next = 0;
				item->prev = itemTail;
				if ( itemTail )
					itemTail->next = item;
				else
					itemHead = item;

				itemTail = item;

				totalItems++;
			}

			viewportSize = GXGetAABBHeight ( boundsLocal ) / GetTotalHeight ();
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LIST_BOX_REMOVE_ALL_ITEMS:
		{
			while ( itemHead )
			{
				GXUIListBoxItem* p = itemHead;
				itemHead = itemHead->next;
				DestroyItem ( p->data );
				delete p;
			}

			itemTail = 0;
			totalItems = 0;

			viewportOffset = 0.0f;
			viewportSize = GXGetAABBHeight ( boundsLocal ) / GetTotalHeight ();
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LIST_BOX_REMOVE_ITEM:
		{
			const GXUInt* itemIndex = (const GXUInt*)data;

			GXUIListBoxItem* p = itemHead;
			for ( GXUInt i = 0; i < *itemIndex; i++ )
				p = p->next;

			if ( p->next )
				p->next->prev = p->prev;
			else
				itemTail = p->prev;

			if ( p->prev )
				p->prev->next = p->next;
			else
				itemHead = p->next;

			DestroyItem ( p->data );
			totalItems--;

			delete p;

			viewportSize = GXGetAABBHeight ( boundsLocal ) / GetTotalHeight ();
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LIST_BOX_SET_VIEWPORT_OFFSET:
		{
			const GXFloat* offset = (const GXFloat*)data;
			viewportOffset = *offset;

			viewportSize = GXGetAABBHeight ( boundsLocal ) / GetTotalHeight ();
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LIST_BOX_SET_ITEM_HEIGHT:
		{
			const GXFloat* height = (const GXFloat*)data;
			itemHeight = *height;

			viewportSize = GXGetAABBHeight ( boundsLocal ) / GetTotalHeight ();
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_RESIZE:
		{
			const GXAABB* newBoundsLocal = (const GXAABB*)data;
			UpdateBoundsWorld ( *newBoundsLocal );
			viewportSize = GXGetAABBHeight ( boundsLocal ) / GetTotalHeight ();
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_REDRAW:
		{
			const GXVec2& position = gx_ui_TouchSurface->GetMousePosition ();
			OnMessage ( GX_MSG_MOUSE_MOVE, &position );
		}
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUIListBox::AddItem ( GXVoid* itemData )
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_LIST_BOX_ADD_ITEM, &itemData, sizeof ( GXVoid* ) );
}

GXVoid GXUIListBox::AddItems ( GXVoid** itemData, GXUInt items )
{
	if ( items == 0 || !itemData ) return;

	GXUInt s = items * sizeof ( GXUIListBoxRawItem );
	GXUIListBoxRawItem* list = (GXUIListBoxRawItem*)gx_ui_MessageBuffer->Allocate ( items * sizeof ( GXUIListBoxRawItem ) );
	GXUIListBoxRawItem* p = list;

	GXUInt limit = items - 1;
	for ( GXUInt i = 0; i < limit; i++ )
	{
		p->data = itemData[ i ];
		p->next = p + 1;
		p++;
	}

	p->data = itemData[ limit ];
	p->next = 0;

	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_LIST_BOX_ADD_ITEMS, &list, sizeof ( GXUIListBoxRawItem* ) );
}

GXVoid GXUIListBox::RemoveItem ( GXUInt itemIndex )
{
	if ( itemIndex >= totalItems ) return;

	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_LIST_BOX_REMOVE_ITEM, &itemIndex, sizeof ( GXUInt ) );
}

GXVoid GXUIListBox::RemoveAllItems ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_LIST_BOX_REMOVE_ALL_ITEMS, 0, 0 );
}

GXVoid* GXUIListBox::GetSelectedItem ()
{
	for ( GXUIListBoxItem* p = itemHead; p; p = p->next )
	{
		if ( p->isSelected )
			return p->data;
	}

	return 0;
}

GXUIListBoxItem* GXUIListBox::GetItems ()
{
	return itemHead;
}

GXUInt GXUIListBox::GetTotalItems ()
{
	return totalItems;
}

GXFloat GXUIListBox::GetViewportOffset ()
{
	return viewportOffset;
}

GXBool GXUIListBox::IsItemVisible ( GXUInt itemIndex )
{
	if ( itemIndex >= totalItems ) return GX_FALSE;

	GXFloat itemTop = itemIndex * itemHeight;
	GXFloat itemBottom = itemTop + itemHeight;

	GXFloat totalHeight = GetTotalHeight ();
	GXFloat viewportTop = totalHeight * viewportOffset;
	GXFloat viewportBottom = viewportTop + viewportSize * totalHeight;

	GXAABB item;
	GXAddVertexToAABB ( item, itemTop, 0.0f, 0.0f );
	GXAddVertexToAABB ( item, itemBottom, 1.0f, 1.0f );

	GXAABB viewport;
	GXAddVertexToAABB ( viewport, viewportTop, 0.0f, 0.0f );
	GXAddVertexToAABB ( viewport, viewportBottom, 1.0f, 1.0f );

	return GXIsOverlapedAABBAABB ( item, viewport );
}

GXFloat GXUIListBox::GetItemLocalOffsetY ( GXUInt itemIndex )
{
	if ( itemIndex >= totalItems ) return FLT_MAX;

	GXFloat itemBottom = itemIndex * itemHeight + itemHeight;
	GXFloat viewportBottom = GetTotalHeight () * ( viewportOffset + viewportSize );

	return viewportBottom - itemBottom;
}

GXVoid GXUIListBox::Redraw ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_REDRAW, 0, 0 );
}

GXVoid GXUIListBox::SetOnItemSelectedCallback ( GXVoid* handler, PFNGXUILISTBOXONITEMSELECTEDPROC callback )
{
	itemSelectedHandler = handler;
	OnItemSelected = callback;
}

GXVoid GXUIListBox::SetOnItemDoubleClickedCallback ( GXVoid* handler, PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC callback )
{
	itemDoubleClickedHandler = handler;
	OnItemDoubleClicked = callback;
}

GXFloat GXUIListBox::GetViewportSize ()
{
	return viewportSize;
}

GXVoid GXUIListBox::SetViewportOffset ( GXFloat offset )
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_LIST_BOX_SET_VIEWPORT_OFFSET, &offset, sizeof ( GXFloat ) );
}

GXVoid GXUIListBox::SetViewportOffsetImmediately ( GXFloat offset )
{
	viewportOffset = offset;

	viewportSize = GXGetAABBHeight ( boundsLocal ) / GetTotalHeight ();
	if ( renderer )
		renderer->OnUpdate ();
}

GXVoid GXUIListBox::SetItemHeight ( GXFloat height )
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_LIST_BOX_SET_ITEM_HEIGHT, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIListBox::GetItemHeight ()
{
	return itemHeight;
}

GXUIListBoxItem* GXUIListBox::FindItem ( const GXVec2 &mousePosition )
{
	GXFloat offset = GetTotalHeight () * viewportOffset;
	GXUIListBoxItem* p = itemHead;

	for ( GXUInt i = 0; i < totalItems; i++ )
	{
		GXAABB itemBounds;
		GXAddVertexToAABB ( itemBounds, boundsWorld.min.x, boundsWorld.max.y + offset - itemHeight, boundsWorld.min.z );
		GXAddVertexToAABB ( itemBounds, boundsWorld.max.x, boundsWorld.max.y + offset, boundsWorld.max.z );

		if ( GXIsOverlapedAABBVec3 ( itemBounds, mousePosition.x, mousePosition.y, 0.0f ) )
			 return p;

		offset -= itemHeight;
		p = p->next;
	}

	return 0;
}

GXUIListBoxItem* GXUIListBox::GetHighlightedItem ()
{
	for ( GXUIListBoxItem* p = itemHead; p; p = p->next )
	{
		if ( p->isHighlighted )
			return p;
	}

	return 0;
}

GXBool GXUIListBox::ResetHighlight ( const GXVec2 &mousePosition )
{
	GXBool isNeedRenderUpdate = GX_FALSE;

	GXUIListBoxItem* currentItem = FindItem ( mousePosition );
	if ( currentItem && !currentItem->isHighlighted )
	{
		currentItem->isHighlighted = GX_TRUE;
		isNeedRenderUpdate = GX_TRUE;
	}

	for ( GXUIListBoxItem* p = itemHead; p; p = p->next )
	{
		if ( p->isHighlighted && p != currentItem )
		{
			isNeedRenderUpdate = GX_TRUE;
			p->isHighlighted = GX_FALSE;
		}
	}

	return isNeedRenderUpdate;
}

GXBool GXUIListBox::IsAbleToScroll ()
{
	return totalItems * itemHeight > GXGetAABBHeight ( boundsLocal );
}

GXFloat GXUIListBox::GetTotalHeight ()
{
	GXFloat totalHeight = totalItems * itemHeight;
	GXFloat viewportHeight = GXGetAABBHeight ( boundsLocal );

	return totalHeight < viewportHeight ? viewportHeight : totalHeight;
}
