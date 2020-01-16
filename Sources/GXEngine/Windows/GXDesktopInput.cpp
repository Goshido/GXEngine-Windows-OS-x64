// version 1.0

#include <GXEngine/Windows/GXDesktopInput.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXLogger.h>

GX_DISABLE_COMMON_WARNINGS

#include <new>
#include <Windowsx.h>

GX_RESTORE_WARNING_STATE


// see https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

#define VK_KEY_0                        0x30u
#define VK_KEY_1                        0x31u
#define VK_KEY_2                        0x32u
#define VK_KEY_3                        0x33u
#define VK_KEY_4                        0x34u
#define VK_KEY_5                        0x35u
#define VK_KEY_6                        0x36u
#define VK_KEY_7                        0x37u
#define VK_KEY_8                        0x38u
#define VK_KEY_9                        0x39u

#define VK_KEY_A                        0x41u
#define VK_KEY_B                        0x42u
#define VK_KEY_C                        0x43u
#define VK_KEY_D                        0x44u
#define VK_KEY_E                        0x45u
#define VK_KEY_F                        0x46u
#define VK_KEY_G                        0x47u
#define VK_KEY_H                        0x48u
#define VK_KEY_I                        0x49u
#define VK_KEY_J                        0x4Au
#define VK_KEY_K                        0x4Bu
#define VK_KEY_L                        0x4Cu
#define VK_KEY_M                        0x4Du
#define VK_KEY_N                        0x4Eu
#define VK_KEY_O                        0x4Fu
#define VK_KEY_P                        0x50u
#define VK_KEY_Q                        0x51u
#define VK_KEY_R                        0x52u
#define VK_KEY_S                        0x53u
#define VK_KEY_T                        0x54u
#define VK_KEY_U                        0x55u
#define VK_KEY_V                        0x56u
#define VK_KEY_W                        0x57u
#define VK_KEY_X                        0x58u
#define VK_KEY_Y                        0x59u
#define VK_KEY_Z                        0x5Au

#define NO_VALUE                        INT_MAX

// ~125 ticks per second
#define THREAD_TICK_TIMEOUT             8u

#define SKIP_KEY_VIRTUAL_CODE           0u

//---------------------------------------------------------------------------------------------------------------------

GXKeyNode::GXKeyNode ( WPARAM virtualCode ):
    _virtualCode ( virtualCode )
{
    // NOTHING
}

GXKeyNode::GXKeyNode ( WPARAM virtualCode, eGXKeyboardKey key ):
    _virtualCode ( virtualCode ),
    _key ( key )
{
    // NOTHING
}

eGXCompareResult GXKeyNode::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
    const GXKeyNode& aValue = static_cast<const GXKeyNode&> ( a );
    const GXKeyNode& bValue = static_cast<const GXKeyNode&> ( b );

    if ( aValue._virtualCode < bValue._virtualCode )
        return eGXCompareResult::Less;

    return aValue._virtualCode > bValue._virtualCode ? eGXCompareResult::Greater : eGXCompareResult::Equal;
}

//---------------------------------------------------------------------------------------------------------------------

GXInputKeyAction::GXInputKeyAction ( const GXInputKeyAction &other ):
    _bind ( other._bind ),
    _next ( other._next )
{
    // NOTHING
}

GXInputKeyAction& GXInputKeyAction::operator = ( const GXInputKeyAction &other )
{
    _bind = other._bind;
    _next = other._next;

    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXInputMouseButtonAction::GXInputMouseButtonAction ( const GXInputMouseButtonAction &other ):
    _bind ( other._bind ),
    _next ( other._next )
{
    // NOTHING
}

GXInputMouseButtonAction& GXInputMouseButtonAction::operator = ( const GXInputMouseButtonAction &other )
{
    _bind = other._bind;
    _next = other._next;

    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXOSMessageNode::GXOSMessageNode ( UINT osType ):
    _osType ( osType )
{
    // NOTHING
}

GXOSMessageNode::GXOSMessageNode ( UINT osType, GXDesktopInputHandler handler ):
    _osType ( osType ),
    _handler ( handler )
{
    // NOTHING
}

eGXCompareResult GXOSMessageNode::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
    const GXOSMessageNode& aValue = static_cast<const GXOSMessageNode&> ( a );
    const GXOSMessageNode& bValue = static_cast<const GXOSMessageNode&> ( b );

    if ( aValue._osType < bValue._osType )
        return eGXCompareResult::Less;

    return aValue._osType > bValue._osType ? eGXCompareResult::Greater : eGXCompareResult::Equal;
}

//---------------------------------------------------------------------------------------------------------------------

GXDesktopInput* GXDesktopInput::_instance = nullptr;

GXDesktopInput& GXCALL GXDesktopInput::GetInstance ()
{
    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXDesktopInput" )
        _instance = new GXDesktopInput ();
    }

    return *_instance;
}

GXDesktopInput::~GXDesktopInput ()
{
    Shutdown ();
    _instance = nullptr;
}

GXVoid GXDesktopInput::Start ()
{
    if ( _thread ) return;

    _isLoop = GX_TRUE;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXThread" )
    _thread = new GXThread ( &GXDesktopInput::ThreadFunction, this );
    _thread->Start ();
}

GXVoid GXDesktopInput::Shutdown ()
{
    if ( !_thread ) return;

    _isLoop = GX_FALSE;
    _thread->Join ();

    GXSafeDelete ( _thread );
}

GXVoid GXDesktopInput::BindKeyboardKey ( GXVoid* context, GXKeyHandler handler, eGXKeyboardKey key, eGXButtonState state )
{
    _smartLock.AcquireExclusive ();

    switch ( state )
    {
        case eGXButtonState::Down:
            _keyDownBinds[ static_cast<GXUPointer> ( static_cast<GXUShort> ( key ) ) ].Init ( context, handler );
        break;

        case eGXButtonState::Up:
            _keyUpBinds[ static_cast<GXUPointer> ( static_cast<GXUShort> ( key ) ) ].Init ( context, handler );
        break;
    }

    _smartLock.ReleaseExclusive ();
}

