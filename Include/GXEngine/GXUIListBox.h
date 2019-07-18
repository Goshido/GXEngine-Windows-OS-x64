// version 1.5

#ifndef GX_UI_LIST_BOX
#define GX_UI_LIST_BOX


#include "GXWidget.h"


class GXUIListBox;
typedef GXVoid ( GXCALL* GXUIListBoxItemDestructorHandler ) ( GXVoid* itemData );
typedef GXVoid ( GXCALL* GXUIListBoxItemOnItemSelectHandler ) ( GXVoid* context, GXUIListBox& listBox, const GXVoid* item );
typedef GXVoid ( GXCALL* GXUIListBoxItemOnItemDoubleClickHandler ) ( GXVoid* handler, GXUIListBox& listBox, const GXVoid* item );

struct GXUIListBoxItem final
{
    GXUIListBoxItem*    _next;
    GXUIListBoxItem*    _prev;

    GXBool              _isSelected;
    GXBool              _isHighlighted;

    GXVoid*             _data;
};

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

        GXUIListBox () = delete;
        GXUIListBox ( const GXUIListBox &other ) = delete;
        GXUIListBox& operator = ( const GXUIListBox &other ) = delete;
};


#endif // GX_UI_LIST_BOX
