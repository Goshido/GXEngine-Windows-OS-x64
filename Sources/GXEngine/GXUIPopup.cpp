// version 1.6

#include <GXEngine/GXUIPopup.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>


#define DEFAULT_HEIGHT    0.5f

//---------------------------------------------------------------------------------------------------------------------

GXUIPopupMessageHandlerNode::GXUIPopupMessageHandlerNode ()
{
    // NOTHING
}

GXUIPopupMessageHandlerNode::GXUIPopupMessageHandlerNode ( eGXUIMessage message ):
    GXUIWidgetMessageHandlerNode ( message )
{
    // NOTHING
}

GXUIPopupMessageHandlerNode::~GXUIPopupMessageHandlerNode ()
{
    // NOTHING
}

GXVoid GXUIPopupMessageHandlerNode::Init ( GXUIPopup &popup, eGXUIMessage message, GXUIPopupOnMessageHandler handler )
{
    _message = message;
    _handler = handler;
    _widget = &popup;
}

GXVoid GXUIPopupMessageHandlerNode::HandleMassage ( const GXVoid* data )
{
    GXUIPopup* popup = static_cast<GXUIPopup*> ( _widget );

    // Note this is C++ syntax for invoke class method.
    ( popup->*_handler ) ( data );
}

//---------------------------------------------------------------------------------------------------------------------

struct GXUIPopupItem final
{
    GXBool                      _isActive;
    GXVoid*                     _context;
    GXUIPopupActionHandler      _action;
    GXAABB                      _boundsWorld;
};

//---------------------------------------------------------------------------------------------------------------------

GXUIPopup::GXUIPopup ( GXWidget* parent ):
    GXWidget ( parent ),
    _items ( sizeof ( GXUIPopupItem ) ),
    _selectedItemIndex ( static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX ) ),
    _itemHeight ( gx_ui_Scale * DEFAULT_HEIGHT ),
    _owner ( nullptr )
{
    InitMessageHandlers ();
}

GXUIPopup::~GXUIPopup ()
{
    // NOTHING
}

GXVoid GXUIPopup::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    GXUIPopupMessageHandlerNode probe ( message );
    GXAVLTreeNode* findResult = _messageHandlerTree.Find ( probe );

    if ( !findResult )
    {
        GXWidget::OnMessage ( message, data );
        return;
    }

    GXUIPopupMessageHandlerNode* targetHandler = static_cast<GXUIPopupMessageHandlerNode*> ( findResult );
    targetHandler->HandleMassage ( data );
}

GXVoid GXUIPopup::AddItem ( GXVoid* context, GXUIPopupActionHandler action )
{
    GXUIPopupItem pi;
    pi._context = context;
    pi._action = action;
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::PopupAddItem, &pi, sizeof ( GXUIPopupItem ) );
}

GXUByte GXUIPopup::GetTotalItems () const
{
    return static_cast<GXUByte> ( _items.GetLength () );
}

GXVoid GXUIPopup::EnableItem ( GXUByte itemIndex )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::PopupEnableItem, &itemIndex, sizeof ( GXUByte ) );
}

GXVoid GXUIPopup::DisableItem ( GXUByte itemIndex )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::PopupDisableItem, &itemIndex, sizeof ( GXUByte ) );
}

GXBool GXUIPopup::IsItemActive ( GXUByte itemIndex ) const
{
    if ( (GXUInt)itemIndex >= _items.GetLength () ) return GX_FALSE;

    GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );
    return itemStorage[ itemIndex ]._isActive;
}

GXUByte GXUIPopup::GetSelectedItemIndex () const
{
    return _selectedItemIndex;
}

GXVoid GXUIPopup::SetItemHeight ( GXFloat height )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::PopupSetItemHeight, &height, sizeof ( GXFloat ) );
}

GXFloat GXUIPopup::GetItemHeight () const
{
    return _itemHeight;
}

GXFloat GXUIPopup::GetItemWidth () const
{
    return _boundsLocal.GetWidth ();
}

GXVoid GXUIPopup::Show ( GXWidget* currentOwner )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::PopupShow, &currentOwner, sizeof ( GXWidget* ) );
}

