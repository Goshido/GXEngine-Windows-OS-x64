#ifndef EM_UI_MENU
#define EM_UI_MENU


#include "EMUI.h"
#include <GXEngine/GXUIMenu.h>


class EMUIMenu : public EMUI
{
	friend class EMUIMenuRenderer;

	private:
		GXUIMenu*		menuWidget;
		GXDynamicArray	submenuWidgets;

	public:
		EMUIMenu ();
		~EMUIMenu () override;

		GXVoid OnDrawMask () override;

		GXVoid AddItem ( const GXWChar* caption );
		GXVoid RenameItem ( GXUByte item, const GXWChar* caption );

		GXVoid AddSubitem ( GXUByte item, const GXWChar* caption, const GXWChar* hotkey, PFNGXONMENUITEMPROC onMouseButton );
		GXVoid RenameSubitem ( GXUByte item, GXUByte subitem, const GXWChar* caption, const GXWChar* hotkey );

		GXVoid SetLocation ( GXFloat x, GXFloat y );
		GXVoid SetLayer ( GXFloat z );

	private:
		GXVoid OnChangeLayer ( GXFloat z );
		static GXVoid GXCALL OnShowSubmenu ( GXVoid* menuHandler, GXUByte item );
};


#endif //EM_UI_MENU
