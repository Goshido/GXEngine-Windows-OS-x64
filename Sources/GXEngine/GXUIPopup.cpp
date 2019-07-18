// version 1.5

#include <GXEngine/GXUIPopup.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>


#define DEFAULT_HEIGHT    0.5f

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
    // NOTHING
}

GXUIPopup::~GXUIPopup ()
{
    // NOTHING
}

GXVoid GXUIPopup::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    if ( message == eGXUIMessage::PopupAddItem )
    {
        const GXUIPopupItem* pi = static_cast<const GXUIPopupItem*> ( data );

        GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
        GXUIPopupItem item;
        item._context = pi->_context;
        item._action = pi->_action;
        item._isActive = GX_TRUE;

        if ( totalItems == 0u )
        {
            item._boundsWorld.AddVertex ( _boundsLocal._min.GetX (), _boundsLocal._max.GetY () - _itemHeight, _boundsLocal._min.GetZ () );
            item._boundsWorld.AddVertex ( _boundsLocal._max.GetX (), _boundsLocal._max.GetY (), _boundsLocal._max.GetZ () );

            UpdateBoundsWorld ( item._boundsWorld );
        }
        else
        {
            item._boundsWorld.AddVertex ( _boundsLocal._min.GetX (), _boundsLocal._min.GetY () - _itemHeight, _boundsLocal._min.GetZ () );
            item._boundsWorld.AddVertex ( _boundsLocal._max.GetX (), _boundsLocal._min.GetY (), _boundsLocal._max.GetZ () );

            GXAABB newBoundsLocal ( _boundsLocal );
            newBoundsLocal._min._data[ 1 ] -= _itemHeight;
            UpdateBoundsWorld ( newBoundsLocal );
        }

        _items.SetValue ( totalItems, &item );
        ++totalItems;

        if ( totalItems != 0u && _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::PopupSetItemHeight )
    {
        const GXFloat* newItemHeight = static_cast<const GXFloat*> ( data );
        _itemHeight = *newItemHeight;

        GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );

        if ( totalItems != 0u )
        {
            GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );

            GXAABB newBoundsLocal ( _boundsLocal );
            newBoundsLocal._min.SetY ( newBoundsLocal._max.GetY () );

            for ( GXUInt i = 0u; i < totalItems; ++i )
            {
                itemStorage[ i ]._boundsWorld.Empty ();
                itemStorage[ i ]._boundsWorld.AddVertex ( newBoundsLocal._max.GetX (), newBoundsLocal._min.GetY (), newBoundsLocal._max.GetZ () );
                newBoundsLocal._min._data[ 1 ] -= _itemHeight;
                itemStorage[ i ]._boundsWorld.AddVertex ( newBoundsLocal._min.GetX (), newBoundsLocal._min.GetY (), newBoundsLocal._min.GetZ () );
            }

            UpdateBoundsWorld ( newBoundsLocal );
        }

        if ( totalItems != 0u && _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::PopupEnableItem )
    {
        const GXUByte* itemIndex = static_cast<const GXUByte*> ( data );

        if ( static_cast<GXUPointer> ( *itemIndex ) >= _items.GetLength () ) return;

        GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );

        if ( itemStorage[ *itemIndex ]._isActive ) return;

        itemStorage[ *itemIndex ]._isActive = GX_TRUE;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::PopupDisableItem )
    {
        const GXUByte* itemIndex = static_cast<const GXUByte*> ( data );

        if ( static_cast<GXUPointer> ( *itemIndex ) >= _items.GetLength () ) return;

        GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );

        if ( !itemStorage[ *itemIndex ]._isActive ) return;

        itemStorage[ *itemIndex ]._isActive = GX_FALSE;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::PopupShow )
    {
        GXVoid* pointer = const_cast<GXVoid*> ( data );
        GXWidget** currentOwner = reinterpret_cast<GXWidget**> ( pointer );
        _owner = *currentOwner;
        GXWidget::Show ();

        return;
    }

    if ( message == eGXUIMessage::LMBUp )
    {
        if ( _selectedItemIndex != GX_UI_POPUP_INVALID_INDEX )
        {
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
        }

        Hide ();
        return;
    }

    if ( message == eGXUIMessage::MouseMove )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );
        GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
            
        GXUByte mouseOverItem = 0u;

        for ( ; mouseOverItem < totalItems; ++mouseOverItem )
            if ( itemStorage[ mouseOverItem ]._boundsWorld.IsOverlaped ( pos->GetX (), pos->GetY (), 0.0f ) ) break;

        if ( _selectedItemIndex == static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX ) && !itemStorage[ mouseOverItem ]._isActive ) return;

        if ( _selectedItemIndex == mouseOverItem && itemStorage[ mouseOverItem ]._isActive ) return;

        _selectedItemIndex = itemStorage[ mouseOverItem ]._isActive ? mouseOverItem : static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX );

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::MouseLeave )
    {
        if ( _selectedItemIndex != static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX ) )
        {
            _selectedItemIndex = static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX );

            if ( _renderer )
                _renderer->OnUpdate ();
        }

        if ( _owner )
        {
            GXUIPopup* pointer = this;
            GXTouchSurface::GetInstance ().SendMessage ( _owner, eGXUIMessage::PopupClosed, &pointer, sizeof ( GXUIPopup* ) );
        }

        Hide ();
        return;
    }

    if ( message == eGXUIMessage::Resize )
    {
        const GXAABB* newBoundsLocal = static_cast<const GXAABB*> ( data );
        GXFloat newWidth = newBoundsLocal->GetWidth ();
        GXVec2 newPosition ( newBoundsLocal->_min.GetX (), newBoundsLocal->_min.GetY () );

        GXUByte totalItems = static_cast<GXUByte> ( _items.GetLength () );
        GXFloat top = newPosition.GetY () + totalItems * _itemHeight;
        GXUIPopupItem* itemStorage = static_cast<GXUIPopupItem*> ( _items.GetData () );

        for ( GXUInt i = 0u; i < totalItems; ++i )
        {
            itemStorage[ i ]._boundsWorld.Empty ();
            itemStorage[ i ]._boundsWorld.AddVertex ( newPosition.GetX (), top - _itemHeight, -1.0f );
            itemStorage[ i ]._boundsWorld.AddVertex ( newPosition.GetX () + newWidth, top, 1.0f );
            top -= _itemHeight;
        }
            
        GXAABB bounds;
        bounds.AddVertex ( newPosition.GetX (), newPosition.GetY (), -1.0f );
        bounds.AddVertex ( newPosition.GetX () + newWidth, newPosition.GetY () + totalItems * _itemHeight, 1.0f );
        GXWidget::OnMessage ( message, &bounds );

        return;
    }

    if ( message == eGXUIMessage::Hide )
    {
        _selectedItemIndex = static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX );
        GXWidget::OnMessage ( message, data );
        return;
    }

    GXWidget::OnMessage ( message, data );
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