GXVoid GXUIPopup::InitMessageHandlers ()
{
    _messageHandlers[ 0u ].Init ( *this, eGXUIMessage::Hide, &GXUIPopup::OnHide );
    _messageHandlerTree.Add ( _messageHandlers[ 0u ] );

    _messageHandlers[ 1u ].Init ( *this, eGXUIMessage::LMBUp, &GXUIPopup::OnLMBUp );
    _messageHandlerTree.Add ( _messageHandlers[ 1u ] );

    _messageHandlers[ 2u ].Init ( *this, eGXUIMessage::MouseLeave, &GXUIPopup::OnMouseLeave );
    _messageHandlerTree.Add ( _messageHandlers[ 2u ] );

    _messageHandlers[ 3u ].Init ( *this, eGXUIMessage::MouseMove, &GXUIPopup::OnMouseMove );
    _messageHandlerTree.Add ( _messageHandlers[ 3u ] );

    _messageHandlers[ 4u ].Init ( *this, eGXUIMessage::PopupAddItem, &GXUIPopup::OnPopupAddItem );
    _messageHandlerTree.Add ( _messageHandlers[ 4u ] );

    _messageHandlers[ 5u ].Init ( *this, eGXUIMessage::PopupDisableItem, &GXUIPopup::OnPopupDisableItem );
    _messageHandlerTree.Add ( _messageHandlers[ 5u ] );

    _messageHandlers[ 6u ].Init ( *this, eGXUIMessage::PopupEnableItem, &GXUIPopup::OnPopupEnableItem );
    _messageHandlerTree.Add ( _messageHandlers[ 6u ] );

    _messageHandlers[ 7u ].Init ( *this, eGXUIMessage::PopupSetItemHeight, &GXUIPopup::OnPopupSetItemHeight );
    _messageHandlerTree.Add ( _messageHandlers[ 7u ] );

    _messageHandlers[ 8u ].Init ( *this, eGXUIMessage::PopupShow, &GXUIPopup::OnPopupShow );
    _messageHandlerTree.Add ( _messageHandlers[ 8u ] );

    _messageHandlers[ 9u ].Init ( *this, eGXUIMessage::Resize, &GXUIPopup::OnResize );
    _messageHandlerTree.Add ( _messageHandlers[ 9u ] );
}

GXVoid GXUIPopup::OnHide ( const GXVoid* data )
{
    _selectedItemIndex = static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX );
    GXWidget::OnMessage ( eGXUIMessage::Hide, data );
}

GXVoid GXUIPopup::OnLMBUp ( const GXVoid* /*data*/ )
{
    if ( _selectedItemIndex == GX_UI_POPUP_INVALID_INDEX )
    {
        Hide ();
        return;
    }

    GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );

    if ( itemStorage[ _selectedItemIndex ]._action )
        itemStorage[ _selectedItemIndex ]._action ( itemStorage[ _selectedItemIndex ]._context );

    if ( _owner )
    {
        GXUIPopup* pointer = this;
        GXTouchSurface::GetInstance ().SendMessage ( _owner, eGXUIMessage::PopupClosed, &pointer, sizeof ( GXUIPopup* ) );
    }

    _selectedItemIndex = GX_UI_POPUP_INVALID_INDEX;

    if ( _renderer )
        _renderer->OnUpdate ();

    Hide ();
}

GXVoid GXUIPopup::OnMouseLeave ( const GXVoid* /*data*/ )
{
    if ( _selectedItemIndex != static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX ) )
    {
        _selectedItemIndex = static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX );

        if ( _renderer )
        {
            _renderer->OnUpdate ();
        }
    }

    if ( _owner )
    {
        GXUIPopup* pointer = this;
        GXTouchSurface::GetInstance ().SendMessage ( _owner, eGXUIMessage::PopupClosed, &pointer, sizeof ( GXUIPopup* ) );
    }

    Hide ();
}

