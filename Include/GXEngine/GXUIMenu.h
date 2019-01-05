// version 1.4

#ifndef GX_UI_MENU_EXT
#define GX_UI_MENU_EXT


#include "GXUIPopup.h"


#define GX_UI_MENU_INVALID_INDEX 0xFFu

//---------------------------------------------------------------------------------------------------------------------

class GXUIMenu final : public GXWidget
{
    private:
        GXDynamicArray      items;
        GXUByte             selectedItemIndex;
        GXUByte             highlightedItemIndex;

    public:
        explicit GXUIMenu ( GXWidget* parent );
        ~GXUIMenu () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

        GXVoid AddItem ( const GXWChar* name, GXFloat itemWidth, GXUIPopup* popup );

        GXUByte GetTotalItems () const;
        const GXWChar* GetItemName ( GXUByte itemIndex ) const;
        GXFloat GetItemOffset ( GXUByte itemIndex ) const;
        GXFloat GetItemWidth ( GXUByte itemIndex ) const;

        GXUByte GetSelectedItemIndex () const;
        GXUByte GetHighlightedItemIndex () const;

    private:
        GXUIMenu ( const GXUIMenu &other ) = delete;
        GXUIMenu& operator = ( const GXUIMenu &other ) = delete;
};


#endif // GX_UI_MENU_EXT
