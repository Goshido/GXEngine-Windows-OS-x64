// version 1.5

#include <GXEngine/GXUIListBox.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine_Editor_Mobile/EMUIFileListBox.h>		// TODO fix this!!

GX_DISABLE_COMMON_WARNINGS

#include <float.h>

GX_RESTORE_WARNING_STATE


#define GX_UI_DEFAULT_ITEM_HEIGHT		0.55f

//---------------------------------------------------------------------------------------------------------------------

struct GXUIListBoxRawItem final
{
    GXUIListBoxRawItem*     _next;
    GXVoid*                 _data;
};

//---------------------------------------------------------------------------------------------------------------------

GXUIListBox::GXUIListBox ( GXWidget* parent, PFNGXUILISTBOXITEMDESTRUCTORPROC itemDestructor ):
    GXWidget ( parent ),
    _itemHead ( nullptr ),
    _itemTail ( nullptr ),
    _itemHeight ( GX_UI_DEFAULT_ITEM_HEIGHT * gx_ui_Scale ),
    _totalItems ( 0u ),
    _destroyItem ( itemDestructor ),
    _onItemSelected ( nullptr ),
    _onItemDoubleClicked ( nullptr ),
    _itemSelectedContext ( nullptr ),
    _itemDoubleClickedContext ( nullptr ),
    _viewportOffset ( 0.0f ),
    _viewportSize ( 1.0f )
{
    // NOTHING
}

GXUIListBox::~GXUIListBox ()
{
    while ( _itemHead )
    {
        GXUIListBoxItem* p = _itemHead;
        _itemHead = _itemHead->_next;
        _destroyItem ( p->_data );
        delete p;
    }
}

GXVoid GXUIListBox::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    if ( message == eGXUIMessage::MouseOver || message == eGXUIMessage::MouseMove )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );

        GXBool isNeedRenderUpdate = ResetHighlight ( *pos );

        if ( isNeedRenderUpdate && _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::MouseLeave )
    {
        for ( GXUIListBoxItem* p = _itemHead; p; p = p->_next )
        {
            if ( p->_isHighlighted )
            {
                p->_isHighlighted = GX_FALSE;

                if ( _renderer )
                    _renderer->OnUpdate ();

                break;
            }
        }

        return;
    }

    if ( message == eGXUIMessage::Scroll )
    {
        const GXVec3* scroll = static_cast<const GXVec3*> ( data );

        if ( !IsAbleToScroll () ) return;

        GXFloat step = 1.0f / static_cast<GXFloat> ( _totalItems );
        GXFloat offset = GXClampf ( _viewportOffset - scroll->GetZ () * step, 0.0f, 1.0f - _viewportSize );

        _viewportOffset = offset;

        GXVec2 pos ( scroll->GetX (), scroll->GetZ () );
        OnMessage ( eGXUIMessage::MouseMove, &pos );

        return;
    }

    if ( message == eGXUIMessage::LMBDown )
    {
        GXUIListBoxItem* item = GetHighlightedItem ();
        GXUIListBoxItem* selected = nullptr;

        for ( selected = _itemHead; selected; selected = selected->_next )
        {
            if ( selected->_isSelected )
                break;
        }

        if ( selected == item ) return;
        
        if ( selected )
            selected->_isSelected = GX_FALSE;

        if ( item )
            item->_isSelected = GX_TRUE;

        if ( _onItemSelected )
            _onItemSelected ( _itemSelectedContext, *this, item->_data );

        if ( _renderer )
            _renderer->OnUpdate ();
        
        return;
    }

    if ( message == eGXUIMessage::DoubleClick )
    {
        GXUIListBoxItem* item = GetHighlightedItem ();

        if ( !item ) return;
        if ( !_onItemDoubleClicked ) return;

        _onItemDoubleClicked ( _itemDoubleClickedContext, *this, item->_data );
        return;
    }

    if ( message == eGXUIMessage::ListBoxAddItem )
    {
        GXVoid* pointer = const_cast<GXVoid*> ( data );
        GXVoid** itemData = reinterpret_cast<GXVoid**> ( pointer );

        GXUIListBoxItem* item = new GXUIListBoxItem ();
        item->_data = *itemData;
        item->_isSelected = item->_isHighlighted = GX_FALSE;

        item->_next = nullptr;
        item->_prev = _itemTail;

        if ( _itemTail )
            _itemTail->_next = item;
        else
            _itemHead = item;

        _itemTail = item;

        ++_totalItems;
        _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::ListBoxAddItems )
    {
        GXVoid* pointer = const_cast<GXVoid*> ( data );
        GXUIListBoxRawItem** list = reinterpret_cast<GXUIListBoxRawItem**> ( pointer );

        for ( GXUIListBoxRawItem* p = *list; p; p = p->_next )
        {
            GXUIListBoxItem* item = new GXUIListBoxItem ();
            item->_data = p->_data;
            item->_isSelected = item->_isHighlighted = GX_FALSE;

            item->_next = nullptr;
            item->_prev = _itemTail;

            if ( _itemTail )
                _itemTail->_next = item;
            else
                _itemHead = item;

            _itemTail = item;

            ++_totalItems;
        }

        _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::ListBoxRemoveAllItems )
    {
        while ( _itemHead )
        {
            GXUIListBoxItem* p = _itemHead;
            _itemHead = _itemHead->_next;
            _destroyItem ( p->_data );
            delete p;
        }

        _itemTail = nullptr;
        _totalItems = 0;

        _viewportOffset = 0.0f;
        _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::ListBoxRemoveItem )
    {
        const GXUInt* itemIndex = static_cast<const GXUInt*> ( data );

        GXUIListBoxItem* p = _itemHead;

        for ( GXUInt i = 0; i < *itemIndex; ++i )
            p = p->_next;

        if ( p->_next )
            p->_next->_prev = p->_prev;
        else
            _itemTail = p->_prev;

        if ( p->_prev )
            p->_prev->_next = p->_next;
        else
            _itemHead = p->_next;

        _destroyItem ( p->_data );
        --_totalItems;

        delete p;

        _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::ListBoxSetViewportOffset )
    {
        const GXFloat* offset = static_cast<const GXFloat*> ( data );
        _viewportOffset = *offset;

        _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::ListBoxSetItemHeight )
    {
        const GXFloat* height = static_cast<const GXFloat*> ( data );
        _itemHeight = *height;

        _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::Resize )
    {
        const GXAABB* newBoundsLocal = static_cast<const GXAABB*> ( data );
        UpdateBoundsWorld ( *newBoundsLocal );
        _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::Redraw )
    {
        const GXVec2& position = GXTouchSurface::GetInstance ().GetMousePosition ();
        OnMessage ( eGXUIMessage::MouseMove, &position );

        return;
    }

    GXWidget::OnMessage ( message, data );
}

