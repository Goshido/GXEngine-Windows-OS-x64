// version 1.3

#include <GXEngine/GXUIMenu.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXStrings.h>


#define ANY_HEIGHT		1.0f
#define INVALID_OFFSET	-1.0f
#define INVALID_WIDTH	0.0f


struct GXUIMenuItem final
{
	GXWChar*	name;
	GXAABB		bounds;
	GXUIPopup*	popup;
};

//-----------------------------------------------------------------

GXUIMenu::GXUIMenu ( GXWidget* parent ):
	GXWidget ( parent ),
	items ( sizeof ( GXUIMenuItem ) ),
	selectedItemIndex ( static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX ) ),
	highlightedItemIndex ( static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX ) )
{
	// NOTHING
}

GXUIMenu::~GXUIMenu ()
{
	GXUByte totalItems = static_cast<GXUByte> ( items.GetLength () );
	GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( items.GetData () );

	for ( GXUByte i = 0u; i < totalItems; ++i )
		GXSafeFree ( itemStorage[ i ].name );
}

GXVoid GXUIMenu::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
	if ( message == eGXUIMessage::MenuAddItem )
	{
		const GXUIMenuItem* item = static_cast<const GXUIMenuItem*> ( data );
		GXFloat itemWidth = item->bounds.GetWidth ();

		GXUIMenuItem newItem;

		if ( item->name )
			GXWcsclone ( &newItem.name, item->name );
		else
			newItem.name = nullptr;

		newItem.popup = item->popup;
		GXAABB newBoundsLocal;
		newBoundsLocal.AddVertex ( boundsLocal.min );
		GXUByte totalItems = static_cast<GXUByte> ( items.GetLength () );

		if ( totalItems > 0u )
		{
			newItem.bounds.AddVertex ( boundsWorld.max.GetX (), boundsWorld.min.GetY (), -1.0f );
			newItem.bounds.AddVertex ( boundsWorld.max.GetX () + itemWidth, boundsWorld.max.GetY (), 1.0f );

			GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( items.GetData () );
			newBoundsLocal.AddVertex ( boundsLocal.min.GetX () + newItem.bounds.max.GetX () - itemStorage[ 0 ].bounds.min.GetX (), boundsLocal.max.GetY (), boundsLocal.max.GetZ () );
		}
		else
		{
			newItem.bounds.AddVertex ( boundsWorld.min.GetX (), boundsWorld.min.GetY (), -1.0f );
			newItem.bounds.AddVertex ( boundsWorld.min.GetX () + itemWidth, boundsWorld.max.GetY (), 1.0f );

			newBoundsLocal.AddVertex ( boundsLocal.min.GetX () + itemWidth, boundsLocal.max.GetY (), boundsLocal.max.GetZ () );
		}

		items.SetValue ( totalItems, &newItem );
		GXWidget::OnMessage ( eGXUIMessage::Resize, &newBoundsLocal );

		return;
	}

	if ( message == eGXUIMessage::LMBDown )
	{
		GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( items.GetData () );

		if ( highlightedItemIndex == selectedItemIndex )
		{
			if ( itemStorage[ highlightedItemIndex ].popup )
				itemStorage[ highlightedItemIndex ].popup->Hide ();

			selectedItemIndex = static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX );
		}
		else
		{
			if ( selectedItemIndex != static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX ) && itemStorage[ selectedItemIndex ].popup )
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

		return;
	}

	if ( message == eGXUIMessage::MouseMove )
	{
		const GXVec2* pos = static_cast<const GXVec2*> ( data );

		GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( items.GetData () );
		GXUByte totalItems = static_cast<GXUByte> ( items.GetLength () );
		GXUByte mouseoverItemIndex = 0u;

		for ( ; mouseoverItemIndex < totalItems; ++mouseoverItemIndex )
			if ( itemStorage[ mouseoverItemIndex ].bounds.IsOverlaped ( pos->GetX (), pos->GetY (), 0.0f ) ) break;

		if ( highlightedItemIndex == mouseoverItemIndex ) return;

		highlightedItemIndex = mouseoverItemIndex;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::MouseLeave )
	{
		highlightedItemIndex = static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX );

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::Resize )
	{
		const GXAABB* bounds = static_cast<const GXAABB*> ( data );

		GXUByte totalItems = static_cast<GXUByte> ( items.GetLength () );
		GXVec2 locationDelta ( bounds->min.GetX () - boundsLocal.min.GetX (), bounds->min.GetY () - boundsLocal.min.GetY () );
		GXAABB newBoundsLocal;

		if ( totalItems > 0u )
		{
			GXFloat heightDelta = boundsLocal.GetHeight () - bounds->GetHeight ();
			GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( items.GetData () );

			for ( GXUByte i = 0u; i < totalItems; ++i )
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
		return;
	}

	if ( message == eGXUIMessage::PopupClosed )
	{
		if ( selectedItemIndex == GX_UI_MENU_INVALID_INDEX ) return;

		GXVoid* pointer = const_cast<GXVoid*> ( data );
		const GXUIPopup** popup = reinterpret_cast<const GXUIPopup**> ( pointer );
		GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();

		if ( itemStorage[ selectedItemIndex ].popup != *popup ) return;

		selectedItemIndex = GX_UI_MENU_INVALID_INDEX;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	GXWidget::OnMessage ( message, data );
}

GXVoid GXUIMenu::AddItem ( const GXWChar* name, GXFloat itemWidth, GXUIPopup* popup )
{
	if ( !name ) return;

	GXUIMenuItem item;

	GXUInt size = ( GXWcslen ( name ) + 1 ) * sizeof ( GXWChar );
	item.name = static_cast<GXWChar*> ( gx_ui_MessageBuffer->Allocate ( size ) );
	memcpy ( item.name, name, size );
	item.bounds.AddVertex ( 0.0f, 0.0f, -1.0f );
	item.bounds.AddVertex ( itemWidth, ANY_HEIGHT, 1.0f );
	item.popup = popup;

	GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::MenuAddItem, &item, sizeof ( GXUIMenuItem ) );
	popup->Hide ();
}

GXUByte GXUIMenu::GetTotalItems () const
{
	return (GXUByte)items.GetLength ();
}

const GXWChar* GXUIMenu::GetItemName ( GXUByte itemIndex ) const
{
	if ( itemIndex >= static_cast<GXUByte> ( items.GetLength () ) ) return nullptr;

	GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( items.GetData () );
	return itemStorage[ itemIndex ].name;
}

GXFloat GXUIMenu::GetItemOffset ( GXUByte itemIndex ) const
{
	if ( itemIndex >= static_cast<GXUByte> ( items.GetLength () ) ) return INVALID_OFFSET;

	GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( items.GetData () );
	return itemStorage[ itemIndex ].bounds.min.GetX () - boundsWorld.min.GetX ();
}

GXFloat GXUIMenu::GetItemWidth ( GXUByte itemIndex ) const
{
	if ( itemIndex >= static_cast<GXUByte> ( items.GetLength () ) ) return INVALID_WIDTH;

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
