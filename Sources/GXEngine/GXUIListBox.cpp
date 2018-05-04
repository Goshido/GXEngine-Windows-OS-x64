// version 1.1

#include <GXEngine/GXUIListBox.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine_Editor_Mobile/EMUIFileListBox.h>		// TODO fix this!!
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <float.h>
#include <GXCommon/GXRestoreWarnings.h>


#define GX_UI_DEFAULT_ITEM_HEIGHT		0.55f

//---------------------------------------------------------------------------------------------------------------------

struct GXUIListBoxRawItem
{
	GXUIListBoxRawItem*			next;
	GXVoid*						data;
};

//---------------------------------------------------------------------------------------------------------------------

GXUIListBox::GXUIListBox ( GXWidget* parent, PFNGXUILISTBOXITEMDESTRUCTORPROC itemDestructor ):
	GXWidget ( parent ),
	itemHead ( nullptr ),
	itemTail ( nullptr ),
	itemHeight ( GX_UI_DEFAULT_ITEM_HEIGHT * gx_ui_Scale ),
	totalItems ( 0u ),
	DestroyItem ( itemDestructor ),
	OnItemSelected ( nullptr ),
	OnItemDoubleClicked ( nullptr ),
	itemSelectedHandler ( nullptr ),
	itemDoubleClickedHandler ( nullptr ),
	viewportOffset ( 0.0f ),
	viewportSize ( 1.0f )
{
	// NOTHING
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
			const GXVec2* pos = static_cast<const GXVec2*> ( data );

			GXBool isNeedRenderUpdate = ResetHighlight ( *pos );

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
			const GXVec3* scroll = static_cast<const GXVec3*> ( data );

			if ( IsAbleToScroll () )
			{
				GXFloat step = 1.0f / static_cast<GXFloat> ( totalItems );
				GXFloat offset = GXClampf ( viewportOffset - scroll->GetZ () * step, 0.0f, 1.0f - viewportSize );

				viewportOffset = offset;

				GXVec2 pos ( scroll->GetX (), scroll->GetZ () );
				OnMessage ( GX_MSG_MOUSE_MOVE, &pos );
			}
		}
		break;

		case GX_MSG_LMBDOWN:
		{
			GXUIListBoxItem* item = GetHighlightedItem ();
			GXUIListBoxItem* selected = nullptr;

			for ( selected = itemHead; selected; selected = selected->next )
			{
				if ( selected->isSelected )
					break;
			}

			if ( selected != item )
			{
				if ( selected )
					selected->isSelected = GX_FALSE;

				if ( item )
					item->isSelected = GX_TRUE;

				if ( OnItemSelected )
					OnItemSelected ( itemSelectedHandler, *this, item->data );

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
					OnItemDoubleClicked ( itemDoubleClickedHandler, *this, item->data );
			}
		}
		break;

		case GX_MSG_LIST_BOX_ADD_ITEM:
		{
			GXVoid* pointer = const_cast<GXVoid*> ( data );
			GXVoid** itemData = reinterpret_cast<GXVoid**> ( pointer );

			GXUIListBoxItem* item = new GXUIListBoxItem ();
			item->data = *itemData;
			item->isSelected = item->isHighlighted = GX_FALSE;

			item->next = nullptr;
			item->prev = itemTail;

			if ( itemTail )
				itemTail->next = item;
			else
				itemHead = item;

			itemTail = item;

			totalItems++;
			viewportSize = boundsLocal.GetHeight () / GetTotalHeight ();

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LIST_BOX_ADD_ITEMS:
		{
			GXVoid* pointer = const_cast<GXVoid*> ( data );
			GXUIListBoxRawItem** list = reinterpret_cast<GXUIListBoxRawItem**> ( pointer );

			for ( GXUIListBoxRawItem* p = *list; p; p = p->next )
			{
				GXUIListBoxItem* item = new GXUIListBoxItem ();
				item->data = p->data;
				item->isSelected = item->isHighlighted = GX_FALSE;

				item->next = nullptr;
				item->prev = itemTail;

				if ( itemTail )
					itemTail->next = item;
				else
					itemHead = item;

				itemTail = item;

				totalItems++;
			}

			viewportSize = boundsLocal.GetHeight () / GetTotalHeight ();

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

			itemTail = nullptr;
			totalItems = 0;

			viewportOffset = 0.0f;
			viewportSize = boundsLocal.GetHeight () / GetTotalHeight ();

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LIST_BOX_REMOVE_ITEM:
		{
			const GXUInt* itemIndex = static_cast<const GXUInt*> ( data );

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

			viewportSize = boundsLocal.GetHeight () / GetTotalHeight ();

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LIST_BOX_SET_VIEWPORT_OFFSET:
		{
			const GXFloat* offset = static_cast<const GXFloat*> ( data );
			viewportOffset = *offset;

			viewportSize = boundsLocal.GetHeight () / GetTotalHeight ();

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LIST_BOX_SET_ITEM_HEIGHT:
		{
			const GXFloat* height = static_cast<const GXFloat*> ( data );
			itemHeight = *height;

			viewportSize = boundsLocal.GetHeight () / GetTotalHeight ();

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_RESIZE:
		{
			const GXAABB* newBoundsLocal = static_cast<const GXAABB*> ( data );
			UpdateBoundsWorld ( *newBoundsLocal );
			viewportSize = boundsLocal.GetHeight () / GetTotalHeight ();

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_REDRAW:
		{
			const GXVec2& position = GXTouchSurface::GetInstance ().GetMousePosition ();
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
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_LIST_BOX_ADD_ITEM, &itemData, sizeof ( GXVoid* ) );
}

GXVoid GXUIListBox::AddItems ( GXVoid** itemData, GXUInt items )
{
	if ( items == 0 || !itemData ) return;

	GXUIListBoxRawItem* list = static_cast<GXUIListBoxRawItem*> ( gx_ui_MessageBuffer->Allocate ( items * sizeof ( GXUIListBoxRawItem ) ) );
	GXUIListBoxRawItem* p = list;

	GXUInt limit = items - 1;

	for ( GXUInt i = 0u; i < limit; i++ )
	{
		p->data = itemData[ i ];
		p->next = p + 1;
		p++;
	}

	p->data = itemData[ limit ];
	p->next = nullptr;

	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_LIST_BOX_ADD_ITEMS, &list, sizeof ( GXUIListBoxRawItem* ) );
}

GXVoid GXUIListBox::RemoveItem ( GXUInt itemIndex )
{
	if ( itemIndex >= totalItems ) return;

	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_LIST_BOX_REMOVE_ITEM, &itemIndex, sizeof ( GXUInt ) );
}

GXVoid GXUIListBox::RemoveAllItems ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_LIST_BOX_REMOVE_ALL_ITEMS, nullptr, 0 );
}

GXVoid* GXUIListBox::GetSelectedItem () const
{
	for ( GXUIListBoxItem* p = itemHead; p; p = p->next )
	{
		if ( p->isSelected )
			return p->data;
	}

	return nullptr;
}

GXUIListBoxItem* GXUIListBox::GetItems () const
{
	return itemHead;
}

GXUInt GXUIListBox::GetTotalItems () const
{
	return totalItems;
}

GXFloat GXUIListBox::GetViewportOffset () const
{
	return viewportOffset;
}

GXBool GXUIListBox::IsItemVisible ( GXUInt itemIndex ) const
{
	if ( itemIndex >= totalItems ) return GX_FALSE;

	GXFloat itemTop = itemIndex * itemHeight;
	GXFloat itemBottom = itemTop + itemHeight;

	GXFloat totalHeight = GetTotalHeight ();
	GXFloat viewportTop = totalHeight * viewportOffset;
	GXFloat viewportBottom = viewportTop + viewportSize * totalHeight;

	GXAABB item;
	item.AddVertex ( itemTop, 0.0f, 0.0f );
	item.AddVertex ( itemBottom, 1.0f, 1.0f );

	GXAABB viewport;
	viewport.AddVertex ( viewportTop, 0.0f, 0.0f );
	viewport.AddVertex ( viewportBottom, 1.0f, 1.0f );

	return item.IsOverlaped ( viewport );
}

GXFloat GXUIListBox::GetItemLocalOffsetY ( GXUInt itemIndex ) const
{
	if ( itemIndex >= totalItems ) return FLT_MAX;

	GXFloat itemBottom = itemIndex * itemHeight + itemHeight;
	GXFloat viewportBottom = GetTotalHeight () * ( viewportOffset + viewportSize );

	return viewportBottom - itemBottom;
}

GXVoid GXUIListBox::Redraw ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_REDRAW, 0, 0 );
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