GXVoid GXDesktopInput::UnbindKeyboardKey ( eGXKeyboardKey key, eGXButtonState state )
{
    _smartLock.AcquireExclusive ();

    switch ( state )
    {
        case eGXButtonState::Down:
            _keyDownBinds[ static_cast<GXUPointer> ( static_cast<GXUShort> ( key ) ) ].Reset ();
        break;

        case eGXButtonState::Up:
            _keyUpBinds[ static_cast<GXUPointer> ( static_cast<GXUShort> ( key ) ) ].Reset ();
        break;
    }

    _smartLock.ReleaseExclusive ();
}

GXVoid GXDesktopInput::LockKeyboard ( GXVoid* /*context*/, GXSymbolHandler /*handler*/ )
{

#pragma message ( "TODO >>> GXDesktopInput::LockKeyboard - Implement me!" )

    // TODO
}

GXVoid GXDesktopInput::UnlockKeyboard ()
{

#pragma message ( "TODO >>> GXDesktopInput::UnlockKeyboard - Implement me!" )

    // TODO
}

GXVoid GXDesktopInput::BindMouseMove ( GXVoid* context, GXMouseMoveHandler handler )
{
    _smartLock.AcquireExclusive ();

    _mouseMoveContext = context;
    _mouseMoveHandler = handler;

    _smartLock.ReleaseExclusive ();
}

GXVoid GXDesktopInput::UnbindMouseMove ()
{
    _smartLock.AcquireExclusive ();

    _mouseMoveContext = nullptr;
    _mouseMoveHandler = nullptr;
    _isMouseMoveEvent = GX_FALSE;

    _smartLock.ReleaseExclusive ();
}

GXVoid GXDesktopInput::BindMouseButton ( GXVoid* context, GXMouseButtonHandler handler, eGXMouseButton button, eGXButtonState state )
{
    _smartLock.AcquireExclusive ();

    switch ( state )
    {
        case eGXButtonState::Down:
            _mouseButtonDownBinds[ static_cast<GXUPointer> ( button ) ].Init ( context, handler );
        break;

        case eGXButtonState::Up:
            _mouseButtonUpBinds[ static_cast<GXUPointer> ( button ) ].Init ( context, handler );
        break;
    }

    _smartLock.ReleaseExclusive ();
}

GXVoid GXDesktopInput::UnbindMouseButton ( eGXMouseButton button, eGXButtonState state )
{
    _smartLock.AcquireExclusive ();

    switch ( state )
    {
        case eGXButtonState::Down:
            _mouseButtonDownBinds[ static_cast<GXUPointer> ( button ) ].Reset ();
        break;

        case eGXButtonState::Up:
            _mouseButtonUpBinds[ static_cast<GXUPointer> ( button ) ].Reset ();
        break;
    }

    _smartLock.ReleaseExclusive ();
}

GXVoid GXDesktopInput::BindMouseScroll ( GXVoid* context, GXMouseScrollHandler handler )
{
    _smartLock.AcquireExclusive ();

    _mouseScrollContext = context;
    _mouseScrollHandler = handler;

    _smartLock.ReleaseExclusive ();
}

GXVoid GXDesktopInput::UnbindMouseScroll ()
{
    _smartLock.AcquireExclusive ();

    _mouseScrollContext = nullptr;
    _mouseScrollHandler = nullptr;
    _isMouseScrollEvent = GX_FALSE;
    _mouseScrollTicks = 0;

    _smartLock.ReleaseExclusive ();
}

GXVoid* GXDesktopInput::ProcessOSMessage ( GXVoid* message )
{
    const MSG& osMessage = *static_cast<const MSG*> ( message );
    const GXOSMessageNode* findResult = static_cast<const GXOSMessageNode*> ( _messageTypeMapper.Find ( GXOSMessageNode ( osMessage.message ) ) );

    if ( !findResult )
#pragma message ( "TODO >>> GXDesktopInput::ProcessOSMessage - Restore DefWindowProc." )
        //return reinterpret_cast<GXVoid*> ( static_cast<GXUPointer> ( DefWindowProc ( osMessage.hwnd, osMessage.message, osMessage.wParam, osMessage.lParam ) ) );
        return nullptr;

    GXDesktopInputHandler handler = findResult->_handler;

    _smartLock.AcquireShared ();

    // Note this is C++ syntax for invoke class method.
    GXVoid* result = reinterpret_cast<GXVoid*> ( static_cast<GXUPointer> ( ( this->*handler ) ( osMessage ) ) );

    _smartLock.ReleaseShared ();

    return result;
}

GXDesktopInput::GXDesktopInput ():
    _keyboardMapper ( &GXKeyNode::Compare, GX_FALSE ),
    _messageTypeMapper ( &GXOSMessageNode::Compare, GX_FALSE ),
    _readyKeyActions ( nullptr ),
    _readyMouseActions ( nullptr ),
    _commitedMouseX ( NO_VALUE ),
    _commitedMouseY ( NO_VALUE ),
    _mouseScrollTicks ( 0 ),
    _lastMouseX ( NO_VALUE ),
    _lastMouseY ( NO_VALUE ),
    _mouseMoveHandler ( nullptr ),
    _mouseMoveContext ( nullptr ),
    _isMouseMoveEvent ( GX_FALSE ),
    _mouseScrollHandler ( nullptr ),
    _mouseScrollContext ( nullptr ),
    _isMouseScrollEvent ( GX_FALSE ),
    _isLoop ( GX_FALSE ),
    _thread ( nullptr )
{
    InitBinds ();
    InitActionPool ();
    InitKeyMappers ();
    InitOSMessageMapper ();
}

GXVoid GXDesktopInput::AddAction ( const GXKeyBind &bind )
{
    // Note actions will be placed in reverse order.
    // It is assumed that order does not matter.

    GXInputKeyAction* newAction = _freeKeyActions;
    _freeKeyActions = _freeKeyActions->_next;

    newAction->_bind = bind;
    newAction->_next = _readyKeyActions;
    _readyKeyActions = newAction;
}

