// version 1.0

#ifndef GX_UI_POPUP
#define GX_UI_POPUP


#include "GXWidget.h"
#include <GXCommon/GXMemory.h>


#define GX_UI_POPUP_INVALID_INDEX 0xFF


typedef GXVoid ( GXCALL* PFNGXONUIPOPUPACTIONPROC ) ( GXVoid* handler );


class GXUIPopup : public GXWidget
{
	private:
		GXDynamicArray		items;
		GXUByte				selectedItemIndex;
		GXFloat				itemHeight;
		GXWidget*			owner;

	public:
		explicit GXUIPopup ( GXWidget* parent );
		~GXUIPopup () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXVoid AddItem ( GXVoid* handler, PFNGXONUIPOPUPACTIONPROC action );
		GXUByte GetTotalItems () const;

		GXVoid EnableItem ( GXUByte itemIndex );
		GXVoid DisableItem ( GXUByte itemIndex );
		GXBool IsItemActive ( GXUByte itemIndex ) const;

		GXUByte GetSelectedItemIndex () const;

		GXVoid SetItemHeight ( GXFloat height );
		GXFloat GetItemHeight () const;

		GXFloat GetItemWidth () const;

		GXVoid Show ( GXWidget* currentOwner );
};


#endif // GX_UI_POPUP
