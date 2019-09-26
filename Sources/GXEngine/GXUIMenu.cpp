// version 1.6

#include <GXEngine/GXUIMenu.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>


#define ANY_HEIGHT          1.0f
#define INVALID_OFFSET      -1.0f
#define INVALID_WIDTH       0.0f

//---------------------------------------------------------------------------------------------------------------------

GXUIMenuMessageHandlerNode::GXUIMenuMessageHandlerNode ()
{
    // NOTHING
}

GXUIMenuMessageHandlerNode::GXUIMenuMessageHandlerNode ( eGXUIMessage message ):
    GXUIWidgetMessageHandlerNode ( message )
{
    // NOTHING
}

GXUIMenuMessageHandlerNode::~GXUIMenuMessageHandlerNode ()
{
    // NOTHING
}

GXVoid GXUIMenuMessageHandlerNode::Init ( GXUIMenu &menu, eGXUIMessage message, GXUIMenuOnMessageHandler handler )
{
    _message = message;
    _handler = handler;
    _widget = &menu;
}

GXVoid GXUIMenuMessageHandlerNode::HandleMassage ( const GXVoid* data )
{
    GXUIMenu* menu = static_cast<GXUIMenu*> ( _widget );

    // Note this is C++ syntax for invoke class method.
    ( menu->*_handler ) ( data );
}

//---------------------------------------------------------------------------------------------------------------------

struct GXUIMenuItem final
{
    const GXString      _name;

    // Note GXUIMenuItem does not own this resource.
    GXUIPopup*          _popup;

    GXAABB              _bounds;

    explicit GXUIMenuItem ( const GXUTF16* name, GXUIPopup* popup );

private:
    GXUIMenuItem () = delete;
    GXUIMenuItem ( const GXUIMenuItem &other ) = delete;
    GXUIMenuItem& operator = ( const GXUIMenuItem &other ) = delete;
};

GXUIMenuItem::GXUIMenuItem ( const GXUTF16* name, GXUIPopup* popup ):
    _name ( name ),
    _popup ( popup )
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

GXUIMenu::GXUIMenu ( GXWidget* parent ):
    GXWidget ( parent ),
    _items ( sizeof ( GXUIMenuItem ) ),
    _selectedItemIndex ( static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX ) ),
    _highlightedItemIndex ( static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX ) )
{
    InitMessageHandlers ();
}

GXUIMenu::~GXUIMenu ()
{
    // Kung-Fu: GXUIMenuItem::_name resouse will be allocated via placement new in AddItem method.
    // Free this resource manualy.

    const GXUPointer totalItems = _items.GetLength ();
    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );

    for ( GXUPointer i = 0u; i < totalItems; ++i )
    {
        GXUIMenuItem& item = itemStorage[ i ];
        item._name.~GXString ();
    }
}

GXVoid GXUIMenu::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    GXUIMenuMessageHandlerNode probe ( message );
    GXAVLTreeNode* findResult = _messageHandlerTree.Find ( probe );

    if ( !findResult )
    {
        GXWidget::OnMessage ( message, data );
        return;
    }

    GXUIMenuMessageHandlerNode* targetHandler = static_cast<GXUIMenuMessageHandlerNode*> ( findResult );
    targetHandler->HandleMassage ( data );
}

GXVoid GXUIMenu::AddItem ( const GXString &name, GXFloat itemWidth, GXUIPopup* popup )
{
    if ( name.IsNull () ) return;

    // Kung-Fu: Item storage is GXDynamicArray. Task is to create pesistent GXString in raw memory.
    // So will be used placement new. This prevents calling destructor for GXUIMenuItem object.
    // GXDynamicArray destruction will be handled manualy in ~GXUIMenu.

    GXUByte memory[ sizeof ( GXUIMenuItem ) ];
    GXUIMenuItem* newItem = ::new ( memory ) GXUIMenuItem ( name, popup );
    GXAABB& bounds = newItem->_bounds;
    bounds.AddVertex ( 0.0f, 0.0f, -1.0f );
    bounds.AddVertex ( itemWidth, ANY_HEIGHT, 1.0f );

    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::MenuAddItem, newItem, sizeof ( GXUIMenuItem ) );
    popup->Hide ();
}

GXUPointer GXUIMenu::GetTotalItems () const
{
    return _items.GetLength ();
}