GXVoid GXDesktopInput::AddAction ( const GXMouseButtonBind &bind )
{
    // Note actions will be placed in reverse order.
    // It is assumed that order does not matter.

    GXInputMouseButtonAction* newAction = _freeMouseButtonActions;
    _freeMouseButtonActions = _freeMouseButtonActions->_next;

    newAction->_bind = bind;
    newAction->_next = _readyMouseActions;
    _readyMouseActions = newAction;
}

GXVoid GXDesktopInput::ExecuteKeyEvents ()
{
    if ( !_readyKeyActions ) return;

    GXInputKeyAction* tail = nullptr;

    while ( _readyKeyActions )
    {
        tail = _readyKeyActions;

        const GXKeyBind& bind = _readyKeyActions->_bind;
        bind._handler ( bind._context );

        _readyKeyActions = _readyKeyActions->_next;
    }

    // Returning key actions to event pool.
    tail->_next = _freeKeyActions;
    _freeKeyActions = tail;
}

GXVoid GXDesktopInput::ExecuteMouseButtonEvents ()
{
    if ( !_readyMouseActions ) return;

    GXInputMouseButtonAction* tail = nullptr;

    while ( _readyMouseActions )
    {
        tail = _readyMouseActions;

        const GXMouseButtonBind& bind = _readyMouseActions->_bind;
        bind._handler ( bind._context, _lastMouseX, _lastMouseY );

        _readyMouseActions = _readyMouseActions->_next;
    }

    // Returning mouse button actions to event pool.
    tail->_next = _freeMouseButtonActions;
    _freeMouseButtonActions = tail;
}

GXVoid GXDesktopInput::ExecuteMouseMoveEvents ()
{
    if ( !_isMouseMoveEvent ) return;

    _isMouseMoveEvent = GX_FALSE;

    if ( _mouseMoveHandler )
        _mouseMoveHandler ( _mouseMoveContext, _lastMouseX, _lastMouseY, _lastMouseX - _commitedMouseX, _lastMouseY - _commitedMouseY );

    _commitedMouseX = _lastMouseX;
    _commitedMouseY = _lastMouseY;
}

GXVoid GXDesktopInput::ExecuteMouseScrollEvents ()
{
    if ( !_isMouseScrollEvent ) return;

    _isMouseScrollEvent = GX_FALSE;

    if ( _mouseScrollHandler && _mouseScrollTicks != 0 )
        _mouseScrollHandler ( _mouseScrollContext, _mouseScrollTicks, _lastMouseX, _lastMouseY );

    _mouseScrollTicks = 0;
}

GXVoid GXDesktopInput::InitActionPool ()
{
    constexpr const GXUPointer lastKey = sizeof ( _keyActionPool ) / sizeof ( *_keyActionPool ) - 1u;

    for ( GXUPointer i = 0u; i < lastKey; ++i )
        _keyActionPool[ i ]._next = _keyActionPool + ( i + 1u );

    _keyActionPool[ lastKey ]._next = nullptr;
    _freeKeyActions = _keyActionPool;

    constexpr const GXUPointer lastMouseButton = sizeof ( _mouseButtonActionPool ) / sizeof ( *_mouseButtonActionPool ) - 1u;

    for ( GXUPointer i = 0u; i < lastMouseButton; ++i )
        _mouseButtonActionPool[ i ]._next = _mouseButtonActionPool + ( i + 1u );

    _mouseButtonActionPool[ lastMouseButton ]._next = nullptr;
    _freeMouseButtonActions = _mouseButtonActionPool;
}

GXVoid GXDesktopInput::InitBinds ()
{
    memset ( _keyDownBinds, 0, sizeof ( _keyDownBinds ) );
    memset ( _keyUpBinds, 0, sizeof ( _keyUpBinds ) );

    memset ( _mouseButtonDownBinds, 0, sizeof ( _mouseButtonDownBinds ) );
    memset ( _mouseButtonUpBinds, 0, sizeof ( _mouseButtonUpBinds ) );
}

