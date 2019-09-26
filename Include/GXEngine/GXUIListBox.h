// version 1.6

#ifndef GX_UI_LIST_BOX
#define GX_UI_LIST_BOX


#include "GXWidget.h"


class GXUIListBox;
typedef GXVoid ( GXCALL* GXUIListBoxItemDestructorHandler ) ( GXVoid* itemData );
typedef GXVoid ( GXCALL* GXUIListBoxItemOnItemSelectHandler ) ( GXVoid* context, GXUIListBox& listBox, const GXVoid* item );
typedef GXVoid ( GXCALL* GXUIListBoxItemOnItemDoubleClickHandler ) ( GXVoid* handler, GXUIListBox& listBox, const GXVoid* item );

typedef GXVoid ( GXUIListBox::* GXUIListBoxOnMessageHandler ) ( const GXVoid* data );

class GXUIListBoxMessageHandlerNode final: public GXUIWidgetMessageHandlerNode
{
    private:
        GXUIListBoxOnMessageHandler     _handler;

    public:
        GXUIListBoxMessageHandlerNode ();

        // Special probe constructor.
        explicit GXUIListBoxMessageHandlerNode ( eGXUIMessage message );
        ~GXUIListBoxMessageHandlerNode () override;

        GXVoid HandleMassage ( const GXVoid* data ) override;

        GXVoid Init ( GXUIListBox &listBox, eGXUIMessage message, GXUIListBoxOnMessageHandler handler );

    private:
        GXUIListBoxMessageHandlerNode ( const GXUIListBoxMessageHandlerNode &other ) = delete;
        GXUIListBoxMessageHandlerNode& operator = ( const GXUIListBoxMessageHandlerNode &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

struct GXUIListBoxItem final : public GXMemoryInspector
{
    GXUIListBoxItem*    _next;
    GXUIListBoxItem*    _prev;

    GXBool              _isSelected;
    GXBool              _isHighlighted;

    GXVoid*             _data;

    GXUIListBoxItem ();
};

//---------------------------------------------------------------------------------------------------------------------

class GXUIListBox final : public GXWidget
{
    private:
        GXUIListBoxItem*                            _itemHead;
        GXUIListBoxItem*                            _itemTail;
        GXFloat                                     _itemHeight;
        GXUInt                                      _totalItems;
        GXUIListBoxItemDestructorHandler            _destroyItem;
        GXUIListBoxItemOnItemSelectHandler          _onItemSelected;
        GXUIListBoxItemOnItemDoubleClickHandler     _onItemDoubleClicked;
        GXVoid*                                     _itemSelectedContext;
        GXVoid*                                     _itemDoubleClickedContext;

        GXFloat                                     _viewportOffset;
        GXFloat                                     _viewportSize;

        GXUIListBoxMessageHandlerNode               _messageHandlers[ 14u ];

    public:
        explicit GXUIListBox ( GXWidget* parent, GXUIListBoxItemDestructorHandler itemDestructor );
        ~GXUIListBox () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

        GXVoid AddItem ( GXVoid* itemData );
        GXVoid AddItems ( GXVoid** itemData, GXUInt items );
        GXVoid RemoveItem ( GXUInt itemIndex );
        GXVoid RemoveAllItems ();
        GXUIListBoxItem* GetItems () const;
        GXVoid* GetSelectedItem () const;
        GXUInt GetTotalItems () const;
        GXBool IsItemVisible ( GXUInt itemIndex ) const;
        GXFloat GetItemLocalOffsetY ( GXUInt itemIndex ) const;    // From widget bottom edge

        GXVoid Redraw ();

        GXVoid SetOnItemSelectedCallback ( GXVoid* context, GXUIListBoxItemOnItemSelectHandler callback );
        GXVoid SetOnItemDoubleClickedCallback ( GXVoid* context, GXUIListBoxItemOnItemDoubleClickHandler callback );

        GXFloat GetViewportOffset () const;
        GXFloat GetViewportSize () const;
        GXVoid SetViewportOffset ( GXFloat offset );
        GXVoid SetViewportOffsetImmediately ( GXFloat offset );

        GXVoid SetItemHeight ( GXFloat height );
        GXFloat GetItemHeight () const;

    private:
        GXUIListBoxItem* FindItem ( const GXVec2 &mousePosition );
        GXUIListBoxItem* GetHighlightedItem () const;
        GXBool ResetHighlight ( const GXVec2 &mousePosition );    // Return true if need render update
        GXBool IsAbleToScroll () const;
        GXFloat GetTotalHeight () const;

        GXVoid InitMessageHandlers ();

        // Message handlers
        GXVoid OnDoubleClick ( const GXVoid* data );
        GXVoid OnLMBDown ( const GXVoid* data );
        GXVoid OnListBoxAddItem ( const GXVoid* data );
        GXVoid OnListBoxAddItems ( const GXVoid* data );
        GXVoid OnListBoxRemoveAllItems ( const GXVoid* data );
        GXVoid OnListBoxRemoveItem ( const GXVoid* data );
        GXVoid OnListBoxSetItemHeight ( const GXVoid* data );
        GXVoid OnListBoxSetViewportOffset ( const GXVoid* data );
        GXVoid OnMouseLeave ( const GXVoid* data );
        GXVoid OnMouseMove ( const GXVoid* data );
        GXVoid OnMouseOver ( const GXVoid* data );
        GXVoid OnRedraw ( const GXVoid* data );
        GXVoid OnResize ( const GXVoid* data );
        GXVoid OnScroll ( const GXVoid* data );

        GXUIListBox () = delete;
        GXUIListBox ( const GXUIListBox &other ) = delete;
        GXUIListBox& operator = ( const GXUIListBox &other ) = delete;
};


#endif // GX_UI_LIST_BOX