const GXString& GXUIMenu::GetItemName ( GXUPointer itemIndex ) const
{
    if ( itemIndex >= _items.GetLength () )
    {
        GXLogA ( "GXUIMenu::GetItemName::Error - Can't return item name for item with %hu index. There is(are) %zu item(s) only.", static_cast<GXUShort> ( itemIndex ), _items.GetLength () );
        return _defaultText;
    }

    const GXUIMenuItem* itemStorage = static_cast<const GXUIMenuItem*> ( _items.GetData () );
    return itemStorage[ itemIndex ]._name;
}

GXFloat GXUIMenu::GetItemOffset ( GXUPointer itemIndex ) const
{
    if ( itemIndex >= _items.GetLength () )
        return INVALID_OFFSET;

    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );
    return itemStorage[ itemIndex ]._bounds._min._data[ 0u ] - _boundsWorld._min._data[ 0u ];
}

GXFloat GXUIMenu::GetItemWidth ( GXUPointer itemIndex ) const
{
    if ( itemIndex >= _items.GetLength () )
        return INVALID_WIDTH;

    const GXUIMenuItem* itemStorage = static_cast<const GXUIMenuItem*> ( _items.GetData () );
    return itemStorage[ itemIndex ]._bounds.GetWidth ();
}

GXUByte GXUIMenu::GetSelectedItemIndex () const
{
    return _selectedItemIndex;
}

GXUByte GXUIMenu::GetHighlightedItemIndex () const
{
    return _highlightedItemIndex;
}

GXVoid GXUIMenu::InitMessageHandlers ()
{
    _messageHandlers[ 0u ].Init ( *this, eGXUIMessage::LMBDown, &GXUIMenu::OnLMBDown );
    _messageHandlerTree.Add ( _messageHandlers[ 0u ] );

    _messageHandlers[ 1u ].Init ( *this, eGXUIMessage::MenuAddItem, &GXUIMenu::OnMenuAddItem );
    _messageHandlerTree.Add ( _messageHandlers[ 1u ] );

    _messageHandlers[ 2u ].Init ( *this, eGXUIMessage::MouseLeave, &GXUIMenu::OnMouseLeave );
    _messageHandlerTree.Add ( _messageHandlers[ 2u ] );

    _messageHandlers[ 3u ].Init ( *this, eGXUIMessage::MouseMove, &GXUIMenu::OnMouseMove );
    _messageHandlerTree.Add ( _messageHandlers[ 3u ] );

    _messageHandlers[ 4u ].Init ( *this, eGXUIMessage::PopupClosed, &GXUIMenu::OnPopupClosed );
    _messageHandlerTree.Add ( _messageHandlers[ 4u ] );

    _messageHandlers[ 5u ].Init ( *this, eGXUIMessage::Resize, &GXUIMenu::OnResize );
    _messageHandlerTree.Add ( _messageHandlers[ 5u ] );
}

GXVoid GXUIMenu::OnLMBDown ( const GXVoid* /*data*/ )
{
    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );

    if ( _highlightedItemIndex == _selectedItemIndex )
    {
        if ( itemStorage[ _highlightedItemIndex ]._popup )
            itemStorage[ _highlightedItemIndex ]._popup->Hide ();

        _selectedItemIndex = static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX );
    }
    else
    {
        if ( _selectedItemIndex != static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX ) && itemStorage[ _selectedItemIndex ]._popup )
            itemStorage[ _selectedItemIndex ]._popup->Hide ();

        GXUIMenuItem* item = itemStorage + _highlightedItemIndex;

        if ( item->_popup )
        {
            GXFloat popupWidth = item->_popup->GetBoundsLocal ().GetWidth ();
            GXFloat popupHeight = item->_popup->GetBoundsLocal ().GetHeight ();
            item->_popup->Resize ( item->_bounds._min._data[ 0u ], item->_bounds._min._data[ 1u ] - popupHeight, popupWidth, popupHeight );
            item->_popup->Show ( this );
        }

        _selectedItemIndex = _highlightedItemIndex;
    }

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIMenu::OnMenuAddItem ( const GXVoid* data )
{
    GXUIMenuItem* newItem = const_cast<GXUIMenuItem*> ( static_cast<const GXUIMenuItem*> ( data ) );
    GXAABB& bounds = newItem->_bounds;
    const GXFloat itemWidth = bounds.GetWidth ();
    bounds.Empty ();

    GXAABB newBoundsLocal;

    newBoundsLocal.AddVertex ( _boundsLocal._min );
    const GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );

    if ( totalItems > 0u )
    {
        bounds.AddVertex ( _boundsWorld._max._data[ 0u ], _boundsWorld._min._data[ 1u ], -1.0f );
        bounds.AddVertex ( _boundsWorld._max._data[ 0u ] + itemWidth, _boundsWorld._max._data[ 1u ], 1.0f );

        GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );
        newBoundsLocal.AddVertex ( _boundsLocal._min._data[ 0u ] + bounds._max._data[ 0u ] - itemStorage[ 0u ]._bounds._min._data[ 0u ], _boundsLocal._max._data[ 1u ], _boundsLocal._max._data[ 2u ] );
    }
    else
    {
        bounds.AddVertex ( _boundsWorld._min._data[ 0u ], _boundsWorld._min._data[ 1u ], -1.0f );
        bounds.AddVertex ( _boundsWorld._min._data[ 0u ] + itemWidth, _boundsWorld._max._data[ 1u ], 1.0f );

        newBoundsLocal.AddVertex ( _boundsLocal._min._data[ 0u ] + itemWidth, _boundsLocal._max._data[ 1u ], _boundsLocal._max._data[ 2u ] );
    }

    _items.PushBack ( newItem );
    GXWidget::OnMessage ( eGXUIMessage::Resize, &newBoundsLocal );
}

