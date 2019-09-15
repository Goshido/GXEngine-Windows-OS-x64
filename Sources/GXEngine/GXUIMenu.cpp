// version 1.6

#include <GXEngine/GXUIMenu.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXStrings.h>


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
    GXWChar*        _name;
    GXAABB          _bounds;
    GXUIPopup*      _popup;
};

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
    GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );

    for ( GXUByte i = 0u; i < totalItems; ++i )
    {
        GXSafeFree ( itemStorage[ i ]._name );
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

GXVoid GXUIMenu::AddItem ( const GXWChar* name, GXFloat itemWidth, GXUIPopup* popup )
{
    if ( !name ) return;

    GXUIMenuItem item;

    GXUPointer size = ( GXWcslen ( name ) + 1u ) * sizeof ( GXWChar );
    item._name = static_cast<GXWChar*> ( gx_ui_MessageBuffer->Allocate ( size ) );
    memcpy ( item._name, name, size );
    item._bounds.AddVertex ( 0.0f, 0.0f, -1.0f );
    item._bounds.AddVertex ( itemWidth, ANY_HEIGHT, 1.0f );
    item._popup = popup;

    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::MenuAddItem, &item, sizeof ( GXUIMenuItem ) );
    popup->Hide ();
}

GXUByte GXUIMenu::GetTotalItems () const
{
    return static_cast<GXUByte> ( _items.GetLength () );
}

const GXWChar* GXUIMenu::GetItemName ( GXUByte itemIndex ) const
{
    if ( itemIndex >= static_cast<GXUByte> ( _items.GetLength () ) ) return nullptr;

    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );
    return itemStorage[ itemIndex ]._name;
}

GXFloat GXUIMenu::GetItemOffset ( GXUByte itemIndex ) const
{
    if ( itemIndex >= static_cast<GXUByte> ( _items.GetLength () ) ) return INVALID_OFFSET;

    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );
    return itemStorage[ itemIndex ]._bounds._min.GetX () - _boundsWorld._min.GetX ();
}

GXFloat GXUIMenu::GetItemWidth ( GXUByte itemIndex ) const
{
    if ( itemIndex >= static_cast<GXUByte> ( _items.GetLength () ) ) return INVALID_WIDTH;

    GXUIMenuItem* itemStorage = (GXUIMenuItem*)_items.GetData ();
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
    const GXUIMenuItem* item = static_cast<const GXUIMenuItem*> ( data );
    const GXFloat itemWidth = item->_bounds.GetWidth ();

    GXUIMenuItem newItem;

    if ( item->_name )
        GXWcsclone ( &newItem._name, item->_name );
    else
        newItem._name = nullptr;

    newItem._popup = item->_popup;
    GXAABB newBoundsLocal;
    newBoundsLocal.AddVertex ( _boundsLocal._min );
    const GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );

    if ( totalItems > 0u )
    {
        newItem._bounds.AddVertex ( _boundsWorld._max._data[ 0u ], _boundsWorld._min._data[ 1u ], -1.0f );
        newItem._bounds.AddVertex ( _boundsWorld._max._data[ 0u ] + itemWidth, _boundsWorld._max._data[ 1u ], 1.0f );

        GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );
        newBoundsLocal.AddVertex ( _boundsLocal._min._data[ 0u ] + newItem._bounds._max._data[ 0u ] - itemStorage[ 0u ]._bounds._min._data[ 0u ], _boundsLocal._max._data[ 1u ], _boundsLocal._max._data[ 2u ] );
    }
    else
    {
        newItem._bounds.AddVertex ( _boundsWorld._min._data[ 0u ], _boundsWorld._min._data[ 1u ], -1.0f );
        newItem._bounds.AddVertex ( _boundsWorld._min._data[ 0u ] + itemWidth, _boundsWorld._max._data[ 1u ], 1.0f );

        newBoundsLocal.AddVertex ( _boundsLocal._min._data[ 0u ] + itemWidth, _boundsLocal._max._data[ 1u ], _boundsLocal._max._data[ 2u ] );
    }

    _items.SetValue ( totalItems, &newItem );
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
    }
    else
    {
        GXFloat heightDelta = _boundsLocal.GetHeight () - bounds->GetHeight ();
        GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );

        for ( GXUByte i = 0u; i < totalItems; ++i )
        {
            GXUIMenuItem* currentItem = itemStorage + i;

            currentItem->_bounds._min._data[ 0u ] += locationDelta._data[ 0u ];
            currentItem->_bounds._max._data[ 0u ] += locationDelta._data[ 0u ];

            currentItem->_bounds._min._data[ 1u ] += locationDelta._data[ 1u ];
            currentItem->_bounds._max._data[ 1u ] += locationDelta._data[ 1u ] + heightDelta;
        }

        newBoundsLocal.AddVertex ( bounds->_min._data[ 0u ], bounds->_min._data[ 1u ], -1.0f );
        newBoundsLocal.AddVertex ( bounds->_min._data[ 0u ] + itemStorage[ totalItems ]._bounds._max._data[ 0u ] - itemStorage[ 0u ]._bounds._min._data[ 0u ], bounds->_min._data[ 1u ] + itemStorage[ totalItems ]._bounds._max._data[ 1u ] - itemStorage[ 0u ]._bounds._min._data[ 1u ], 1.0f );
    }

    GXWidget::OnMessage ( eGXUIMessage::Resize, &newBoundsLocal );
}
