// version 1.0

#include <GXEngine/GXDesktopInput.h>


GXKeyBind::GXKeyBind ( const GXKeyBind &other ):
    _context ( other._context ),
    _handler ( other._handler )
{
    // NOTHING
}

GXVoid GXKeyBind::Init ( GXVoid* context, GXKeyHandler handler )
{
    _context = context;
    _handler = handler;
}

GXVoid GXKeyBind::Reset ()
{
    _context = nullptr;
    _handler = nullptr;
}

GXKeyBind& GXKeyBind::operator = ( const GXKeyBind &other )
{
    _context = other._context;
    _handler = other._handler;

    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXMouseButtonBind::GXMouseButtonBind ( const GXMouseButtonBind &other ):
    _context ( other._context ),
    _handler ( other._handler )
{
    // NOTHING
}

GXVoid GXMouseButtonBind::Init ( GXVoid* context, GXMouseButtonHandler handler )
{
    _context = context;
    _handler = handler;
}

GXVoid GXMouseButtonBind::Reset ()
{
    _context = nullptr;
    _handler = nullptr;
}

GXMouseButtonBind& GXMouseButtonBind::operator = ( const GXMouseButtonBind &other )
{
    _context = other._context;
    _handler = other._handler;

    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXAbstractDesktopInput::GXAbstractDesktopInput ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXAbstractDesktopInput" )
{
    // NOTHING
}