GXVoid GXUIPopup::OnMouseMove ( const GXVoid* data )
{
    const GXVec2* pos = static_cast<const GXVec2*> ( data );
    GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );
    GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );

    GXUByte mouseOverItem = 0u;

    for ( ; mouseOverItem < totalItems; ++mouseOverItem )
        if ( itemStorage[ mouseOverItem ]._boundsWorld.IsOverlaped ( pos->_data[ 0u ], pos->_data[ 1u ], 0.0f ) ) break;

    if ( _selectedItemIndex == static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX ) && !itemStorage[ mouseOverItem ]._isActive ) return;

    if ( _selectedItemIndex == mouseOverItem && itemStorage[ mouseOverItem ]._isActive ) return;

    _selectedItemIndex = itemStorage[ mouseOverItem ]._isActive ? mouseOverItem : static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX );

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIPopup::OnPopupAddItem ( const GXVoid* data )
{
    const GXUIPopupItem* pi = static_cast<const GXUIPopupItem*> ( data );

    GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
    GXUIPopupItem item;
    item._context = pi->_context;
    item._action = pi->_action;
    item._isActive = GX_TRUE;

    if ( totalItems == 0u )
    {
        item._boundsWorld.AddVertex ( _boundsLocal._min._data[ 0u ], _boundsLocal._max._data[ 1u ] - _itemHeight, _boundsLocal._min._data[ 2u ] );
        item._boundsWorld.AddVertex ( _boundsLocal._max._data[ 0u ], _boundsLocal._max._data[ 1u ], _boundsLocal._max._data[ 2u ] );

        UpdateBoundsWorld ( item._boundsWorld );
    }
    else
    {
        item._boundsWorld.AddVertex ( _boundsLocal._min._data[ 0u ], _boundsLocal._min._data[ 1u ] - _itemHeight, _boundsLocal._min._data[ 2u ] );
        item._boundsWorld.AddVertex ( _boundsLocal._max._data[ 0u ], _boundsLocal._min._data[ 1u ], _boundsLocal._max._data[ 2u ] );

        GXAABB newBoundsLocal ( _boundsLocal );
        newBoundsLocal._min._data[ 1u ] -= _itemHeight;
        UpdateBoundsWorld ( newBoundsLocal );
    }

    _items.SetValue ( totalItems, &item );
    ++totalItems;

    if ( totalItems < 1u || !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIPopup::OnPopupDisableItem ( const GXVoid* data )
{
    const GXUByte* itemIndex = static_cast<const GXUByte*> ( data );

    if ( static_cast<GXUPointer> ( *itemIndex ) >= _items.GetLength () ) return;

    GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );

    if ( !itemStorage[ *itemIndex ]._isActive ) return;

    itemStorage[ *itemIndex ]._isActive = GX_FALSE;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIPopup::OnPopupEnableItem ( const GXVoid* data )
{
    const GXUByte* itemIndex = static_cast<const GXUByte*> ( data );

    if ( static_cast<GXUPointer> ( *itemIndex ) >= _items.GetLength () ) return;

    GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );

    if ( itemStorage[ *itemIndex ]._isActive ) return;

    itemStorage[ *itemIndex ]._isActive = GX_TRUE;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIPopup::OnPopupSetItemHeight ( const GXVoid* data )
{
    const GXFloat* newItemHeight = static_cast<const GXFloat*> ( data );
    _itemHeight = *newItemHeight;

    GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );

    if ( totalItems != 0u )
    {
        GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );

        GXAABB newBoundsLocal ( _boundsLocal );
        newBoundsLocal._min.SetY ( newBoundsLocal._max._data[ 1u ] );

        for ( GXUInt i = 0u; i < totalItems; ++i )
        {
            itemStorage[ i ]._boundsWorld.Empty ();
            itemStorage[ i ]._boundsWorld.AddVertex ( newBoundsLocal._max._data[ 0u ], newBoundsLocal._min._data[ 1u ], newBoundsLocal._max._data[ 2u ] );
            newBoundsLocal._min._data[ 1u ] -= _itemHeight;
            itemStorage[ i ]._boundsWorld.AddVertex ( newBoundsLocal._min._data[ 0u ], newBoundsLocal._min._data[ 1u ], newBoundsLocal._min._data[ 2u ] );
        }

        UpdateBoundsWorld ( newBoundsLocal );
    }

    if ( totalItems < 1u || !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIPopup::OnPopupShow ( const GXVoid* data )
{
    GXVoid* pointer = const_cast<GXVoid*> ( data );
    GXWidget** currentOwner = reinterpret_cast<GXWidget**> ( pointer );
    _owner = *currentOwner;
    GXWidget::Show ();
}

GXVoid GXUIPopup::OnResize ( const GXVoid* data )
{
    const GXAABB* newBoundsLocal = static_cast<const GXAABB*> ( data );
    GXFloat newWidth = newBoundsLocal->GetWidth ();
    GXVec2 newPosition ( newBoundsLocal->_min._data[ 0u ], newBoundsLocal->_min._data[ 1u ] );

    GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
    GXFloat top = newPosition._data[ 1u ] + totalItems * _itemHeight;
    GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );

    for ( GXUInt i = 0u; i < totalItems; ++i )
    {
        itemStorage[ i ]._boundsWorld.Empty ();
        itemStorage[ i ]._boundsWorld.AddVertex ( newPosition._data[ 0u ], top - _itemHeight, -1.0f );
        itemStorage[ i ]._boundsWorld.AddVertex ( newPosition._data[ 0u ] + newWidth, top, 1.0f );
        top -= _itemHeight;
    }

    GXAABB bounds;
    bounds.AddVertex ( newPosition._data[ 0u ], newPosition._data[ 1u ], -1.0f );
    bounds.AddVertex ( newPosition._data[ 0u ] + newWidth, newPosition._data[ 1u ] + totalItems * _itemHeight, 1.0f );
    GXWidget::OnMessage ( eGXUIMessage::Resize, &bounds );
}
