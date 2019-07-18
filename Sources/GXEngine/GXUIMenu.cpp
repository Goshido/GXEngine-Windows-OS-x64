// version 1.5

#include <GXEngine/GXUIMenu.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXStrings.h>


#define ANY_HEIGHT          1.0f
#define INVALID_OFFSET      -1.0f
#define INVALID_WIDTH       0.0f


struct GXUIMenuItem final
{
    GXWChar*        _name;
    GXAABB          _bounds;
    GXUIPopup*      _popup;
};

//-----------------------------------------------------------------

GXUIMenu::GXUIMenu ( GXWidget* parent ):
    GXWidget ( parent ),
    _items ( sizeof ( GXUIMenuItem ) ),
    _selectedItemIndex ( static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX ) ),
    _highlightedItemIndex ( static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX ) )
{
    // NOTHING
}

GXUIMenu::~GXUIMenu ()
{
    GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
    GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );

    for ( GXUByte i = 0u; i < totalItems; ++i )
        GXSafeFree ( itemStorage[ i ]._name );
}

GXVoid GXUIMenu::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    if ( message == eGXUIMessage::MenuAddItem )
    {
        const GXUIMenuItem* item = static_cast<const GXUIMenuItem*> ( data );
        GXFloat itemWidth = item->_bounds.GetWidth ();

        GXUIMenuItem newItem;

        if ( item->_name )
            GXWcsclone ( &newItem._name, item->_name );
        else
            newItem._name = nullptr;

        newItem._popup = item->_popup;
        GXAABB newBoundsLocal;
        newBoundsLocal.AddVertex ( _boundsLocal._min );
        GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );

        if ( totalItems > 0u )
        {
            newItem._bounds.AddVertex ( _boundsWorld._max.GetX (), _boundsWorld._min.GetY (), -1.0f );
            newItem._bounds.AddVertex ( _boundsWorld._max.GetX () + itemWidth, _boundsWorld._max.GetY (), 1.0f );

            GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );
            newBoundsLocal.AddVertex ( _boundsLocal._min.GetX () + newItem._bounds._max.GetX () - itemStorage[ 0 ]._bounds._min.GetX (), _boundsLocal._max.GetY (), _boundsLocal._max.GetZ () );
        }
        else
        {
            newItem._bounds.AddVertex ( _boundsWorld._min.GetX (), _boundsWorld._min.GetY (), -1.0f );
            newItem._bounds.AddVertex ( _boundsWorld._min.GetX () + itemWidth, _boundsWorld._max.GetY (), 1.0f );

            newBoundsLocal.AddVertex ( _boundsLocal._min.GetX () + itemWidth, _boundsLocal._max.GetY (), _boundsLocal._max.GetZ () );
        }

        _items.SetValue ( totalItems, &newItem );
        GXWidget::OnMessage ( eGXUIMessage::Resize, &newBoundsLocal );

        return;
    }

    if ( message == eGXUIMessage::LMBDown )
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
                item->_popup->Resize ( item->_bounds._min.GetX (), item->_bounds._min.GetY () - popupHeight, popupWidth, popupHeight );
                item->_popup->Show ( this );
            }

            _selectedItemIndex = _highlightedItemIndex;
        }

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::MouseMove )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );

        GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );
        GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
        GXUByte mouseoverItemIndex = 0u;

        for ( ; mouseoverItemIndex < totalItems; ++mouseoverItemIndex )
            if ( itemStorage[ mouseoverItemIndex ]._bounds.IsOverlaped ( pos->GetX (), pos->GetY (), 0.0f ) ) break;

        if ( _highlightedItemIndex == mouseoverItemIndex ) return;

        _highlightedItemIndex = mouseoverItemIndex;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::MouseLeave )
    {
        _highlightedItemIndex = static_cast<GXUByte> ( GX_UI_MENU_INVALID_INDEX );

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::Resize )
    {
        const GXAABB* bounds = static_cast<const GXAABB*> ( data );

        GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
        GXVec2 locationDelta ( bounds->_min.GetX () - _boundsLocal._min.GetX (), bounds->_min.GetY () - _boundsLocal._min.GetY () );
        GXAABB newBoundsLocal;

        if ( totalItems > 0u )
        {
            GXFloat heightDelta = _boundsLocal.GetHeight () - bounds->GetHeight ();
            GXUIMenuItem* itemStorage = static_cast<GXUIMenuItem*> ( _items.GetData () );

            for ( GXUByte i = 0u; i < totalItems; ++i )
            {
                GXUIMenuItem* currentItem = itemStorage + i;

                currentItem->_bounds._min._data[ 0 ] += locationDelta._data[ 0 ];
                currentItem->_bounds._max._data[ 0 ] += locationDelta._data[ 0 ];

                currentItem->_bounds._min._data[ 1 ] += locationDelta._data[ 1 ];
                currentItem->_bounds._max._data[ 1 ] += locationDelta._data[ 1 ] + heightDelta;
            }

            newBoundsLocal.AddVertex ( bounds->_min.GetX (), bounds->_min.GetY (), -1.0f );
            newBoundsLocal.AddVertex ( bounds->_min.GetX () + itemStorage[ totalItems ]._bounds._max.GetX () - itemStorage[ 0 ]._bounds._min.GetX (), bounds->_min.GetY () + itemStorage[ totalItems ]._bounds._max.GetY () - itemStorage[ 0 ]._bounds._min.GetY (), 1.0f );
        }
        else
        {
            newBoundsLocal = *bounds;
        }

        GXWidget::OnMessage ( message, &newBoundsLocal );
        return;
    }

    if ( message == eGXUIMessage::PopupClosed )
    {
        if ( _selectedItemIndex == GX_UI_MENU_INVALID_INDEX ) return;

        GXVoid* pointer = const_cast<GXVoid*> ( data );
        const GXUIPopup** popup = reinterpret_cast<const GXUIPopup**> ( pointer );
        GXUIMenuItem* itemStorage = (GXUIMenuItem*)_items.GetData ();

        if ( itemStorage[ _selectedItemIndex ]._popup != *popup ) return;

        _selectedItemIndex = GX_UI_MENU_INVALID_INDEX;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    GXWidget::OnMessage ( message, data );
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
    return (GXUByte)_items.GetLength ();
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