GXFloat GXUIListBox::GetViewportSize () const
{
	return viewportSize;
}

GXVoid GXUIListBox::SetViewportOffset ( GXFloat offset )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_LIST_BOX_SET_VIEWPORT_OFFSET, &offset, sizeof ( GXFloat ) );
}

GXVoid GXUIListBox::SetViewportOffsetImmediately ( GXFloat offset )
{
	viewportOffset = offset;

	viewportSize = boundsLocal.GetHeight () / GetTotalHeight ();

	if ( renderer )
		renderer->OnUpdate ();
}

GXVoid GXUIListBox::SetItemHeight ( GXFloat height )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_LIST_BOX_SET_ITEM_HEIGHT, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIListBox::GetItemHeight () const
{
	return itemHeight;
}

GXUIListBoxItem* GXUIListBox::FindItem ( const GXVec2 &mousePosition )
{
	GXFloat offset = GetTotalHeight () * viewportOffset;
	GXUIListBoxItem* p = itemHead;

	for ( GXUInt i = 0u; i < totalItems; i++ )
	{
		GXAABB itemBounds;
		itemBounds.AddVertex ( boundsWorld.min.GetX (), boundsWorld.max.GetY () + offset - itemHeight, boundsWorld.min.GetZ () );
		itemBounds.AddVertex ( boundsWorld.max.GetX (), boundsWorld.max.GetY () + offset, boundsWorld.max.GetZ () );

		if ( itemBounds.IsOverlaped ( mousePosition.GetX (), mousePosition.GetY (), 0.0f ) )
			 return p;

		offset -= itemHeight;
		p = p->next;
	}

	return nullptr;
}

GXUIListBoxItem* GXUIListBox::GetHighlightedItem () const
{
	for ( GXUIListBoxItem* p = itemHead; p; p = p->next )
	{
		if ( p->isHighlighted )
			return p;
	}

	return nullptr;
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

GXBool GXUIListBox::IsAbleToScroll () const
{
	return totalItems * itemHeight > boundsLocal.GetHeight ();
}

GXFloat GXUIListBox::GetTotalHeight () const
{
	GXFloat totalHeight = totalItems * itemHeight;
	GXFloat viewportHeight = boundsLocal.GetHeight ();

	return totalHeight < viewportHeight ? viewportHeight : totalHeight;
}