GXVoid GXDesktopInput::InitKeyMappers ()
{
    ::new ( _keyMap ) GXKeyNode ( VK_ESCAPE, eGXKeyboardKey::Esc );
    _keyboardMapper.Add ( _keyMap[ 0u ] );

    ::new ( _keyMap + 1u ) GXKeyNode ( VK_KEY_0, eGXKeyboardKey::Zero );
    _keyboardMapper.Add ( _keyMap[ 1u ] );

    ::new ( _keyMap + 2u ) GXKeyNode ( VK_KEY_1, eGXKeyboardKey::One );
    _keyboardMapper.Add ( _keyMap[ 2u ] );

    ::new ( _keyMap + 3u ) GXKeyNode ( VK_KEY_2, eGXKeyboardKey::Two );
    _keyboardMapper.Add ( _keyMap[ 3u ] );

    ::new ( _keyMap + 4u ) GXKeyNode ( VK_KEY_3, eGXKeyboardKey::Three );
    _keyboardMapper.Add ( _keyMap[ 4u ] );

    ::new ( _keyMap + 5u ) GXKeyNode ( VK_KEY_4, eGXKeyboardKey::Four );
    _keyboardMapper.Add ( _keyMap[ 5u ] );

    ::new ( _keyMap + 6u ) GXKeyNode ( VK_KEY_5, eGXKeyboardKey::Five );
    _keyboardMapper.Add ( _keyMap[ 6u ] );

    ::new ( _keyMap + 7u ) GXKeyNode ( VK_KEY_6, eGXKeyboardKey::Six );
    _keyboardMapper.Add ( _keyMap[ 7u ] );

    ::new ( _keyMap + 8u ) GXKeyNode ( VK_KEY_7, eGXKeyboardKey::Seven );
    _keyboardMapper.Add ( _keyMap[ 8u ] );

    ::new ( _keyMap + 9u ) GXKeyNode ( VK_KEY_8, eGXKeyboardKey::Eight );
    _keyboardMapper.Add ( _keyMap[ 9u ] );

    ::new ( _keyMap + 10u ) GXKeyNode ( VK_KEY_9, eGXKeyboardKey::Nine );
    _keyboardMapper.Add ( _keyMap[ 10u ] );

    ::new ( _keyMap + 11u ) GXKeyNode ( VK_KEY_A, eGXKeyboardKey::A );
    _keyboardMapper.Add ( _keyMap[ 11u ] );

    ::new ( _keyMap + 12u ) GXKeyNode ( VK_KEY_B, eGXKeyboardKey::B );
    _keyboardMapper.Add ( _keyMap[ 12u ] );

    ::new ( _keyMap + 13u ) GXKeyNode ( VK_KEY_C, eGXKeyboardKey::C );
    _keyboardMapper.Add ( _keyMap[ 13u ] );

    ::new ( _keyMap + 14u ) GXKeyNode ( VK_KEY_D, eGXKeyboardKey::D );
    _keyboardMapper.Add ( _keyMap[ 14u ] );

    ::new ( _keyMap + 15u ) GXKeyNode ( VK_KEY_E, eGXKeyboardKey::E );
    _keyboardMapper.Add ( _keyMap[ 15u ] );

    ::new ( _keyMap + 16u ) GXKeyNode ( VK_KEY_F, eGXKeyboardKey::F );
    _keyboardMapper.Add ( _keyMap[ 16u ] );

    ::new ( _keyMap + 17u ) GXKeyNode ( VK_KEY_G, eGXKeyboardKey::G );
    _keyboardMapper.Add ( _keyMap[ 17u ] );

    ::new ( _keyMap + 18u ) GXKeyNode ( VK_KEY_H, eGXKeyboardKey::H );
    _keyboardMapper.Add ( _keyMap[ 18u ] );

    ::new ( _keyMap + 19u ) GXKeyNode ( VK_KEY_I, eGXKeyboardKey::I );
    _keyboardMapper.Add ( _keyMap[ 19u ] );

    ::new ( _keyMap + 20u ) GXKeyNode ( VK_KEY_J, eGXKeyboardKey::J );
    _keyboardMapper.Add ( _keyMap[ 20u ] );

    ::new ( _keyMap + 21u ) GXKeyNode ( VK_KEY_K, eGXKeyboardKey::K );
    _keyboardMapper.Add ( _keyMap[ 21u ] );

    ::new ( _keyMap + 22u ) GXKeyNode ( VK_KEY_L, eGXKeyboardKey::L );
    _keyboardMapper.Add ( _keyMap[ 22u ] );

    ::new ( _keyMap + 23u ) GXKeyNode ( VK_KEY_M, eGXKeyboardKey::M );
    _keyboardMapper.Add ( _keyMap[ 23u ] );

    ::new ( _keyMap + 24u ) GXKeyNode ( VK_KEY_N, eGXKeyboardKey::N );
    _keyboardMapper.Add ( _keyMap[ 24u ] );

    ::new ( _keyMap + 25u ) GXKeyNode ( VK_KEY_O, eGXKeyboardKey::O );
    _keyboardMapper.Add ( _keyMap[ 25u ] );

    ::new ( _keyMap + 26u ) GXKeyNode ( VK_KEY_P, eGXKeyboardKey::P );
    _keyboardMapper.Add ( _keyMap[ 26u ] );

    ::new ( _keyMap + 27u ) GXKeyNode ( VK_KEY_Q, eGXKeyboardKey::Q );
    _keyboardMapper.Add ( _keyMap[ 27u ] );

    ::new ( _keyMap + 28u ) GXKeyNode ( VK_KEY_R, eGXKeyboardKey::R );
    _keyboardMapper.Add ( _keyMap[ 28u ] );

    ::new ( _keyMap + 29u ) GXKeyNode ( VK_KEY_S, eGXKeyboardKey::S );
    _keyboardMapper.Add ( _keyMap[ 29u ] );

    ::new ( _keyMap + 30u ) GXKeyNode ( VK_KEY_T, eGXKeyboardKey::T );
    _keyboardMapper.Add ( _keyMap[ 30u ] );

    ::new ( _keyMap + 31u ) GXKeyNode ( VK_KEY_U, eGXKeyboardKey::U );
    _keyboardMapper.Add ( _keyMap[ 31u ] );

    ::new ( _keyMap + 32u ) GXKeyNode ( VK_KEY_V, eGXKeyboardKey::V );
    _keyboardMapper.Add ( _keyMap[ 32u ] );

    ::new ( _keyMap + 33u ) GXKeyNode ( VK_KEY_W, eGXKeyboardKey::W );
    _keyboardMapper.Add ( _keyMap[ 33u ] );

    ::new ( _keyMap + 34u ) GXKeyNode ( VK_KEY_X, eGXKeyboardKey::X );
    _keyboardMapper.Add ( _keyMap[ 34u ] );

    ::new ( _keyMap + 35u ) GXKeyNode ( VK_KEY_Y, eGXKeyboardKey::Y );
    _keyboardMapper.Add ( _keyMap[ 35u ] );

    ::new ( _keyMap + 36u ) GXKeyNode ( VK_KEY_Z, eGXKeyboardKey::Z );
    _keyboardMapper.Add ( _keyMap[ 36u ] );

    ::new ( _keyMap + 37u ) GXKeyNode ( VK_F1, eGXKeyboardKey::F1 );
    _keyboardMapper.Add ( _keyMap[ 37u ] );

    ::new ( _keyMap + 38u ) GXKeyNode ( VK_F2, eGXKeyboardKey::F2 );
    _keyboardMapper.Add ( _keyMap[ 38u ] );

    ::new ( _keyMap + 39u ) GXKeyNode ( VK_F3, eGXKeyboardKey::F3 );
    _keyboardMapper.Add ( _keyMap[ 39u ] );

    ::new ( _keyMap + 40u ) GXKeyNode ( VK_F4, eGXKeyboardKey::F4 );
    _keyboardMapper.Add ( _keyMap[ 40u ] );

    ::new ( _keyMap + 41u ) GXKeyNode ( VK_F5, eGXKeyboardKey::F5 );
    _keyboardMapper.Add ( _keyMap[ 41u ] );

    ::new ( _keyMap + 42u ) GXKeyNode ( VK_F6, eGXKeyboardKey::F6 );
    _keyboardMapper.Add ( _keyMap[ 42u ] );

    ::new ( _keyMap + 43u ) GXKeyNode ( VK_F7, eGXKeyboardKey::F7 );
    _keyboardMapper.Add ( _keyMap[ 43u ] );

    ::new ( _keyMap + 44u ) GXKeyNode ( VK_F8, eGXKeyboardKey::F8 );
    _keyboardMapper.Add ( _keyMap[ 44u ] );

    ::new ( _keyMap + 45u ) GXKeyNode ( VK_F9, eGXKeyboardKey::F9 );
    _keyboardMapper.Add ( _keyMap[ 45u ] );

    ::new ( _keyMap + 46u ) GXKeyNode ( VK_F10, eGXKeyboardKey::F10 );
    _keyboardMapper.Add ( _keyMap[ 46u ] );

    ::new ( _keyMap + 47u ) GXKeyNode ( VK_F11, eGXKeyboardKey::F11 );
    _keyboardMapper.Add ( _keyMap[ 47u ] );

    ::new ( _keyMap + 48u ) GXKeyNode ( VK_F12, eGXKeyboardKey::F12 );
    _keyboardMapper.Add ( _keyMap[ 48u ] );

    ::new ( _keyMap + 49u ) GXKeyNode ( VK_PRIOR, eGXKeyboardKey::PageUp );
    _keyboardMapper.Add ( _keyMap[ 49u ] );

    ::new ( _keyMap + 50u ) GXKeyNode ( VK_NEXT, eGXKeyboardKey::PageDown );
    _keyboardMapper.Add ( _keyMap[ 50u ] );

    ::new ( _keyMap + 51u ) GXKeyNode ( VK_HOME, eGXKeyboardKey::Home );
    _keyboardMapper.Add ( _keyMap[ 51u ] );

    ::new ( _keyMap + 52u ) GXKeyNode ( VK_END, eGXKeyboardKey::End );
    _keyboardMapper.Add ( _keyMap[ 52u ] );

    ::new ( _keyMap + 53u ) GXKeyNode ( VK_INSERT, eGXKeyboardKey::Insert );
    _keyboardMapper.Add ( _keyMap[ 53u ] );

    ::new ( _keyMap + 54u ) GXKeyNode ( VK_DELETE, eGXKeyboardKey::Delete );
    _keyboardMapper.Add ( _keyMap[ 54u ] );

    ::new ( _keyMap + 55u ) GXKeyNode ( VK_SPACE, eGXKeyboardKey::Space );
    _keyboardMapper.Add ( _keyMap[ 55u ] );

    ::new ( _keyMap + 56u ) GXKeyNode ( VK_CAPITAL, eGXKeyboardKey::CapsLock );
    _keyboardMapper.Add ( _keyMap[ 56u ] );

    ::new ( _keyMap + 57u ) GXKeyNode ( VK_TAB, eGXKeyboardKey::Tab );
    _keyboardMapper.Add ( _keyMap[ 57u ] );

    ::new ( _keyMap + 58u ) GXKeyNode ( VK_OEM_3, eGXKeyboardKey::Tilde );
    _keyboardMapper.Add ( _keyMap[ 58u ] );

    ::new ( _keyMap + 59u ) GXKeyNode ( VK_OEM_MINUS, eGXKeyboardKey::Minus );
    _keyboardMapper.Add ( _keyMap[ 59u ] );

    ::new ( _keyMap + 60u ) GXKeyNode ( VK_OEM_PLUS, eGXKeyboardKey::Equal );
    _keyboardMapper.Add ( _keyMap[ 60u ] );

    ::new ( _keyMap + 61u ) GXKeyNode ( VK_BACK, eGXKeyboardKey::Backspace );
    _keyboardMapper.Add ( _keyMap[ 61u ] );

    ::new ( _keyMap + 62u ) GXKeyNode ( VK_OEM_4, eGXKeyboardKey::OpeningSquareBracket );
    _keyboardMapper.Add ( _keyMap[ 62u ] );

    ::new ( _keyMap + 63u ) GXKeyNode ( VK_OEM_6, eGXKeyboardKey::ClosingSquareBracket );
    _keyboardMapper.Add ( _keyMap[ 63u ] );

    ::new ( _keyMap + 64u ) GXKeyNode ( VK_OEM_5, eGXKeyboardKey::BackSlash );
    _keyboardMapper.Add ( _keyMap[ 64u ] );

    ::new ( _keyMap + 65u ) GXKeyNode ( VK_OEM_1, eGXKeyboardKey::Semicolon );
    _keyboardMapper.Add ( _keyMap[ 65u ] );

    ::new ( _keyMap + 66u ) GXKeyNode ( VK_OEM_7, eGXKeyboardKey::Quote );
    _keyboardMapper.Add ( _keyMap[ 66u ] );

    ::new ( _keyMap + 67u ) GXKeyNode ( VK_RETURN, eGXKeyboardKey::Enter );
    _keyboardMapper.Add ( _keyMap[ 67u ] );

    ::new ( _keyMap + 68u ) GXKeyNode ( VK_OEM_COMMA, eGXKeyboardKey::Less );
    _keyboardMapper.Add ( _keyMap[ 68u ] );

    ::new ( _keyMap + 69u ) GXKeyNode ( VK_OEM_PERIOD, eGXKeyboardKey::Greater );
    _keyboardMapper.Add ( _keyMap[ 69u ] );

    ::new ( _keyMap + 70u ) GXKeyNode ( VK_OEM_2, eGXKeyboardKey::Slash );
    _keyboardMapper.Add ( _keyMap[ 70u ] );

    ::new ( _keyMap + 71u ) GXKeyNode ( VK_LEFT, eGXKeyboardKey::Left );
    _keyboardMapper.Add ( _keyMap[ 71u ] );

    ::new ( _keyMap + 72u ) GXKeyNode ( VK_UP, eGXKeyboardKey::Up );
    _keyboardMapper.Add ( _keyMap[ 72u ] );

    ::new ( _keyMap + 73u ) GXKeyNode ( VK_RIGHT, eGXKeyboardKey::Right );
    _keyboardMapper.Add ( _keyMap[ 73u ] );

    ::new ( _keyMap + 74u ) GXKeyNode ( VK_DOWN, eGXKeyboardKey::Down );
    _keyboardMapper.Add ( _keyMap[ 74u ] );

    ::new ( _keyMap + 75u ) GXKeyNode ( VK_NUMPAD0, eGXKeyboardKey::Numpad0 );
    _keyboardMapper.Add ( _keyMap[ 75u ] );

    ::new ( _keyMap + 76u ) GXKeyNode ( VK_NUMPAD1, eGXKeyboardKey::Numpad1 );
    _keyboardMapper.Add ( _keyMap[ 76u ] );

    ::new ( _keyMap + 77u ) GXKeyNode ( VK_NUMPAD2, eGXKeyboardKey::Numpad2 );
    _keyboardMapper.Add ( _keyMap[ 77u ] );

    ::new ( _keyMap + 78u ) GXKeyNode ( VK_NUMPAD3, eGXKeyboardKey::Numpad3 );
    _keyboardMapper.Add ( _keyMap[ 78u ] );

    ::new ( _keyMap + 79u ) GXKeyNode ( VK_NUMPAD4, eGXKeyboardKey::Numpad4 );
    _keyboardMapper.Add ( _keyMap[ 79u ] );

    ::new ( _keyMap + 80u ) GXKeyNode ( VK_NUMPAD5, eGXKeyboardKey::Numpad5 );
    _keyboardMapper.Add ( _keyMap[ 80u ] );

    ::new ( _keyMap + 81u ) GXKeyNode ( VK_NUMPAD6, eGXKeyboardKey::Numpad6 );
    _keyboardMapper.Add ( _keyMap[ 81u ] );

    ::new ( _keyMap + 82u ) GXKeyNode ( VK_NUMPAD7, eGXKeyboardKey::Numpad7 );
    _keyboardMapper.Add ( _keyMap[ 82u ] );

    ::new ( _keyMap + 83u ) GXKeyNode ( VK_NUMPAD8, eGXKeyboardKey::Numpad8 );
    _keyboardMapper.Add ( _keyMap[ 83u ] );

    ::new ( _keyMap + 84u ) GXKeyNode ( VK_NUMPAD9, eGXKeyboardKey::Numpad9 );
    _keyboardMapper.Add ( _keyMap[ 84u ] );

    ::new ( _keyMap + 85u ) GXKeyNode ( VK_DECIMAL, eGXKeyboardKey::NumpadDot );
    _keyboardMapper.Add ( _keyMap[ 85u ] );

    ::new ( _keyMap + 86u ) GXKeyNode ( VK_SEPARATOR, eGXKeyboardKey::NumpadEnter );
    _keyboardMapper.Add ( _keyMap[ 86u ] );

    ::new ( _keyMap + 87u ) GXKeyNode ( VK_ADD, eGXKeyboardKey::NumpadPlus );
    _keyboardMapper.Add ( _keyMap[ 87u ] );

    ::new ( _keyMap + 88u ) GXKeyNode ( VK_SUBTRACT, eGXKeyboardKey::NumpadMinus );
    _keyboardMapper.Add ( _keyMap[ 88u ] );

    ::new ( _keyMap + 89u ) GXKeyNode ( VK_MULTIPLY, eGXKeyboardKey::NumpadMultiply );
    _keyboardMapper.Add ( _keyMap[ 89u ] );

    ::new ( _keyMap + 90u ) GXKeyNode ( VK_DIVIDE, eGXKeyboardKey::NumpadDivide );
    _keyboardMapper.Add ( _keyMap[ 90u ] );

    ::new ( _keyMap + 91u ) GXKeyNode ( VK_NUMLOCK, eGXKeyboardKey::NumLock );
    _keyboardMapper.Add ( _keyMap[ 91u ] );

    ::new ( _keyMap + 92u ) GXKeyNode ( VK_APPS, eGXKeyboardKey::ContextMenu );
    _keyboardMapper.Add ( _keyMap[ 92u ] );

    ::new ( _keyMap + 93u ) GXKeyNode ( VK_SCROLL, eGXKeyboardKey::ScrollLock );
    _keyboardMapper.Add ( _keyMap[ 93u ] );

    ::new ( _keyMap + 94u ) GXKeyNode ( VK_PAUSE, eGXKeyboardKey::Pause );
    _keyboardMapper.Add ( _keyMap[ 94u ] );

    ::new ( _keyMap + 95u ) GXKeyNode ( VK_LSHIFT, eGXKeyboardKey::LeftShift );
    _keyboardMapper.Add ( _keyMap[ 95u ] );

    ::new ( _keyMap + 96u ) GXKeyNode ( VK_LCONTROL, eGXKeyboardKey::LeftCtrl );
    _keyboardMapper.Add ( _keyMap[ 96u ] );

    ::new ( _keyMap + 97u ) GXKeyNode ( VK_LMENU, eGXKeyboardKey::LeftAlt );
    _keyboardMapper.Add ( _keyMap[ 97u ] );

    ::new ( _keyMap + 98u ) GXKeyNode ( VK_RMENU, eGXKeyboardKey::RightAlt );
    _keyboardMapper.Add ( _keyMap[ 98u ] );

    ::new ( _keyMap + 99u ) GXKeyNode ( VK_RCONTROL, eGXKeyboardKey::RightCtrl );
    _keyboardMapper.Add ( _keyMap[ 99u ] );

    ::new ( _keyMap + 100u ) GXKeyNode ( VK_RSHIFT, eGXKeyboardKey::RightShift );
    _keyboardMapper.Add ( _keyMap[ 100u ] );
}