GXVoid GXUIListBox::AddItem ( GXVoid* itemData )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ListBoxAddItem, &itemData, sizeof ( GXVoid* ) );
}

GXVoid GXUIListBox::AddItems ( GXVoid** itemData, GXUInt items )
{
    if ( items == 0 || !itemData ) return;

    GXUIListBoxRawItem* list = static_cast<GXUIListBoxRawItem*> ( gx_ui_MessageBuffer->Allocate ( items * sizeof ( GXUIListBoxRawItem ) ) );
    GXUIListBoxRawItem* p = list;

    GXUInt limit = items - 1;

    for ( GXUInt i = 0u; i < limit; ++i )
    {
        p->_data = itemData[ i ];
        p->_next = p + 1;
        ++p;
    }

    p->_data = itemData[ limit ];
    p->_next = nullptr;

    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ListBoxAddItems, &list, sizeof ( GXUIListBoxRawItem* ) );
}

GXVoid GXUIListBox::RemoveItem ( GXUInt itemIndex )
{
    if ( itemIndex >= _totalItems ) return;

    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ListBoxRemoveItem, &itemIndex, sizeof ( GXUInt ) );
}

GXVoid GXUIListBox::RemoveAllItems ()
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ListBoxRemoveAllItems, nullptr, 0 );
}

GXVoid* GXUIListBox::GetSelectedItem () const
{
    for ( GXUIListBoxItem* p = _itemHead; p; p = p->_next )
    {
        if ( p->_isSelected )
            return p->_data;
    }

    return nullptr;
}

GXUIListBoxItem* GXUIListBox::GetItems () const
{
    return _itemHead;
}

GXUInt GXUIListBox::GetTotalItems () const
{
    return _totalItems;
}

GXFloat GXUIListBox::GetViewportOffset () const
{
    return _viewportOffset;
}

