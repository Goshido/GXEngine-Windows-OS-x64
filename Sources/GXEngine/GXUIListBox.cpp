// version 1.6

#include <GXEngine/GXUIListBox.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine_Editor_Mobile/EMUIFileListBox.h>		// TODO fix this!!

GX_DISABLE_COMMON_WARNINGS

#include <float.h>

GX_RESTORE_WARNING_STATE


#define GX_UI_DEFAULT_ITEM_HEIGHT       0.55f

//---------------------------------------------------------------------------------------------------------------------

GXUIListBoxMessageHandlerNode::GXUIListBoxMessageHandlerNode ()
{
    // NOTHING
}

GXUIListBoxMessageHandlerNode::GXUIListBoxMessageHandlerNode ( eGXUIMessage message ):
    GXUIWidgetMessageHandlerNode ( message )
{
    // NOTHING
}

GXUIListBoxMessageHandlerNode::~GXUIListBoxMessageHandlerNode ()
{
    // NOTHING
}

GXVoid GXUIListBoxMessageHandlerNode::Init ( GXUIListBox &listBox, eGXUIMessage message, GXUIListBoxOnMessageHandler handler )
{
    _message = message;
    _handler = handler;
    _widget = &listBox;
}

GXVoid GXUIListBoxMessageHandlerNode::HandleMassage ( const GXVoid* data )
{
    GXUIListBox* listBox = static_cast<GXUIListBox*> ( _widget );

    // Note this is C++ syntax for invoke class method.
    ( listBox->*_handler ) ( data );
}

//---------------------------------------------------------------------------------------------------------------------

struct GXUIListBoxRawItem final
{
    GXUIListBoxRawItem*     _next;
    GXVoid*                 _data;
};

//---------------------------------------------------------------------------------------------------------------------

GXUIListBoxItem::GXUIListBoxItem ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXUIListBoxItem" )
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

GXUIListBox::GXUIListBox ( GXWidget* parent, GXUIListBoxItemDestructorHandler itemDestructor ):
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
    InitMessageHandlers ();
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
    GXUIListBoxMessageHandlerNode probe ( message );
    GXAVLTreeNode* findResult = _messageHandlerTree.Find ( probe );

    if ( !findResult )
    {
        GXWidget::OnMessage ( message, data );
        return;
    }

    GXUIListBoxMessageHandlerNode* targetHandler = static_cast<GXUIListBoxMessageHandlerNode*> ( findResult );
    targetHandler->HandleMassage ( data );
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

GXVoid GXUIListBox::SetOnItemSelectedCallback ( GXVoid* context, GXUIListBoxItemOnItemSelectHandler callback )
{
    _itemSelectedContext = context;
    _onItemSelected = callback;
}

GXVoid GXUIListBox::SetOnItemDoubleClickedCallback ( GXVoid* context, GXUIListBoxItemOnItemDoubleClickHandler callback )
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
        itemBounds.AddVertex ( _boundsWorld._min.GetX (), _boundsWorld._max.GetY () + offset - _itemHeight, _boundsWorld._min.GetZ () );
        itemBounds.AddVertex ( _boundsWorld._max.GetX (), _boundsWorld._max.GetY () + offset, _boundsWorld._max.GetZ () );

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

GXVoid GXUIListBox::InitMessageHandlers ()
{
    _messageHandlers[ 0u ].Init ( *this, eGXUIMessage::DoubleClick, &GXUIListBox::OnDoubleClick );
    _messageHandlerTree.Add ( _messageHandlers[ 0u ] );

    _messageHandlers[ 1u ].Init ( *this, eGXUIMessage::LMBDown, &GXUIListBox::OnLMBDown );
    _messageHandlerTree.Add ( _messageHandlers[ 1u ] );

    _messageHandlers[ 2u ].Init ( *this, eGXUIMessage::ListBoxAddItem, &GXUIListBox::OnListBoxAddItem );
    _messageHandlerTree.Add ( _messageHandlers[ 2u ] );

    _messageHandlers[ 3u ].Init ( *this, eGXUIMessage::ListBoxAddItems, &GXUIListBox::OnListBoxAddItems );
    _messageHandlerTree.Add ( _messageHandlers[ 3u ] );

    _messageHandlers[ 4u ].Init ( *this, eGXUIMessage::ListBoxRemoveAllItems, &GXUIListBox::OnListBoxRemoveAllItems );
    _messageHandlerTree.Add ( _messageHandlers[ 4u ] );

    _messageHandlers[ 5u ].Init ( *this, eGXUIMessage::ListBoxRemoveItem, &GXUIListBox::OnListBoxRemoveItem );
    _messageHandlerTree.Add ( _messageHandlers[ 5u ] );

    _messageHandlers[ 6u ].Init ( *this, eGXUIMessage::ListBoxSetItemHeight, &GXUIListBox::OnListBoxSetItemHeight );
    _messageHandlerTree.Add ( _messageHandlers[ 6u ] );

    _messageHandlers[ 7u ].Init ( *this, eGXUIMessage::ListBoxSetViewportOffset, &GXUIListBox::OnListBoxSetViewportOffset );
    _messageHandlerTree.Add ( _messageHandlers[ 7u ] );

    _messageHandlers[ 8u ].Init ( *this, eGXUIMessage::MouseLeave, &GXUIListBox::OnMouseLeave );
    _messageHandlerTree.Add ( _messageHandlers[ 8u ] );

    _messageHandlers[ 9u ].Init ( *this, eGXUIMessage::MouseMove, &GXUIListBox::OnMouseMove );
    _messageHandlerTree.Add ( _messageHandlers[ 9u ] );

    _messageHandlers[ 10u ].Init ( *this, eGXUIMessage::MouseOver, &GXUIListBox::OnMouseOver );
    _messageHandlerTree.Add ( _messageHandlers[ 10u ] );

    _messageHandlers[ 11u ].Init ( *this, eGXUIMessage::Redraw, &GXUIListBox::OnRedraw );
    _messageHandlerTree.Add ( _messageHandlers[ 11u ] );

    _messageHandlers[ 12u ].Init ( *this, eGXUIMessage::Resize, &GXUIListBox::OnResize );
    _messageHandlerTree.Add ( _messageHandlers[ 12u ] );

    _messageHandlers[ 13u ].Init ( *this, eGXUIMessage::Scroll, &GXUIListBox::OnScroll );
    _messageHandlerTree.Add ( _messageHandlers[ 13u ] );
}

