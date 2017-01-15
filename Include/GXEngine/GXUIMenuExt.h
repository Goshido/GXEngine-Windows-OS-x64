//version 1.0

#ifndef GX_UI_MENU_EXT
#define GX_UI_MENU_EXT


#include "GXUIPopup.h"


#define GX_UI_MENU_INVALID_INDEX 0xFF


class GXUIMenuExt : public GXWidget
{
	private:
		GXDynamicArray		items;
		GXUByte				selectedItemIndex;

	public:
		explicit GXUIMenuExt ( GXWidget* parent );
		~GXUIMenuExt () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXVoid AddItem ( const GXWChar* name, GXFloat itemWidth, GXUIPopup* popup );

		GXUByte GetTotalItems () const;
		const GXWChar* GetItemName ( GXUByte itemIndex ) const;
		const GXAABB& GetItemBoundsLocal ( GXUByte itemIndex ) const;

		GXUByte GetSelectedItemIndex () const;
};


#endif //GX_UI_MENU_EXT
