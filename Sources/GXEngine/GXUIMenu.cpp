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
			GXFloat itemWidth = item->bounds.GetWidth ();

			GXUIMenuItem newItem;
			if ( item->name )
				GXWcsclone ( &newItem.name, item->name );
			else
				newItem.name = nullptr;

			newItem.popup = item->popup;
			GXAABB newBoundsLocal;
			newBoundsLocal.AddVertex ( boundsLocal.min );
			GXUInt totalItems = items.GetLength ();

			if ( totalItems > 0 )
			{
				newItem.bounds.AddVertex ( boundsWorld.max.GetX (), boundsWorld.min.GetY (), -1.0f );
				newItem.bounds.AddVertex ( boundsWorld.max.GetX () + itemWidth, boundsWorld.max.GetY (), 1.0f );

				GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();
				newBoundsLocal.AddVertex ( boundsLocal.min.GetX () + newItem.bounds.max.GetX () - itemStorage[ 0 ].bounds.min.GetX (), boundsLocal.max.GetY (), boundsLocal.max.GetZ () );
			}
			else
			{
				newItem.bounds.AddVertex ( boundsWorld.min.GetX (), boundsWorld.min.GetY (), -1.0f );
				newItem.bounds.AddVertex ( boundsWorld.min.GetX () + itemWidth, boundsWorld.max.GetY (), 1.0f );

				newBoundsLocal.AddVertex ( boundsLocal.min.GetX () + itemWidth, boundsLocal.max.GetY (), boundsLocal.max.GetZ () );
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
					GXFloat popupWidth = item->popup->GetBoundsLocal ().GetWidth ();
					GXFloat popupHeight = item->popup->GetBoundsLocal ().GetHeight ();
					item->popup->Resize ( item->bounds.min.GetX (), item->bounds.min.GetY () - popupHeight, popupWidth, popupHeight );
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
				if ( itemStorage[ mouseoverItemIndex ].bounds.IsOverlaped ( pos->GetX (), pos->GetY (), 0.0f ) ) break;

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
			GXVec2 locationDelta ( bounds->min.GetX () - boundsLocal.min.GetX (), bounds->min.GetY () - boundsLocal.min.GetY () );
			GXAABB newBoundsLocal;

			if ( totalItems > 0 )
			{
				GXFloat heightDelta = boundsLocal.GetHeight () - bounds->GetHeight ();
				GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();

				for ( GXUByte i = 0; i < totalItems; i++ )
				{
					GXUIMenuItem* currentItem = itemStorage + i;

					currentItem->bounds.min.data[ 0 ] += locationDelta.data[ 0 ];
					currentItem->bounds.max.data[ 0 ] += locationDelta.data[ 0 ];

					currentItem->bounds.min.data[ 1 ] += locationDelta.data[ 1 ];
					currentItem->bounds.max.data[ 1 ] += locationDelta.data[ 1 ] + heightDelta;
				}

				newBoundsLocal.AddVertex ( bounds->min.GetX (), bounds->min.GetY (), -1.0f );
				newBoundsLocal.AddVertex ( bounds->min.GetX () + itemStorage[ totalItems ].bounds.max.GetX () - itemStorage[ 0 ].bounds.min.GetX (), bounds->min.GetY () + itemStorage[ totalItems ].bounds.max.GetY () - itemStorage[ 0 ].bounds.min.GetY (), 1.0f );
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
	item.bounds.AddVertex ( 0.0f, 0.0f, -1.0f );
	item.bounds.AddVertex ( itemWidth, GX_ANY_HEIGHT, 1.0f );
	item.popup = popup;

	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_MENY_ADD_ITEM, &item, sizeof ( GXUIMenuItem ) );
	popup->Hide ();
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
	return itemStorage[ itemIndex ].bounds.min.GetX () - boundsWorld.min.GetX ();
}

GXFloat GXUIMenu::GetItemWidth ( GXUByte itemIndex ) const
{
	if ( itemIndex >= (GXUByte)items.GetLength () ) return 0.0f;

	GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();
	return itemStorage[ itemIndex ].bounds.GetWidth ();
}

GXUByte GXUIMenu::GetSelectedItemIndex () const
{
	return selectedItemIndex;
}

GXUByte GXUIMenu::GetHighlightedItemIndex () const
{
	return highlightedItemIndex;
}