GXVoid GXUIMenu::OnMouseLeave ( const GXVoid* /*data*/ )
{
    _highlightedItemIndex = static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX );

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIMenu::OnMouseMove ( const GXVoid* data )
{
    const GXVec2* pos = static_cast<const GXVec2*> ( data );

    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );
    GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
    GXUByte mouseoverItemIndex = 0u;

    for ( ; mouseoverItemIndex < totalItems; ++mouseoverItemIndex )
    {
        if ( !itemStorage[ mouseoverItemIndex ]._bounds.IsOverlaped ( pos->_data[ 0u ], pos->_data[ 1u ], 0.0f ) ) continue;

        break;
    }

    if ( _highlightedItemIndex == mouseoverItemIndex ) return;

    _highlightedItemIndex = mouseoverItemIndex;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIMenu::OnPopupClosed ( const GXVoid* data )
{
    if ( _selectedItemIndex == GX_UI_MENU_INVALID_INDEX ) return;

    GXVoid* pointer = const_cast<GXVoid*> ( data );
    const GXUIPopup** popup = reinterpret_cast<const GXUIPopup**> ( pointer );
    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );

    if ( itemStorage[ _selectedItemIndex ]._popup != *popup ) return;

    _selectedItemIndex = GX_UI_MENU_INVALID_INDEX;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIMenu::OnResize ( const GXVoid* data )
{
    const GXAABB* bounds = static_cast<const GXAABB*> ( data );

    const GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
    const GXVec2 locationDelta ( bounds->_min._data[ 0u ] - _boundsLocal._min._data[ 0u ], bounds->_min._data[ 1u ] - _boundsLocal._min._data[ 1u ] );
    GXAABB newBoundsLocal;

    if ( totalItems < 1u )
    {
        newBoundsLocal = *bounds;
        GXWidget::OnMessage ( eGXUIMessage::Resize, &newBoundsLocal );
        return;
    }

    const GXFloat heightDelta = _boundsLocal.GetHeight () - bounds->GetHeight ();
    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );

    for ( GXUByte i = 0u; i < totalItems; ++i )
    {
        GXAABB& itemBounds = itemStorage[ i ]._bounds;

        itemBounds._min._data[ 0u ] += locationDelta._data[ 0u ];
        itemBounds._max._data[ 0u ] += locationDelta._data[ 0u ];

        itemBounds._min._data[ 1u ] += locationDelta._data[ 1u ];
        itemBounds._max._data[ 1u ] += locationDelta._data[ 1u ] + heightDelta;
    }

    newBoundsLocal.AddVertex ( bounds->_min._data[ 0u ], bounds->_min._data[ 1u ], -1.0f );
    newBoundsLocal.AddVertex ( bounds->_min._data[ 0u ] + itemStorage[ totalItems ]._bounds._max._data[ 0u ] - itemStorage[ 0u ]._bounds._min._data[ 0u ], bounds->_min._data[ 1u ] + itemStorage[ totalItems ]._bounds._max._data[ 1u ] - itemStorage[ 0u ]._bounds._min._data[ 1u ], 1.0f );

    GXWidget::OnMessage ( eGXUIMessage::Resize, &newBoundsLocal );
}