GXVoid GXDesktopInput::InitOSMessageMapper ()
{
    ::new ( _osMessageMap ) GXOSMessageNode ( WM_LBUTTONDOWN, &GXDesktopInput::HandleMouseLeftButtonDown );
    _messageTypeMapper.Add ( _osMessageMap[ 0u ] );

    ::new ( _osMessageMap + 1u ) GXOSMessageNode ( WM_LBUTTONUP, &GXDesktopInput::HandleMouseLeftButtonUp );
    _messageTypeMapper.Add ( _osMessageMap[ 1u ] );

    ::new ( _osMessageMap + 2u ) GXOSMessageNode ( WM_MBUTTONDOWN, &GXDesktopInput::HandleMouseMiddleButtonDown );
    _messageTypeMapper.Add ( _osMessageMap[ 2u ] );

    ::new ( _osMessageMap + 3u ) GXOSMessageNode ( WM_MBUTTONUP, &GXDesktopInput::HandleMouseMiddleButtonUp );
    _messageTypeMapper.Add ( _osMessageMap[ 3u ] );

    ::new ( _osMessageMap + 4u ) GXOSMessageNode ( WM_RBUTTONDOWN, &GXDesktopInput::HandleMouseRightButtonDown );
    _messageTypeMapper.Add ( _osMessageMap[ 4u ] );

    ::new ( _osMessageMap + 5u ) GXOSMessageNode ( WM_RBUTTONUP, &GXDesktopInput::HandleMouseRightButtonUp );
    _messageTypeMapper.Add ( _osMessageMap[ 5u ] );

    ::new ( _osMessageMap + 6u ) GXOSMessageNode ( WM_MOUSEWHEEL, &GXDesktopInput::HandleMouseScroll );
    _messageTypeMapper.Add ( _osMessageMap[ 6u ] );

    ::new ( _osMessageMap + 7u ) GXOSMessageNode ( WM_MOUSEMOVE, &GXDesktopInput::HandleMouseMove );
    _messageTypeMapper.Add ( _osMessageMap[ 7u ] );

    ::new ( _osMessageMap + 8u ) GXOSMessageNode ( WM_KEYDOWN, &GXDesktopInput::HandleKeyDown );
    _messageTypeMapper.Add ( _osMessageMap[ 8u ] );

    ::new ( _osMessageMap + 9u ) GXOSMessageNode ( WM_SYSKEYDOWN, &GXDesktopInput::HandleKeyDown );
    _messageTypeMapper.Add ( _osMessageMap[ 9u ] );

    ::new ( _osMessageMap + 10u ) GXOSMessageNode ( WM_KEYUP, &GXDesktopInput::HandleKeyUp );
    _messageTypeMapper.Add ( _osMessageMap[ 10u ] );

    ::new ( _osMessageMap + 11u ) GXOSMessageNode ( WM_SYSKEYUP, &GXDesktopInput::HandleKeyUp );
    _messageTypeMapper.Add ( _osMessageMap[ 11u ] );
}

