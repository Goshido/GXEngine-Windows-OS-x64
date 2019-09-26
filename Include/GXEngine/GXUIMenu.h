// version 1.6

#ifndef GX_UI_MENU_EXT
#define GX_UI_MENU_EXT


#include "GXUIPopup.h"
#include <GXCommon/GXStrings.h>


#define GX_UI_MENU_INVALID_INDEX 0xFFu

//---------------------------------------------------------------------------------------------------------------------

class GXUIMenu;
typedef GXVoid ( GXUIMenu::* GXUIMenuOnMessageHandler ) ( const GXVoid* data );

class GXUIMenuMessageHandlerNode final : public GXUIWidgetMessageHandlerNode
{
    private:
        GXUIMenuOnMessageHandler     _handler;

    public:
        GXUIMenuMessageHandlerNode ();

        // Special probe constructor.
        explicit GXUIMenuMessageHandlerNode ( eGXUIMessage message );
        ~GXUIMenuMessageHandlerNode () override;

        GXVoid HandleMassage ( const GXVoid* data ) override;

        GXVoid Init ( GXUIMenu &menu, eGXUIMessage message, GXUIMenuOnMessageHandler handler );

    private:
        GXUIMenuMessageHandlerNode ( const GXUIMenuMessageHandlerNode &other ) = delete;
        GXUIMenuMessageHandlerNode& operator = ( const GXUIMenuMessageHandlerNode &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

class GXUIMenu final : public GXWidget
{
    private:
        GXDynamicArray                  _items;
        GXUByte                         _selectedItemIndex;
        GXUByte                         _highlightedItemIndex;

        GXUIMenuMessageHandlerNode      _messageHandlers[ 6u ];
        const GXString                  _defaultText;

    public:
        explicit GXUIMenu ( GXWidget* parent );
        ~GXUIMenu () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

        GXVoid AddItem ( const GXString &name, GXFloat itemWidth, GXUIPopup* popup );

        GXUPointer GetTotalItems () const;
        const GXString& GetItemName ( GXUPointer itemIndex ) const;
        GXFloat GetItemOffset ( GXUPointer itemIndex ) const;
        GXFloat GetItemWidth ( GXUPointer itemIndex ) const;

        GXUByte GetSelectedItemIndex () const;
        GXUByte GetHighlightedItemIndex () const;

    private:
        GXVoid InitMessageHandlers ();

        GXVoid OnLMBDown ( const GXVoid* data );
        GXVoid OnMenuAddItem ( const GXVoid* data );
        GXVoid OnMouseLeave ( const GXVoid* data );
        GXVoid OnMouseMove ( const GXVoid* data );
        GXVoid OnPopupClosed ( const GXVoid* data );
        GXVoid OnResize ( const GXVoid* data );

        GXUIMenu ( const GXUIMenu &other ) = delete;
        GXUIMenu& operator = ( const GXUIMenu &other ) = delete;
};


#endif // GX_UI_MENU_EXT
