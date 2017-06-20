//version 1.0

#include <GXEngine/GXUIPopup.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>


#define GX_DEFAULT_HEIGHT	0.5f

struct GXUIPopupItem
{
	GXBool						isActive;
	GXVoid*						handler;
	PFNGXONUIPOPUPACTIONPROC	action;
	GXAABB						boundsWorld;
};

//---------------------------------------------------------------------

GXUIPopup::GXUIPopup ( GXWidget* parent ):
GXWidget ( parent ), items ( sizeof ( GXUIPopupItem ) )
{
	selectedItemIndex = GX_UI_POPUP_INVALID_INDEX;
	itemHeight = gx_ui_Scale * GX_DEFAULT_HEIGHT;
	owner = nullptr;
}

GXUIPopup::~GXUIPopup ()
{
	//NOTHING
}

GXVoid GXUIPopup::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_POPUP_ADD_ITEM:
		{
			const GXUIPopupItem* pi = (const GXUIPopupItem*)data;

			GXUInt totalItems = items.GetLength ();
			GXUIPopupItem item;
			item.handler = pi->handler;
			item.action = pi->action;
			item.isActive = GX_TRUE;

			if ( totalItems == 0 )
			{
				GXAddVertexToAABB ( item.boundsWorld, boundsLocal.min.x, boundsLocal.max.y - itemHeight, boundsLocal.min.z );
				GXAddVertexToAABB ( item.boundsWorld, boundsLocal.max.x, boundsLocal.max.y, boundsLocal.max.z );

				UpdateBoundsWorld ( item.boundsWorld );
			}
			else
			{
				GXAddVertexToAABB ( item.boundsWorld, boundsLocal.min.x, boundsLocal.min.y - itemHeight, boundsLocal.min.z );
				GXAddVertexToAABB ( item.boundsWorld, boundsLocal.max.x, boundsLocal.min.y, boundsLocal.max.z );

				GXAABB newBoundsLocal = boundsLocal;
				newBoundsLocal.min.y -= itemHeight;
				UpdateBoundsWorld ( newBoundsLocal );
			}

			items.SetValue ( totalItems, &item );
			totalItems++;

			if ( totalItems != 0 && renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_POPUP_SET_ITEM_HEIGHT:
		{
			const GXFloat* newItemHeight = (const GXFloat*)data;
			itemHeight = *newItemHeight;

			GXUInt totalItems = items.GetLength ();

			if ( totalItems != 0 )
			{
				GXUIPopupItem* itemStorage = (GXUIPopupItem*)items.GetData ();

				GXAABB newBoundsLocal = boundsLocal;
				newBoundsLocal.min.y = newBoundsLocal.max.y;

				for ( GXUInt i = 0; i < totalItems; i++ )
				{
					GXSetAABBEmpty ( itemStorage[ i ].boundsWorld );
					GXAddVertexToAABB ( itemStorage[ i ].boundsWorld, newBoundsLocal.max.x, newBoundsLocal.min.y, newBoundsLocal.max.z );
					newBoundsLocal.min.y -= itemHeight;
					GXAddVertexToAABB ( itemStorage[ i ].boundsWorld, newBoundsLocal.min.x, newBoundsLocal.min.y, newBoundsLocal.min.z );
				}

				UpdateBoundsWorld ( newBoundsLocal );
			}

			if ( totalItems != 0 && renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_POPUP_ENABLE_ITEM:
		{
			const GXUByte* itemIndex = (const GXUByte*)data;
			if ( (GXUInt)( *itemIndex ) >= items.GetLength () ) break;

			GXUIPopupItem* itemStorage = (GXUIPopupItem*)items.GetData ();
			if ( itemStorage[ *itemIndex ].isActive ) break;

			itemStorage[ *itemIndex ].isActive = GX_TRUE;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_POPUP_DISABLE_ITEM:
		{
			const GXUByte* itemIndex = (const GXUByte*)data;
			if ( (GXUInt)( *itemIndex ) >= items.GetLength () ) break;

			GXUIPopupItem* itemStorage = (GXUIPopupItem*)items.GetData ();
			if ( !itemStorage[ *itemIndex ].isActive ) break;

			itemStorage[ *itemIndex ].isActive = GX_FALSE;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_POPUP_SHOW:
		{
			GXWidget** owner = (GXWidget**)data;
			this->owner = *owner;
			GXWidget::Show ();
		}
		break;

		case GX_MSG_LMBUP:
		{
			if ( selectedItemIndex != GX_UI_POPUP_INVALID_INDEX )
			{
				GXUIPopupItem* itemStorage = (GXUIPopupItem*)items.GetData ();
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
			const GXVec2* pos = (const GXVec2*)data;
			GXUIPopupItem* itemStorage = (GXUIPopupItem*)items.GetData ();
			GXUByte totalItems = (GXUByte)items.GetLength ();
			
			GXUByte mouseOverItem = 0;
			for ( ; mouseOverItem < totalItems; mouseOverItem++ )
				if ( GXIsOverlapedAABBVec3 ( itemStorage[ mouseOverItem ].boundsWorld, pos->x, pos->y, 0.0f ) ) break;

			if ( selectedItemIndex == GX_UI_POPUP_INVALID_INDEX && !itemStorage[ mouseOverItem ].isActive ) break;
			if ( selectedItemIndex == mouseOverItem && itemStorage[ mouseOverItem ].isActive ) break;

			selectedItemIndex = itemStorage[ mouseOverItem ].isActive ? mouseOverItem : GX_UI_POPUP_INVALID_INDEX;
			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			if ( selectedItemIndex != GX_UI_POPUP_INVALID_INDEX )
			{
				selectedItemIndex = GX_UI_POPUP_INVALID_INDEX;
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
			const GXAABB* newBoundsLocal = (const GXAABB*)data;
			GXFloat newWidth = GXGetAABBWidth ( *newBoundsLocal );
			GXVec2 newPosition = GXCreateVec2 ( newBoundsLocal->min.x, newBoundsLocal->min.y );

			GXUInt totalItems = items.GetLength ();
			GXFloat top = newPosition.y + totalItems * itemHeight;
			GXUIPopupItem* itemStorage = (GXUIPopupItem*)items.GetData ();
			for ( GXUInt i = 0; i < totalItems; i++ )
			{
				GXSetAABBEmpty ( itemStorage[ i ].boundsWorld );
				GXAddVertexToAABB ( itemStorage[ i ].boundsWorld, newPosition.x, top - itemHeight, -1.0f );
				GXAddVertexToAABB ( itemStorage[ i ].boundsWorld, newPosition.x + newWidth, top, 1.0f );
				top -= itemHeight;
			}
			
			GXAABB bounds;
			GXAddVertexToAABB ( bounds, newPosition.x, newPosition.y, -1.0f );
			GXAddVertexToAABB ( bounds, newPosition.x + newWidth, newPosition.y + totalItems * itemHeight, 1.0f );
			GXWidget::OnMessage ( message, &bounds );
		}
		break;

		case GX_MSG_HIDE:
			selectedItemIndex = GX_UI_POPUP_INVALID_INDEX;
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
	return items.GetLength ();
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

	GXUIPopupItem* itemStorage = (GXUIPopupItem*)items.GetData ();
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
	return GXGetAABBWidth ( boundsLocal );
}

GXVoid GXUIPopup::Show ( GXWidget* owner )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_POPUP_SHOW, &owner, sizeof ( GXWidget* ) );
}