LRESULT GXDesktopInput::HandleKeyInternal ( GXKeyBind const* const& allBinds, const MSG &message, eGXButtonState ignoreIfEqual )
{
    // Note Windows OS spams key down events if user holds key.
    // It is not very useful for key bind cases, but it is useful for typing cases.
    // Fortunately Windows OS provides previous key state. This is very nice for antispam strategy.
    // It is 30 bit in "message.lParam". Note that cheking logic is working transparent for key up event too.
    // See https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
    // See https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-syskeydown
    constexpr const LPARAM previousStateMask = 1 << 30;
    const eGXButtonState previousState = ( message.lParam & previousStateMask ) ? eGXButtonState::Down : eGXButtonState::Up;

    if ( ignoreIfEqual == previousState )
        return 0;

    const WPARAM virtualCode = ResolveNativeKeyVirtualCode ( message );

    if ( virtualCode == SKIP_KEY_VIRTUAL_CODE )
        return 0;

    const GXKeyNode* findResult = static_cast<const GXKeyNode*> ( _keyboardMapper.Find ( GXKeyNode ( virtualCode ) ) );

    if ( !findResult )
    {
        GXLogA ( "GXDesktopInput::HandleKeyInternal::Warning - Can't resolve virtual key code %zu.\n", static_cast<GXUPointer> ( virtualCode ) );
        return 0;
    }

    const GXKeyBind& bind = allBinds[ static_cast<GXUPointer> ( findResult->_key ) ];

    if ( bind._handler )
        AddAction ( bind );

    return 0;
}