GXVoid GXUIListBox::OnDoubleClick ( const GXVoid* /*data*/ )
{
    GXUIListBoxItem* item = GetHighlightedItem ();

    if ( !item || !_onItemDoubleClicked ) return;

    _onItemDoubleClicked ( _itemDoubleClickedContext, *this, item->_data );
}

GXVoid GXUIListBox::OnLMBDown ( const GXVoid* /*data*/ )
{
    GXUIListBoxItem* item = GetHighlightedItem ();
    GXUIListBoxItem* selected = nullptr;

    for ( selected = _itemHead; selected; selected = selected->_next )
    {
        if ( !selected->_isSelected ) continue;

        break;
    }

    if ( selected == item ) return;

    if ( selected )
        selected->_isSelected = GX_FALSE;

    if ( item )
        item->_isSelected = GX_TRUE;

    if ( _onItemSelected && item )
        _onItemSelected ( _itemSelectedContext, *this, item->_data );

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnListBoxAddItem ( const GXVoid* data )
{
    GXVoid* pointer = const_cast<GXVoid*> ( data );
    GXVoid** itemData = reinterpret_cast<GXVoid**> ( pointer );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXUIListBoxItem" )
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

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnListBoxAddItems ( const GXVoid* data )
{
    GXVoid* pointer = const_cast<GXVoid*> ( data );
    GXUIListBoxRawItem** list = reinterpret_cast<GXUIListBoxRawItem**> ( pointer );

    for ( GXUIListBoxRawItem* p = *list; p; p = p->_next )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXUIListBoxItem" )
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

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnListBoxRemoveAllItems ( const GXVoid* /*data*/ )
{
    while ( _itemHead )
    {
        GXUIListBoxItem* p = _itemHead;
        _itemHead = _itemHead->_next;
        _destroyItem ( p->_data );
        delete p;
    }

    _itemTail = nullptr;
    _totalItems = 0u;

    _viewportOffset = 0.0f;
    _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnListBoxRemoveItem ( const GXVoid* data )
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

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnListBoxSetItemHeight ( const GXVoid* data )
{
    const GXFloat* height = static_cast<const GXFloat*> ( data );
    _itemHeight = *height;

    _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

    if ( !_renderer )

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnListBoxSetViewportOffset ( const GXVoid* data )
{
    const GXFloat* offset = static_cast<const GXFloat*> ( data );
    _viewportOffset = *offset;

    _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnMouseLeave ( const GXVoid* /*data*/ )
{
    for ( GXUIListBoxItem* p = _itemHead; p; p = p->_next )
    {
        if ( !p->_isHighlighted ) continue;

        p->_isHighlighted = GX_FALSE;

        if ( _renderer )
            _renderer->OnUpdate ();

        break;
    }
}

GXVoid GXUIListBox::OnMouseMove ( const GXVoid* data )
{
    const GXVec2* pos = static_cast<const GXVec2*> ( data );

    const GXBool isNeedRenderUpdate = ResetHighlight ( *pos );

    if ( !isNeedRenderUpdate || !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnMouseOver ( const GXVoid* data )
{
    const GXVec2* pos = static_cast<const GXVec2*> ( data );

    const GXBool isNeedRenderUpdate = ResetHighlight ( *pos );

    if ( !isNeedRenderUpdate || !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnRedraw ( const GXVoid* /*data*/ )
{
    const GXVec2& position = GXTouchSurface::GetInstance ().GetMousePosition ();
    OnMessage ( eGXUIMessage::MouseMove, &position );
}

GXVoid GXUIListBox::OnResize ( const GXVoid* data )
{
    const GXAABB* newBoundsLocal = static_cast<const GXAABB*> ( data );
    UpdateBoundsWorld ( *newBoundsLocal );
    _viewportSize = _boundsLocal.GetHeight () / GetTotalHeight ();

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIListBox::OnScroll ( const GXVoid* data )
{
    const GXVec3* scroll = static_cast<const GXVec3*> ( data );

    if ( !IsAbleToScroll () ) return;

    const GXFloat step = 1.0f / static_cast<GXFloat> ( _totalItems );
    const GXFloat offset = GXClampf ( _viewportOffset - scroll->_data[ 2u ] * step, 0.0f, 1.0f - _viewportSize );

    _viewportOffset = offset;

    const GXVec2 pos ( scroll->_data[ 0u ], scroll->_data[ 2u ] );
    OnMessage ( eGXUIMessage::MouseMove, &pos );
}