GXBool GXUIListBox::IsItemVisible ( GXUInt itemIndex ) const
{
    if ( itemIndex >= _totalItems ) return GX_FALSE;

    GXFloat itemTop = itemIndex * _itemHeight;
    GXFloat itemBottom = itemTop + _itemHeight;

    GXFloat totalHeight = GetTotalHeight ();
    GXFloat viewportTop = totalHeight * _viewportOffset;
    GXFloat viewportBottom = viewportTop + _viewportSize * totalHeight;

    GXAABB item;
    item.AddVertex ( itemTop, 0.0f, 0.0f );
    item.AddVertex ( itemBottom, 1.0f, 1.0f );

    GXAABB viewport;
    viewport.AddVertex ( viewportTop, 0.0f, 0.0f );
    viewport.AddVertex ( viewportBottom, 1.0f, 1.0f );

    return item.IsOverlaped ( viewport );
}

GXFloat GXUIListBox::GetItemLocalOffsetY ( GXUInt itemIndex ) const
{
    if ( itemIndex >= _totalItems ) return FLT_MAX;

    GXFloat itemBottom = itemIndex * _itemHeight + _itemHeight;
    GXFloat viewportBottom = GetTotalHeight () * ( _viewportOffset + _viewportSize );

    return viewportBottom - itemBottom;
}

GXVoid GXUIListBox::Redraw ()
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Redraw, 0, 0 );
}

GXVoid GXUIListBox::SetOnItemSelectedCallback ( GXVoid* context, PFNGXUILISTBOXONITEMSELECTEDPROC callback )
{
    _itemSelectedContext = context;
    _onItemSelected = callback;
}

GXVoid GXUIListBox::SetOnItemDoubleClickedCallback ( GXVoid* context, PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC callback )
{
    _itemDoubleClickedContext = context;
    _onItemDoubleClicked = callback;
}

GXFloat GXUIListBox::GetViewportSize () const
{
    return _viewportSize;
}

GXVoid GXUIListBox::SetViewportOffset ( GXFloat offset )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ListBoxSetViewportOffset, &offset, sizeof ( GXFloat ) );
}

GXVoid GXUIListBox::SetViewportOffsetImmediately ( GXFloat offset )
{
    _viewportOffset = offset;

    _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

    if ( _renderer )
        _renderer->OnUpdate ();
}

GXVoid GXUIListBox::SetItemHeight ( GXFloat height )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ListBoxSetItemHeight, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIListBox::GetItemHeight () const
{
    return _itemHeight;
}

GXUIListBoxItem* GXUIListBox::FindItem ( const GXVec2 &mousePosition )
{
    GXFloat offset = GetTotalHeight () * _viewportOffset;
    GXUIListBoxItem* p = _itemHead;

    for ( GXUInt i = 0u; i < _totalItems; ++i )
    {
        GXAABB itemBounds;
        itemBounds.AddVertex ( _boundsWorld.min.GetX (), _boundsWorld.max.GetY () + offset - _itemHeight, _boundsWorld.min.GetZ () );
        itemBounds.AddVertex ( _boundsWorld.max.GetX (), _boundsWorld.max.GetY () + offset, _boundsWorld.max.GetZ () );

        if ( itemBounds.IsOverlaped ( mousePosition.GetX (), mousePosition.GetY (), 0.0f ) )
             return p;

        offset -= _itemHeight;
        p = p->_next;
    }

    return nullptr;
}

GXUIListBoxItem* GXUIListBox::GetHighlightedItem () const
{
    for ( GXUIListBoxItem* p = _itemHead; p; p = p->_next )
    {
        if ( !p->_isHighlighted ) continue;

        return p;
    }

    return nullptr;
}

GXBool GXUIListBox::ResetHighlight ( const GXVec2 &mousePosition )
{
    GXBool isNeedRenderUpdate = GX_FALSE;

    GXUIListBoxItem* currentItem = FindItem ( mousePosition );

    if ( currentItem && !currentItem->_isHighlighted )
    {
        currentItem->_isHighlighted = GX_TRUE;
        isNeedRenderUpdate = GX_TRUE;
    }

    for ( GXUIListBoxItem* p = _itemHead; p; p = p->_next )
    {
        if ( p->_isHighlighted && p != currentItem )
        {
            isNeedRenderUpdate = GX_TRUE;
            p->_isHighlighted = GX_FALSE;
        }
    }

    return isNeedRenderUpdate;
}

GXBool GXUIListBox::IsAbleToScroll () const
{
    return _totalItems * _itemHeight > _boundsLocal.GetHeight ();
}

GXFloat GXUIListBox::GetTotalHeight () const
{
    GXFloat totalHeight = _totalItems * _itemHeight;
    GXFloat viewportHeight = _boundsLocal.GetHeight ();

    return totalHeight < viewportHeight ? viewportHeight : totalHeight;
}
