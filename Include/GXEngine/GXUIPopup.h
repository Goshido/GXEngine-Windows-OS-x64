// version 1.5

#ifndef GX_UI_POPUP
#define GX_UI_POPUP


#include "GXWidget.h"
#include <GXCommon/GXMemory.h>


#define GX_UI_POPUP_INVALID_INDEX 0xFFu

//---------------------------------------------------------------------------------------------------------------------

typedef GXVoid ( GXCALL* PFNGXONUIPOPUPACTIONPROC ) ( GXVoid* handler );

class GXUIPopup final : public GXWidget
{
    private:
        GXDynamicArray      _items;
        GXUByte             _selectedItemIndex;
        GXFloat             _itemHeight;
        GXWidget*           _owner;

    public:
        explicit GXUIPopup ( GXWidget* parent );
        ~GXUIPopup () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

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

    private:
        GXUIPopup ( const GXUIPopup &other ) = delete;
        GXUIPopup& operator = ( const GXUIPopup &other ) = delete;
};


#endif // GX_UI_POPUP
