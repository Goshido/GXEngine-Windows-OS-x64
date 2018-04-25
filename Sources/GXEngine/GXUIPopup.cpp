// version 1.1

#include <GXEngine/GXUIPopup.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>


#define DEFAULT_HEIGHT	0.5f


struct GXUIPopupItem
{
	GXBool						isActive;
	GXVoid*						handler;
	PFNGXONUIPOPUPACTIONPROC	action;
	GXAABB						boundsWorld;
};

//---------------------------------------------------------------------

GXUIPopup::GXUIPopup ( GXWidget* parent ):
	GXWidget ( parent ),
	items ( sizeof ( GXUIPopupItem ) ),
	selectedItemIndex ( static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX ) ),
	itemHeight ( gx_ui_Scale * DEFAULT_HEIGHT ),
	owner ( nullptr )
{
	// NOTHING
}

GXUIPopup::~GXUIPopup ()
{
	// NOTHING
}

GXVoid GXUIPopup::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_POPUP_ADD_ITEM:
		{
			const GXUIPopupItem* pi = static_cast<const GXUIPopupItem*> ( data );

			GXUByte totalItems = static_cast<GXUByte> ( items.GetLength () );
			GXUIPopupItem item;
			item.handler = pi->handler;
			item.action = pi->action;
			item.isActive = GX_TRUE;

			if ( totalItems == 0u )
			{
				item.boundsWorld.AddVertex ( boundsLocal.min.GetX (), boundsLocal.max.GetY () - itemHeight, boundsLocal.min.GetZ () );
				item.boundsWorld.AddVertex ( boundsLocal.max.GetX (), boundsLocal.max.GetY (), boundsLocal.max.GetZ () );

				UpdateBoundsWorld ( item.boundsWorld );
			}
			else
			{
				item.boundsWorld.AddVertex ( boundsLocal.min.GetX (), boundsLocal.min.GetY () - itemHeight, boundsLocal.min.GetZ () );
				item.boundsWorld.AddVertex ( boundsLocal.max.GetX (), boundsLocal.min.GetY (), boundsLocal.max.GetZ () );

				GXAABB newBoundsLocal ( boundsLocal );
				newBoundsLocal.min.data[ 1 ] -= itemHeight;
				UpdateBoundsWorld ( newBoundsLocal );
			}

			items.SetValue ( totalItems, &item );
			totalItems++;

			if ( totalItems != 0u && renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_POPUP_SET_ITEM_HEIGHT:
		{
			const GXFloat* newItemHeight = static_cast<const GXFloat*> ( data );
			itemHeight = *newItemHeight;

			GXUByte totalItems = static_cast<GXUByte> ( items.GetLength () );

			if ( totalItems != 0u )
			{
				GXUIPopupItem* itemStorage = (GXUIPopupItem*)items.GetData ();

				GXAABB newBoundsLocal ( boundsLocal );
				newBoundsLocal.min.SetY ( newBoundsLocal.max.GetY () );

				for ( GXUInt i = 0u; i < totalItems; i++ )
				{
					itemStorage[ i ].boundsWorld.Empty ();
					itemStorage[ i ].boundsWorld.AddVertex ( newBoundsLocal.max.GetX (), newBoundsLocal.min.GetY (), newBoundsLocal.max.GetZ () );
					newBoundsLocal.min.data[ 1 ] -= itemHeight;
					itemStorage[ i ].boundsWorld.AddVertex ( newBoundsLocal.min.GetX (), newBoundsLocal.min.GetY (), newBoundsLocal.min.GetZ () );
				}

				UpdateBoundsWorld ( newBoundsLocal );
			}

			if ( totalItems != 0u && renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_POPUP_ENABLE_ITEM:
		{
			const GXUByte* itemIndex = static_cast<const GXUByte*> ( data );

			if ( static_cast<GXUPointer> ( *itemIndex ) >= items.GetLength () ) break;

			GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( items.GetData () );
			if ( itemStorage[ *itemIndex ].isActive ) break;

			itemStorage[ *itemIndex ].isActive = GX_TRUE;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_POPUP_DISABLE_ITEM:
		{
			const GXUByte* itemIndex = static_cast<const GXUByte*> ( data );

			if ( static_cast<GXUPointer> ( *itemIndex ) >= items.GetLength () ) break;

			GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( items.GetData () );
			if ( !itemStorage[ *itemIndex ].isActive ) break;

			itemStorage[ *itemIndex ].isActive = GX_FALSE;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_POPUP_SHOW:
		{
			GXVoid* pointer = const_cast<GXVoid*> ( data );
			GXWidget** currentOwner = reinterpret_cast<GXWidget**> ( pointer );
			owner = *currentOwner;
			GXWidget::Show ();
		}
		break;

		case GX_MSG_LMBUP:
		{
			if ( selectedItemIndex != GX_UI_POPUP_INVALID_INDEX )
			{
				GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( items.GetData () );

				if ( itemStorage[ selectedItemIndex ].action )
					itemStorage[ selectedItemIndex ].action ( itemStorage[ selectedItemIndex ].handler );

				if ( owner )
				{
					GXUIPopup* pointer = this;
					GXTouchSurface::GetInstance ().SendMessage ( owner, GX_MSG_POPUP_CLOSED, &pointer, sizeof ( GXUIPopup* ) );
				}

				selectedItemIndex = GX_UI_POPUP_INVALID_INDEX;

				if ( renderer )
					renderer->OnUpdate ();
			}

			Hide ();
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = static_cast<const GXVec2*> ( data );
			GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( items.GetData () );
			GXUByte totalItems = static_cast<GXUByte> ( items.GetLength () );
			
			GXUByte mouseOverItem = 0u;

			for ( ; mouseOverItem < totalItems; mouseOverItem++ )
				if ( itemStorage[ mouseOverItem ].boundsWorld.IsOverlaped ( pos->GetX (), pos->GetY (), 0.0f ) ) break;

			if ( selectedItemIndex == static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX ) && !itemStorage[ mouseOverItem ].isActive ) break;
			if ( selectedItemIndex == mouseOverItem && itemStorage[ mouseOverItem ].isActive ) break;

			selectedItemIndex = itemStorage[ mouseOverItem ].isActive ? mouseOverItem : static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			if ( selectedItemIndex != static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX ) )
			{
				selectedItemIndex = static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX );

				if ( renderer )
					renderer->OnUpdate ();
			}

			if ( owner )
			{
				GXUIPopup* pointer = this;
				GXTouchSurface::GetInstance ().SendMessage ( owner, GX_MSG_POPUP_CLOSED, &pointer, sizeof ( GXUIPopup* ) );
			}

			Hide ();
		}
		break;

		case GX_MSG_RESIZE:
		{
			const GXAABB* newBoundsLocal = static_cast<const GXAABB*> ( data );
			GXFloat newWidth = newBoundsLocal->GetWidth ();
			GXVec2 newPosition ( newBoundsLocal->min.GetX (), newBoundsLocal->min.GetY () );

			GXUByte totalItems = static_cast<GXUByte> ( items.GetLength () );
			GXFloat top = newPosition.GetY () + totalItems * itemHeight;
			GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( items.GetData () );

			for ( GXUInt i = 0u; i < totalItems; i++ )
			{
				itemStorage[ i ].boundsWorld.Empty ();
				itemStorage[ i ].boundsWorld.AddVertex ( newPosition.GetX (), top - itemHeight, -1.0f );
				itemStorage[ i ].boundsWorld.AddVertex ( newPosition.GetX () + newWidth, top, 1.0f );
				top -= itemHeight;
			}
			
			GXAABB bounds;
			bounds.AddVertex ( newPosition.GetX (), newPosition.GetY (), -1.0f );
			bounds.AddVertex ( newPosition.GetX () + newWidth, newPosition.GetY () + totalItems * itemHeight, 1.0f );
			GXWidget::OnMessage ( message, &bounds );
		}
		break;

		case GX_MSG_HIDE:
			selectedItemIndex = static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX );
			GXWidget::OnMessage ( message, data );
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUIPopup::AddItem ( GXVoid* handler, PFNGXONUIPOPUPACTIONPROC action )
{
	GXUIPopupItem pi;
	pi.handler = handler;
	pi.action = action;
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_POPUP_ADD_ITEM, &pi, sizeof ( GXUIPopupItem ) );
}

GXUByte GXUIPopup::GetTotalItems () const
{
	return static_cast<GXUByte> ( items.GetLength () );
}

GXVoid GXUIPopup::EnableItem ( GXUByte itemIndex )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_POPUP_ENABLE_ITEM, &itemIndex, sizeof ( GXUByte ) );
}

GXVoid GXUIPopup::DisableItem ( GXUByte itemIndex )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_POPUP_DISABLE_ITEM, &itemIndex, sizeof ( GXUByte ) );
}

GXBool GXUIPopup::IsItemActive ( GXUByte itemIndex ) const
{
	if ( (GXUInt)itemIndex >= items.GetLength () ) return GX_FALSE;

	GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( items.GetData () );
	return itemStorage[ itemIndex ].isActive;
}

GXUByte GXUIPopup::GetSelectedItemIndex () const
{
	return selectedItemIndex;
}

GXVoid GXUIPopup::SetItemHeight ( GXFloat height )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_POPUP_SET_ITEM_HEIGHT, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIPopup::GetItemHeight () const
{
	return itemHeight;
}

GXFloat GXUIPopup::GetItemWidth () const
{
	return boundsLocal.GetWidth ();
}

GXVoid GXUIPopup::Show ( GXWidget* currentOwner )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_POPUP_SHOW, &currentOwner, sizeof ( GXWidget* ) );
}
