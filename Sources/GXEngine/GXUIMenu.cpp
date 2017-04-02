//version 1.0

#include <GXEngine/GXUIMenu.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXStrings.h>


#define GX_ANY_HEIGHT 1.0f


struct GXUIMenuItem
{
	GXWChar*	name;
	GXAABB		bounds;
	GXUIPopup*	popup;
};

//-----------------------------------------------------------------

GXUIMenu::GXUIMenu ( GXWidget* parent ):
GXWidget ( parent ), items ( sizeof ( GXUIMenuItem ) )
{
	selectedItemIndex = highlightedItemIndex = GX_UI_MENU_INVALID_INDEX;
}

GXUIMenu::~GXUIMenu ()
{
	GXUInt totalItems = items.GetLength ();
	GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();

	for ( GXUByte i = 0; i < totalItems; i++ )
		GXSafeFree ( itemStorage[ i ].name );
}

GXVoid GXUIMenu::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_MENY_ADD_ITEM:
		{
			const GXUIMenuItem* item = (const GXUIMenuItem*)data;
			GXFloat itemWidth = GXGetAABBWidth ( item->bounds );

			GXUIMenuItem newItem;
			if ( item->name )
				GXWcsclone ( &newItem.name, item->name );
			else
				newItem.name = nullptr;

			newItem.popup = item->popup;
			GXAABB newBoundsLocal;
			GXAddVertexToAABB ( newBoundsLocal, boundsLocal.min );

			GXUInt totalItems = items.GetLength ();
			if ( totalItems > 0 )
			{
				GXAddVertexToAABB ( newItem.bounds, boundsWorld.max.x, boundsWorld.min.y, -1.0f );
				GXAddVertexToAABB ( newItem.bounds, boundsWorld.max.x + itemWidth, boundsWorld.max.y, 1.0f );

				GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();
				GXAddVertexToAABB ( newBoundsLocal, boundsLocal.min.x + newItem.bounds.max.x - itemStorage[ 0 ].bounds.min.x, boundsLocal.max.y, boundsLocal.max.z );
			}
			else
			{
				GXAddVertexToAABB ( newItem.bounds, boundsWorld.min.x, boundsWorld.min.y, -1.0f );
				GXAddVertexToAABB ( newItem.bounds, boundsWorld.min.x + itemWidth, boundsWorld.max.y, 1.0f );

				GXAddVertexToAABB ( newBoundsLocal, boundsLocal.min.x + itemWidth, boundsLocal.max.y, boundsLocal.max.z );
			}

			items.SetValue ( totalItems, &newItem );
			GXWidget::OnMessage ( GX_MSG_RESIZE, &newBoundsLocal );
		}
		break;

		case GX_MSG_LMBDOWN:
		{
			GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();

			if ( highlightedItemIndex == selectedItemIndex )
			{
				if ( itemStorage[ highlightedItemIndex ].popup )
					itemStorage[ highlightedItemIndex ].popup->Hide ();

				selectedItemIndex = GX_UI_MENU_INVALID_INDEX;
			}
			else
			{
				if ( selectedItemIndex != GX_UI_MENU_INVALID_INDEX && itemStorage[ selectedItemIndex ].popup )
					itemStorage[ selectedItemIndex ].popup->Hide ();

				GXUIMenuItem* item = itemStorage + highlightedItemIndex;

				if ( item->popup )
				{
					GXFloat popupWidth = GXGetAABBWidth ( item->popup->GetBoundsLocal () );
					GXFloat popupHeight = GXGetAABBHeight ( item->popup->GetBoundsLocal () );
					item->popup->Resize ( item->bounds.min.x, item->bounds.min.y - popupHeight, popupWidth, popupHeight );
					item->popup->Show ( this );
				}

				selectedItemIndex = highlightedItemIndex;
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = (const GXVec2*)data;

			GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();
			GXUByte totalItems = (GXUByte)items.GetLength ();
			GXUByte mouseoverItemIndex = 0;
			for ( ; mouseoverItemIndex < totalItems; mouseoverItemIndex++ )
				if ( GXIsOverlapedAABBVec3 ( itemStorage[ mouseoverItemIndex ].bounds, pos->x, pos->y, 0.0f ) ) break;

			if ( highlightedItemIndex != mouseoverItemIndex )
			{
				highlightedItemIndex = mouseoverItemIndex;
				if ( renderer )
					renderer->OnUpdate ();
			}
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			highlightedItemIndex = GX_UI_MENU_INVALID_INDEX;
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_RESIZE:
		{
			const GXAABB* bounds = (const GXAABB*)data;

			GXUByte totalItems = (GXUByte)items.GetLength ();
			GXVec2 locationDelta = GXCreateVec2 ( bounds->min.x - boundsLocal.min.x, bounds->min.y - boundsLocal.min.y );
			GXAABB newBoundsLocal;

			if ( totalItems > 0 )
			{
				GXFloat heightDelta = GXGetAABBHeight ( boundsLocal ) - GXGetAABBHeight ( *bounds );
				GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();

				for ( GXUByte i = 0; i < totalItems; i++ )
				{
					GXUIMenuItem* currentItem = itemStorage + i;

					currentItem->bounds.min.x += locationDelta.x;
					currentItem->bounds.max.x += locationDelta.x;

					currentItem->bounds.min.y += locationDelta.y;
					currentItem->bounds.max.y += locationDelta.y + heightDelta;
				}

				GXAddVertexToAABB ( newBoundsLocal, bounds->min.x, bounds->min.y, -1.0f );
				GXAddVertexToAABB ( newBoundsLocal, bounds->min.x + itemStorage[ totalItems ].bounds.max.x - itemStorage[ 0 ].bounds.min.x, bounds->min.y + itemStorage[ totalItems ].bounds.max.y - itemStorage[ 0 ].bounds.min.y, 1.0f );
			}
			else
			{
				newBoundsLocal = *bounds;
			}

			GXWidget::OnMessage ( message, &newBoundsLocal );
		}
		break;

		case GX_MSG_POPUP_CLOSED:
		{
			if ( selectedItemIndex == GX_UI_MENU_INVALID_INDEX ) break;

			const GXUIPopup** popup = (const GXUIPopup**)data;
			GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();
			if ( itemStorage[ selectedItemIndex ].popup == *popup )
			{
				selectedItemIndex = GX_UI_MENU_INVALID_INDEX;
				if ( renderer )
					renderer->OnUpdate ();
			}
		}
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUIMenu::AddItem ( const GXWChar* name, GXFloat itemWidth, GXUIPopup* popup )
{
	if ( !name ) return;

	GXUIMenuItem item;

	GXUInt size = ( GXWcslen ( name ) + 1 ) * sizeof ( GXWChar );
	item.name = (GXWChar*)gx_ui_MessageBuffer->Allocate ( size );
	memcpy ( item.name, name, size );
	GXAddVertexToAABB ( item.bounds, 0.0f, 0.0f, -1.0f );
	GXAddVertexToAABB ( item.bounds, itemWidth, GX_ANY_HEIGHT, 1.0f );
	item.popup = popup;

	GXTouchSurface::GetInstance ()->SendMessage ( this, GX_MSG_MENY_ADD_ITEM, &item, sizeof ( GXUIMenuItem ) );
}

GXUByte GXUIMenu::GetTotalItems () const
{
	return (GXUByte)items.GetLength ();
}

const GXWChar* GXUIMenu::GetItemName ( GXUByte itemIndex ) const
{
	if ( itemIndex >= (GXUByte)items.GetLength () ) return nullptr;

	GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();
	return itemStorage[ itemIndex ].name;
}

GXFloat GXUIMenu::GetItemOffset ( GXUByte itemIndex ) const
{
	if ( itemIndex >= (GXUByte)items.GetLength () ) return -1.0f;

	GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();
	return itemStorage[ itemIndex ].bounds.min.x - boundsWorld.min.x;
}

GXFloat GXUIMenu::GetItemWidth ( GXUByte itemIndex ) const
{
	if ( itemIndex >= (GXUByte)items.GetLength () ) return 0.0f;

	GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();
	return GXGetAABBWidth ( itemStorage[ itemIndex ].bounds );
}

GXUByte GXUIMenu::GetSelectedItemIndex () const
{
	return selectedItemIndex;
}

GXUByte GXUIMenu::GetHighlightedItemIndex () const
{
	return highlightedItemIndex;
}