LRESULT GXDesktopInput::HandleMouseButtonInternal ( GXMouseButtonBind const* const& allBinds, eGXMouseButton button, const MSG &message )
{
    const GXMouseButtonBind& bind = allBinds[ static_cast<GXUPointer> ( button ) ];

    if ( !bind._handler )
        return 0;

    _lastMouseX = static_cast<const GXInt> ( LOWORD ( message.lParam ) );
    _lastMouseY = GXRenderer::GetInstance ().GetHeight () - static_cast<const GXInt> ( HIWORD ( message.lParam ) );
    AddAction ( bind );

    return 0;
}

LRESULT GXDesktopInput::HandleKeyDown ( const MSG &message )
{
    return HandleKeyInternal ( _keyDownBinds, message, eGXButtonState::Down );
}

LRESULT GXDesktopInput::HandleKeyUp ( const MSG &message )
{
    return HandleKeyInternal ( _keyUpBinds, message, eGXButtonState::Up );
}

LRESULT GXDesktopInput::HandleMouseLeftButtonDown ( const MSG &message )
{
    return HandleMouseButtonInternal ( _mouseButtonDownBinds, eGXMouseButton::LeftButton, message );
}

LRESULT GXDesktopInput::HandleMouseLeftButtonUp ( const MSG &message )
{
    return HandleMouseButtonInternal ( _mouseButtonUpBinds, eGXMouseButton::LeftButton, message );
}

LRESULT GXDesktopInput::HandleMouseMiddleButtonDown ( const MSG &message )
{
    return HandleMouseButtonInternal ( _mouseButtonDownBinds, eGXMouseButton::MiddleButton, message );
}

