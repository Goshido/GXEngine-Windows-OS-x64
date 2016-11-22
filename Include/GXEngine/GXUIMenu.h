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
		~GXUIMenu () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXVoid AddItem ();
		GXVoid ResizeItem ( GXUByte item, GXFloat width );
		GXUByte GetTotalItems () const;
		const GXAABB& GetItemBounds ( GXUByte item ) const;

		GXVoid Redraw ();

		GXBool IsItemPressed ( GXUByte item ) const;
		GXBool IsItemHighlighted ( GXUByte item ) const;
};

//-----------------------------------------------------------------------------------

class GXUISubmenu : public GXWidget
{
	private:
		GXDynamicArray		items;
		GXFloat				itemHeight;

	public:
		GXUISubmenu ( GXWidget* parent );
		~GXUISubmenu () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXVoid AddItem ( PFNGXONMENUITEMPROC callback );
		GXVoid SetItemHeight ( GXFloat height );
		GXUByte GetTotalItems () const;
		GXFloat GetItemHeight () const;

		GXVoid Redraw ();

		GXBool IsItemPressed ( GXUByte item ) const;
		GXBool IsItemHighlighted ( GXUByte item ) const;
};


#endif //GX_UI_MENU
