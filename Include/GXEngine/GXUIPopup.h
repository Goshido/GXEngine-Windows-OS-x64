// version 1.6

#ifndef GX_UI_POPUP
#define GX_UI_POPUP


#include "GXWidget.h"
#include <GXCommon/GXMemory.h>


#define GX_UI_POPUP_INVALID_INDEX 0xFFu

//---------------------------------------------------------------------------------------------------------------------

typedef GXVoid ( GXCALL* GXUIPopupActionHandler ) ( GXVoid* context );

class GXUIPopup;
typedef GXVoid ( GXUIPopup::* GXUIPopupOnMessageHandler ) ( const GXVoid* data );

class GXUIPopupMessageHandlerNode final: public GXUIWidgetMessageHandlerNode
{
    private:
        GXUIPopupOnMessageHandler       _handler;

    public:
        GXUIPopupMessageHandlerNode ();

        // Special probe constructor.
        explicit GXUIPopupMessageHandlerNode ( eGXUIMessage message );
        ~GXUIPopupMessageHandlerNode () override;

        GXVoid HandleMassage ( const GXVoid* data ) override;

        GXVoid Init ( GXUIPopup &popup, eGXUIMessage message, GXUIPopupOnMessageHandler handler );

    private:
        GXUIPopupMessageHandlerNode ( const GXUIPopupMessageHandlerNode &other ) = delete;
        GXUIPopupMessageHandlerNode& operator = ( const GXUIPopupMessageHandlerNode &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

class GXUIPopup final : public GXWidget
{
    private:
        GXDynamicArray                  _items;
        GXUByte                         _selectedItemIndex;
        GXFloat                         _itemHeight;
        GXWidget*                       _owner;

        GXUIPopupMessageHandlerNode     _messageHandlers[ 10u ];

    public:
        explicit GXUIPopup ( GXWidget* parent );
        ~GXUIPopup () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

        GXVoid AddItem ( GXVoid* context, GXUIPopupActionHandler action );
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
        GXVoid InitMessageHandlers ();

        // Message handlers
        GXVoid OnHide ( const GXVoid* data );
        GXVoid OnLMBUp ( const GXVoid* data );
        GXVoid OnMouseLeave ( const GXVoid* data );
        GXVoid OnMouseMove ( const GXVoid* data );
        GXVoid OnPopupAddItem ( const GXVoid* data );
        GXVoid OnPopupDisableItem ( const GXVoid* data );
        GXVoid OnPopupEnableItem ( const GXVoid* data );
        GXVoid OnPopupSetItemHeight ( const GXVoid* data );
        GXVoid OnPopupShow ( const GXVoid* data );
        GXVoid OnResize ( const GXVoid* data );

        GXUIPopup ( const GXUIPopup &other ) = delete;
        GXUIPopup& operator = ( const GXUIPopup &other ) = delete;
};


#endif // GX_UI_POPUP
