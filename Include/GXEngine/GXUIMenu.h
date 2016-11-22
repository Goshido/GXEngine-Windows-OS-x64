//version 1.0

#ifndef GX_UI_MENU
#define GX_UI_MENU


#include "GXWidget.h"
#include "GXUICommon.h"
#include <GXCommon/GXMemory.h>


typedef GXVoid ( GXCALL* PFNGXONSHOWSUBMENUPROC ) ( GXVoid* menuHandler, GXUByte item );
typedef GXVoid ( GXCALL* PFNGXONMENUITEMPROC ) ( GXFloat x, GXFloat y, eGXMouseButtonState state );


class GXUIMenu : public GXWidget
{
	private:
		GXDynamicArray			items;
		GXVoid*					menuHandler;
		PFNGXONSHOWSUBMENUPROC	OnShowSubmenu;

	public:
		GXUIMenu ( GXWidget* parent, GXVoid* menuHandler, PFNGXONSHOWSUBMENUPROC onShowSubmenu );
		virtual ~GXUIMenu ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid AddItem ();
		GXVoid ResizeItem ( GXUByte item, GXFloat width );
		GXUByte GetTotalItems ();
		const GXAABB& GetItemBounds ( GXUByte item );

		GXVoid Redraw ();

		GXBool IsItemPressed ( GXUByte item );
		GXBool IsItemHighlighted ( GXUByte item );
};

//-----------------------------------------------------------------------------------

class GXUISubmenu : public GXWidget
{
	private:
		GXDynamicArray		items;
		GXFloat				itemHeight;

	public:
		GXUISubmenu ( GXWidget* parent );
		virtual ~GXUISubmenu ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid AddItem ( PFNGXONMENUITEMPROC callback );
		GXVoid SetItemHeight ( GXFloat height );
		GXUByte GetTotalItems ();
		GXFloat GetItemHeight ();

		GXVoid Redraw ();

		GXBool IsItemPressed ( GXUByte item );
		GXBool IsItemHighlighted ( GXUByte item );
};


#endif //GX_UI_MENU