LRESULT GXDesktopInput::HandleMouseMiddleButtonUp ( const MSG &message )
{
    return HandleMouseButtonInternal ( _mouseButtonUpBinds, eGXMouseButton::MiddleButton, message );
}

LRESULT GXDesktopInput::HandleMouseRightButtonDown ( const MSG &message )
{
    return HandleMouseButtonInternal ( _mouseButtonDownBinds, eGXMouseButton::RightButton, message );
}

LRESULT GXDesktopInput::HandleMouseRightButtonUp ( const MSG &message )
{
    return HandleMouseButtonInternal ( _mouseButtonUpBinds, eGXMouseButton::RightButton, message );
}

LRESULT GXDesktopInput::HandleMouseMove ( const MSG &message )
{
    _lastMouseX = static_cast<const GXInt> ( LOWORD ( message.lParam ) );
    _lastMouseY = GXRenderer::GetInstance ().GetHeight () - static_cast<const GXInt> ( HIWORD ( message.lParam ) );

    if ( !_mouseMoveHandler )
        return 0;

    _isMouseMoveEvent = GX_TRUE;
    return 0;
}

LRESULT GXDesktopInput::HandleMouseScroll ( const MSG &message )
{
    if ( !_mouseScrollHandler )
        return 0;

    POINT posRaw;
    posRaw.x = GET_X_LPARAM ( message.lParam );
    posRaw.y = GET_Y_LPARAM ( message.lParam );
    ScreenToClient ( message.hwnd, &posRaw );

    _isMouseScrollEvent = GX_TRUE;
    _lastMouseX = static_cast<const GXInt> ( posRaw.x );
    _lastMouseY = GXRenderer::GetInstance ().GetHeight () - static_cast<const GXInt> ( posRaw.y );
    _mouseScrollTicks += static_cast<const GXInt> ( GET_WHEEL_DELTA_WPARAM ( message.wParam ) / WHEEL_DELTA );

    return 0;
}

WPARAM GXDesktopInput::ResolveNativeKeyVirtualCode ( const MSG &message ) const
{
    // Implementation is based on some ideas https://gamedev.stackexchange.com/questions/1859/handling-keyboard-and-mouse-input-win-api/1868#1868

    // Tricky part is to detect left|right ALT|CTRL|SHIFT buttons, ENTER|Numpad ENTER buttons.
    // Also GXEngine has to ignore PRINT SCREEN events.
    // Left and right shift buttons have different scancodes, but they can not be detected via extended flag in "message.lParam".
    // CTRL and ALT buttons could be resolved via extended flag in "message.lParam".
    // And finaly ALT|CTRL|SHIFT buttons have unique value in "message.wParam".

    const WPARAM virtualCode = message.wParam;

    if ( virtualCode == VK_SNAPSHOT )
    {
        // Alright. This is PRINT SCREEN button. Ignore it!
        return SKIP_KEY_VIRTUAL_CODE;
    }

    // See https://gamedev.stackexchange.com/questions/1859/handling-keyboard-and-mouse-input-win-api/1868#1868
    // And see https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/keyCode
    constexpr const WPARAM altTrait = 18u;
    constexpr const WPARAM ctrlTrait = 17u;
    constexpr const WPARAM shiftTrait = 16u;

    if ( virtualCode != altTrait && virtualCode != ctrlTrait && virtualCode != shiftTrait && virtualCode != VK_RETURN )
    {
        // Great! This is not a modifier keyboard button or ENTER|Numpad ENTER button. So simple return virtual code value in "message.wParam".
        return virtualCode;
    }

    // Checking SHIFT buttons firts.

    if ( virtualCode == shiftTrait )
    {
        // Unfortunatelly right|left SHIFT can not be detected via extended state.
        // But they can be detected via scancode info.

        // 16-23 bits.
        // See https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
        // See https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keyup
        constexpr const LPARAM scancodeMask = 0xFF0000;

        // Note that these values were revealed after local experimentation.
        // Please make sure that this strategy is correct for any Windows OS.
        constexpr const LPARAM leftShiftScancode = 0x2A0000;
        constexpr const LPARAM rightShiftScancode = 0x360000;

        switch ( message.lParam & scancodeMask )
        {
            case leftShiftScancode:
            return static_cast<WPARAM> ( VK_LSHIFT );

            case rightShiftScancode:
            return static_cast<WPARAM> ( VK_RSHIFT );

            default:
                GXLogA ( "GXDesktopInput::ResolveNativeKeyVirtualCode::Error - Shift detection was failed!" );
            return SKIP_KEY_VIRTUAL_CODE;
        }
    }

    // Next checking CTRL buttons.

    // Just a little bit about an extended state, e.g. a right hand ALT or CTRL.
    // See https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
    // See https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-syskeydown
    constexpr const LPARAM extendedMask = 1 << 24;
    const GXBool isExtended = ( message.lParam & extendedMask ) != 0;

    if ( virtualCode == ctrlTrait )
        return static_cast<WPARAM> ( isExtended ? VK_RCONTROL : VK_LCONTROL );

    // Next checking ENTER|Numpad ENTER buttons.
    // Actually extended enter is numpad ENTER.

    if ( virtualCode == VK_RETURN )
        return static_cast<WPARAM> ( isExtended ? VK_SEPARATOR : VK_RETURN );

    // Duh, this is some of ALT buttons, obviously.
    return static_cast<WPARAM> ( isExtended ? VK_RMENU : VK_LMENU );
}

GXUPointer GXTHREADCALL GXDesktopInput::ThreadFunction ( GXVoid* argument, GXThread &thread )
{
    GXDesktopInput& input = *static_cast<GXDesktopInput*> ( argument );

    while ( input._isLoop )
    {
        input._smartLock.AcquireExclusive ();

        input.ExecuteMouseMoveEvents ();
        input.ExecuteMouseScrollEvents ();
        input.ExecuteMouseButtonEvents ();
        input.ExecuteKeyEvents ();

        input._smartLock.ReleaseExclusive ();

        thread.Sleep ( THREAD_TICK_TIMEOUT );
    }

    return 0u;
}
