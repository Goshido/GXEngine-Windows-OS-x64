//version 1.0

#include <GXEngine/GXUIMenuExt.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXStrings.h>


#define GX_ANY_HEIGHT 1.0f


struct GXUIMenuItem
{
	GXWChar*	name;
	GXAABB		boundsLocal;
	GXUIPopup*	popup;
};

GXUIMenuExt::GXUIMenuExt ( GXWidget* parent ):
GXWidget ( parent ), items ( sizeof ( GXUIMenuItem ) )
{
	selectedItemIndex = GX_UI_MENU_INVALID_INDEX;
}

GXUIMenuExt::~GXUIMenuExt ()
{
	GXUInt totalItems = items.GetLength ();
	GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();

	for ( GXUByte i = 0; i < totalItems; i++ )
		GXSafeFree ( itemStorage[ i ].name );
}

GXVoid GXUIMenuExt::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_MENY_ADD_ITEM:
		{
			const GXUIMenuItem* item = (const GXUIMenuItem*)data;
			//TODO
		}
		break;

		case GX_MSG_LMBDOWN:
		{
			//TODO
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = (const GXVec2*)data;
			//TODO
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			//TODO
		}
		break;

		case GX_MSG_RESIZE:
		{
			const GXAABB* newBoundsLocal = (const GXAABB*)data;
			//TODO
		}
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUIMenuExt::AddItem ( const GXWChar* name, GXFloat itemWidth, GXUIPopup* popup )
{
	if ( !name ) return;

	GXUIMenuItem item;

	GXUInt size = ( GXWcslen ( name ) + 1 ) * sizeof ( GXWChar );
	item.name = (GXWChar*)gx_ui_MessageBuffer->Allocate ( size );
	memcpy ( item.name, name, size );
	GXAddVertexToAABB ( item.boundsLocal, 0.0f, 0.0f, -1.0f );
	GXAddVertexToAABB ( item.boundsLocal, itemWidth, GX_ANY_HEIGHT, 1.0f );
	item.popup = popup;

	GXTouchSurface::GetInstance ()->SendMessage ( this, GX_MSG_MENY_ADD_ITEM, &item, sizeof ( GXUIMenuItem ) );
}

GXUByte GXUIMenuExt::GetTotalItems () const
{
	return (GXUByte)items.GetLength ();
}

const GXWChar* GXUIMenuExt::GetItemName ( GXUByte itemIndex ) const
{
	GXUInt totalItems = items.GetLength ();
	GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();

	if ( (GXUByte)totalItems >= itemIndex ) return nullptr;

	return itemStorage[ itemIndex ].name;
}

const GXAABB& GXUIMenuExt::GetItemBoundsLocal ( GXUByte itemIndex ) const
{
	GXUInt totalItems = items.GetLength ();
	GXUIMenuItem* itemStorage = (GXUIMenuItem*)items.GetData ();

	if ( (GXUByte)totalItems >= itemIndex ) return boundsLocal;

	return itemStorage[ itemIndex ].boundsLocal;
}

GXUByte GXUIMenuExt::GetSelectedItemIndex () const
{
	return selectedItemIndex;
}
