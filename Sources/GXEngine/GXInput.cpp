// version 1.20

#include <GXEngine/GXInput.h>
#include <GXEngine/GXCore.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXLogger.h>
#include <GXEngineDLL/GXEngineAPI.h>

GX_DISABLE_COMMON_WARNINGS

#include <Windowsx.h>

GX_RESTORE_WARNING_STATE


#define    INPUT_THREAD_SLEEP       40u

#define INPUT_INV_STICK_VALUE       3.0519e-5f
#define INPUT_INV_TRIGGER_VALUE     3.9216e-3f


extern HMODULE gx_GXEngineDLLModuleHandle;


GXThread*                   GXInput::_thread = nullptr;
GXBool                      GXInput::_loopFlag = false;

GXVoid*                     GXInput::_keysHandlers[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
PFNGXKEYPROC                GXInput::_KeysMapping[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
GXBool                      GXInput::_keysMask[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];

GXVoid*                     GXInput::_onTypeHandler = nullptr;
PFNGXTYPEPROC               GXInput::_onType = nullptr;
GXWChar                     GXInput::_symbol;

XINPUT_STATE                GXInput::_gamepadState[ 2u ];
GXUByte                     GXInput::_currentGamepadState;

GXVoid*                     GXInput::_gamepadKeysHandlers[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];
PFNGXKEYPROC                GXInput::_gamepadKeysMapping[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];
GXByte                      GXInput::_gamepadKeysMask[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];

GXVoid*                     GXInput::_onLeftTriggerHandler = nullptr;
PFNGXTRIGGERPROC            GXInput::_doLeftTrigger = nullptr;

GXVoid*                     GXInput::_onRightTriggerHandler = nullptr;
PFNGXTRIGGERPROC            GXInput::_doRightTrigger = nullptr;

GXVoid*                     GXInput::_onLeftStickHandler = nullptr;
PFNGXSTICKPROC              GXInput::_doLeftStick = nullptr;

GXVoid*                     GXInput::_onRightStickTrigger = nullptr;
PFNGXSTICKPROC              GXInput::_doRightStick = nullptr;

GXVoid*                     GXInput::_onMouseMoveHandler = nullptr;
PFNGXMOUSEMOVEPROC          GXInput::_doMouseMoving = nullptr;

GXVoid*                     GXInput::_onMouseButtonHandler = nullptr;
PFNGXMOUSEBUTTONSPROC       GXInput::_doMouseButton = nullptr;

GXVoid*                     GXInput::_onMouseWheelHandler = nullptr;
PFNGXMOUSEWHEELPROC         GXInput::_doMouseWheel = nullptr;

GXInputMouseFlags           GXInput::_mouseflags;

eGXInputDevice              GXInput::_activeInputDevice = eGXInputDevice::Keyboard;

PFNXINPUTGETSTATEPROC       GXInput::_xInputGetState = nullptr;
PFNXINPUTENABLEPROC         GXInput::_xInputEnable = nullptr;

GXInput*                    GXInput::_instance = nullptr;


GXInput& GXCALL GXInput::GetInstance ()
{
    if ( !_instance )
        _instance = new GXInput ();

    return *_instance;
}

GXInput::~GXInput ()
{
    DestroyXInputLibrary ();
    delete _thread;
}

GXVoid GXInput::Start ()
{
    _thread->Start ();
}

GXVoid GXInput::Shutdown ()
{
    _loopFlag = GX_FALSE;
    _thread->Join ();
}

GXVoid GXInput::BindKeyCallback ( GXVoid* handler, PFNGXKEYPROC callback, GXInt virtualKeyCode, eGXInputButtonState state )
{
    const GXUShort i = ( state == eGXInputButtonState::Down ) ? static_cast<GXUShort> ( virtualKeyCode * 2 ) : static_cast<GXUShort> ( ( virtualKeyCode * 2 ) + 1 );

    _keysMask[ i ] = GX_FALSE;
    _KeysMapping[ i ] = callback;
    _keysHandlers[ i ] = handler;
}

GXVoid GXInput::UnbindKeyCallback ( GXInt virtualKeyCode, eGXInputButtonState state )
{
    const GXUShort i = ( state == eGXInputButtonState::Down ) ? static_cast<GXUShort> ( virtualKeyCode * 2 ) : static_cast<GXUShort> ( ( virtualKeyCode * 2 ) + 1 );

    _keysMask[ i ] = GX_FALSE;
    _KeysMapping[ i ] = nullptr;
    _keysHandlers[ i ] = nullptr;
}

GXVoid GXInput::BindTypeCallback ( GXVoid* handler, PFNGXTYPEPROC callback )
{
    _onTypeHandler = handler;
    _onType = callback;
    _symbol = 0;
}

GXVoid GXInput::UnbindTypeCallback ()
{
    _onTypeHandler = nullptr;
    _onType = nullptr;
    _symbol = 0;
}

GXVoid GXInput::BindMouseMoveCallback ( GXVoid* handler, PFNGXMOUSEMOVEPROC callback )
{
    _onMouseMoveHandler = handler;
    _doMouseMoving = callback;
}

GXVoid GXInput::UnbindMouseMoveCallback ()
{
    _onMouseMoveHandler = nullptr;
    _doMouseMoving = nullptr;
}

GXVoid GXInput::BindMouseButtonCallback ( GXVoid* handler, PFNGXMOUSEBUTTONSPROC callback )
{
    _onMouseButtonHandler = handler;
    _doMouseButton = callback;
}

GXVoid GXInput::UnbindMouseButtonCallback ()
{
    _onMouseButtonHandler = nullptr;
    _doMouseButton = nullptr;
}

GXVoid GXInput::BindMouseWheelCallback ( GXVoid* handler, PFNGXMOUSEWHEELPROC callback )
{
    _onMouseWheelHandler = handler;
    _doMouseWheel = callback;
}

GXVoid GXInput::UnbindMouseWheelCallback ()
{
    _onMouseWheelHandler = nullptr;
    _doMouseWheel = nullptr;
}

GXVoid GXInput::BindGamepadKeyCallback ( GXVoid* handler, PFNGXKEYPROC callback, GXInt gamepadKey, eGXInputButtonState state )
{
    const GXUByte i = ( state == eGXInputButtonState::Down ) ? static_cast<GXUByte> ( gamepadKey * 2 ) : static_cast<GXUByte> ( ( gamepadKey * 2 ) + 1 );

    _gamepadKeysHandlers[ i ] = handler;
    _gamepadKeysMapping[ i ] = callback;
    _gamepadKeysMask[ i ] = GX_FALSE;
}

GXVoid GXInput::UnbindGamepadKeyCallback ( GXInt gamepadKey, eGXInputButtonState state )
{
    const GXUByte i = ( state == eGXInputButtonState::Down ) ? static_cast<GXUByte> ( gamepadKey * 2 ) : static_cast<GXUByte> ( ( gamepadKey * 2 ) + 1 );

    _gamepadKeysHandlers[ i ] = nullptr;
    _gamepadKeysMapping[ i ] = nullptr;
    _gamepadKeysMask[ i ] = GX_FALSE;
}

GXVoid GXInput::BindLeftTriggerCallback ( GXVoid* handler, PFNGXTRIGGERPROC callback )
{
    _onLeftTriggerHandler = handler;
    _doLeftTrigger = callback;
}

GXVoid GXInput::UnbindLeftTriggerCallback ()
{
    _onLeftTriggerHandler = nullptr;
    _doLeftTrigger = nullptr;
}

GXVoid GXInput::BindRightTriggerCallback ( GXVoid* handler, PFNGXTRIGGERPROC callback )
{
    _onRightTriggerHandler = handler;
    _doRightTrigger = callback;
}

GXVoid GXInput::UnbindRightTriggerCallback ()
{
    _onRightTriggerHandler = nullptr;
    _doRightTrigger = nullptr;
}

GXVoid GXInput::BindLeftStickCallback ( GXVoid* handler, PFNGXSTICKPROC callback )
{
    _onLeftStickHandler = handler;
    _doLeftStick = callback;
}

GXVoid GXInput::UnbindLeftStickCallback ()
{
    _onLeftStickHandler = nullptr;
    _doLeftStick = nullptr;
}

GXVoid GXInput::BindRightStickCallback ( GXVoid* handler, PFNGXSTICKPROC callback )
{
    _onRightTriggerHandler = handler;
    _doRightStick = callback;
}

GXVoid GXInput::UnbindRightStickCallback ()
{
    _onRightTriggerHandler = nullptr;
    _doRightStick = nullptr;
}

LRESULT CALLBACK GXInput::InputProc ( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch ( msg )
    {
        case WM_CREATE:
        return 0;

        case WM_DESTROY:
        return 0;
    
        case WM_SYSCOMMAND:
            if ( wParam ==  SC_SCREENSAVE || wParam ==  SC_SCREENSAVE )
                return 0;
        break;

        case WM_CLOSE:
        case WM_QUIT:
            GXCore::GetInstance ().Exit ();
            PostQuitMessage ( 0 );
        return 0;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            _activeInputDevice = eGXInputDevice::Keyboard;
            _keysMask[ wParam << 1 ] = GX_TRUE;

            GXTouchSurface::GetInstance ().OnKeyDown ( (GXInt)wParam );

            if ( !_onType ) return 0;

            GXBool tests[ 6 ];
            tests[ 0 ] = wParam == VK_SPACE;
            tests[ 1 ] = ( wParam >= VK_KEY_0 ) && ( wParam <= VK_KEY_9 );
            tests[ 2 ] = ( wParam >= VK_KEY_A ) && ( wParam <= VK_KEY_Z );
            tests[ 3 ] = ( wParam >= VK_NUMPAD0 ) && ( wParam <= VK_DIVIDE );
            tests[ 4 ] = ( wParam >= VK_OEM_1 ) && ( wParam <= VK_OEM_3 );
            tests[ 5 ] = ( wParam >= VK_OEM_4 ) && ( wParam <= VK_OEM_8 );

            GXBool result = tests[ 0 ] || tests[ 1 ] || tests[ 2 ] || tests[ 3 ] || tests[ 4 ] || tests[ 5 ];

            if ( !result ) return 0;

            static BYTE inputKeys[ 256 ];

            GetKeyboardState ( inputKeys );
            GXWChar buff[ 20 ];

            if ( ToUnicode ( static_cast<UINT> ( wParam ), static_cast<UINT> ( lParam ), inputKeys, buff, 20, 0u ) )
                _onType ( _onTypeHandler, buff[ 0 ] );
        }
        return 0;

        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            GXTouchSurface::GetInstance ().OnKeyUp ( static_cast<GXInt> ( wParam ) );
            _keysMask[ ( wParam << 1 ) + 1 ] = GX_TRUE;
        }
        return 0;

        case WM_SIZE:
            // core->NotifyResize ( LOWORD ( lParam ), HIWORD ( lParam ) );
        return 0;
        
        case WM_LBUTTONDOWN:
        {
            if ( _doMouseButton )
            {
                _mouseflags._leftMouseButton = 1;
                _doMouseButton ( _onMouseButtonHandler, _mouseflags );
            }

            GXVec2 pos;
            pos.SetX ( static_cast<GXFloat> ( LOWORD ( lParam ) ) );
            pos.SetY ( static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

            GXTouchSurface::GetInstance ().OnLeftMouseButtonDown ( pos );
        }
        return 0;
        
        case WM_RBUTTONDOWN:
        {
            if ( _doMouseButton )
            {
                _mouseflags._rightMouseButton = 1;
                _doMouseButton ( _onMouseButtonHandler, _mouseflags );
            }

            GXVec2 pos;
            pos.SetX ( static_cast<GXFloat> ( LOWORD ( lParam ) ) );
            pos.SetY ( static_cast<GXFloat> ( ( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) ) );

            GXTouchSurface::GetInstance ().OnRightMouseButtonDown ( pos );
        }
        return 0;

        case WM_MBUTTONDOWN:
        {
            if ( _doMouseButton )
            {
                _mouseflags._middleMouseButton = GX_TRUE;
                _doMouseButton ( _onMouseButtonHandler, _mouseflags );
            }

            GXVec2 pos;
            pos.SetX ( static_cast<GXFloat> ( LOWORD ( lParam ) ) );
            pos.SetY ( static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

            GXTouchSurface::GetInstance ().OnMiddleMouseButtonDown ( pos );
        }
        return 0;
        
        case WM_LBUTTONUP:
        {
            if ( _doMouseButton )
            {
                _mouseflags._leftMouseButton = GX_TRUE;
                _doMouseButton ( _onMouseButtonHandler, _mouseflags );
            }

            GXVec2 pos;
            pos.SetX ( static_cast<GXFloat> ( LOWORD ( lParam ) ) );
            pos.SetY ( static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

            GXTouchSurface::GetInstance ().OnLeftMouseButtonUp ( pos );
        }
        return 0;
        
        case WM_RBUTTONUP:
        {
            if ( _doMouseButton )
            {
                _mouseflags._rightMouseButton = GX_FALSE;
                _doMouseButton ( _onMouseButtonHandler, _mouseflags );
            }

            GXVec2 pos;
            pos.SetX ( static_cast<GXFloat> ( LOWORD ( lParam ) ) );
            pos.SetY ( static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

            GXTouchSurface::GetInstance ().OnRightMouseButtonUp ( pos );
        }
        return 0;

        case WM_MBUTTONUP:
        {
            if ( _doMouseButton )
            {
                _mouseflags._middleMouseButton = 0;
                _doMouseButton ( _onMouseButtonHandler, _mouseflags );
            }

            GXVec2 pos;
            pos.SetX ( static_cast<GXFloat> ( LOWORD ( lParam ) ) );
            pos.SetY ( static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

            GXTouchSurface::GetInstance ().OnMiddleMouseButtonUp ( pos );
        }
        return 0;
        
        case WM_MOUSEMOVE:
        {
            if ( _doMouseMoving )
                _doMouseMoving ( _onMouseMoveHandler, LOWORD ( lParam ), HIWORD ( lParam ) );

            GXVec2 pos;
            pos.SetX ( static_cast<GXFloat> ( LOWORD ( lParam ) ) );
            pos.SetY ( static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

            GXTouchSurface::GetInstance ().OnMouseMove ( pos );
        }
        return 0;

        case WM_MOUSEWHEEL:
        {
            GXInt steps = GET_WHEEL_DELTA_WPARAM ( wParam ) / WHEEL_DELTA;

            if ( _doMouseWheel )
                _doMouseWheel ( _onMouseWheelHandler, steps );

            POINT posRaw;
            posRaw.x = GET_X_LPARAM ( lParam );
            posRaw.y = GET_Y_LPARAM ( lParam );
            ScreenToClient ( hwnd, &posRaw );

            GXVec2 pos;
            pos.SetX ( static_cast<GXFloat> ( posRaw.x ) );
            pos.SetY ( static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () - posRaw.y ) );

            GXTouchSurface::GetInstance ().OnScroll ( pos, static_cast<GXFloat> ( steps ) );
        }
        return 0;

        case WM_SETCURSOR:
            // NOTHING
        return 0;

        case WM_LBUTTONDBLCLK:
        {
            GXVec2 pos;
            pos.SetX ( static_cast<GXFloat> ( GET_X_LPARAM ( lParam ) ) );
            pos.SetY ( static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () - GET_Y_LPARAM ( lParam ) ) );

            GXTouchSurface::GetInstance ().OnDoubleClick ( pos );
        }
        return 0;
    }

    return DefWindowProc ( hwnd, msg, wParam, lParam );
}

GXInput::GXInput ()
{
    _activeInputDevice = eGXInputDevice::Keyboard;

    for ( GXInt i = 0; i < GX_INPUT_TOTAL_KEYBOARD_KEYS; ++i )
    {
        _keysMask[ i ] = GX_FALSE;
        _KeysMapping[ i ] = nullptr;
    }

    _onTypeHandler = nullptr;
    _onType = nullptr;
    _symbol = 0;
    
    _mouseflags._leftMouseButton = false;
    _mouseflags._middleMouseButton = false;
    _mouseflags._rightMouseButton = false;

    _onMouseMoveHandler = nullptr;
    _doMouseMoving = nullptr;

    _onMouseButtonHandler = nullptr;
    _doMouseButton = nullptr;

    _onMouseWheelHandler = nullptr;
    _doMouseWheel = nullptr;

    GXInt limit = GX_INPUT_TOTAL_GAMEPAD_KEYS * 2;

    for ( GXInt i = 0; i < limit; ++i )
    {
        _gamepadKeysHandlers[ i ] = nullptr;
        _gamepadKeysMapping[ i ] = nullptr;
        _gamepadKeysMask[ i ] = GX_FALSE;
    }

    _onLeftStickHandler = _onRightStickTrigger = nullptr;
    _doLeftStick = _doRightStick = nullptr;

    _onLeftTriggerHandler = _onRightTriggerHandler = nullptr;
    _doLeftTrigger = _doRightTrigger = nullptr;

    _currentGamepadState = 0;
    _gamepadState[ 0 ].Gamepad.wButtons = _gamepadState[ 1 ].Gamepad.wButtons = 0;

    InitXInputLibrary ();

    _loopFlag = GX_TRUE;
    _thread = new GXThread ( &InputLoop, nullptr );
}

GXUPointer GXTHREADCALL GXInput::InputLoop ( GXVoid* /*args*/, GXThread &inputThread )
{
    while ( _loopFlag )
    {
        UpdateGamepad ();

        switch ( _activeInputDevice )
        {
            case eGXInputDevice::Keyboard:
            {
                for ( GXUShort i = 0u; i < GX_INPUT_TOTAL_KEYBOARD_KEYS; ++i )
                {
                    if ( _keysMask[ i ] && _KeysMapping[ i ] )
                    {
                        _KeysMapping[ i ] ( _keysHandlers[ i ] );
                        _keysMask[ i ] = GX_FALSE;
                    }
                }

                if ( _onType && _symbol )
                    _onType ( _onTypeHandler, _symbol );
            }
            break;

            case eGXInputDevice::Mouse:
                // NOTHING
            break;

            case eGXInputDevice::XBOXController:
            {
                for ( GXInt i = 0;  i < GX_INPUT_TOTAL_GAMEPAD_KEYS * 2; ++i )
                {
                    if ( _gamepadKeysMapping[ i ] && _gamepadKeysMask[ i ] )
                    {
                        _gamepadKeysMapping[ i ] ( _gamepadKeysHandlers[ i ] );
                        _gamepadKeysMask[ i ] = GX_FALSE;
                    }
                }
            }
            break;
        }

        inputThread.Sleep ( INPUT_THREAD_SLEEP );
    }

    return 0u;
}

GXBool GXInput::IsGamepadConnected ( GXDword gamepadID )
{
    XINPUT_STATE state;
    return _xInputGetState ( gamepadID, &state ) != ERROR_DEVICE_NOT_CONNECTED;
}

GXVoid GXInput::TestGamepadButton ( GXDword buttonFlag, GXUChar buttonID )
{
    GXUByte oldGamepadState = ( _currentGamepadState == 0 ) ? (GXUByte)1 : (GXUByte)0;

    if ( ( _gamepadState[ _currentGamepadState ].Gamepad.wButtons & buttonFlag ) && !( _gamepadState[ oldGamepadState ].Gamepad.wButtons & buttonFlag ) )
    {
        _activeInputDevice = eGXInputDevice::XBOXController;
        _gamepadKeysMask[ buttonID << 1 ] = GX_TRUE;
    }
    else if ( !( _gamepadState[ _currentGamepadState ].Gamepad.wButtons & buttonFlag ) && ( _gamepadState[ oldGamepadState ].Gamepad.wButtons & buttonFlag ) )
    {
        _gamepadKeysMask[ ( buttonID << 1 ) + 1 ] = GX_TRUE;
    }
}

GXVoid GXInput::UpdateGamepad ()
{
    if ( _xInputGetState ( 0u, &_gamepadState[ _currentGamepadState ] ) == ERROR_DEVICE_NOT_CONNECTED ) return;
    
    if ( _activeInputDevice == eGXInputDevice::XBOXController )
    {
        if ( _doLeftStick )
            _doLeftStick ( _onLeftStickHandler, _gamepadState[ _currentGamepadState ].Gamepad.sThumbLX * INPUT_INV_STICK_VALUE, _gamepadState[ _currentGamepadState ].Gamepad.sThumbLY * INPUT_INV_STICK_VALUE );

        if ( _doRightStick )
            _doRightStick ( _onRightStickTrigger, _gamepadState[ _currentGamepadState ].Gamepad.sThumbRX * INPUT_INV_STICK_VALUE, _gamepadState[ _currentGamepadState ].Gamepad.sThumbRY * INPUT_INV_STICK_VALUE );

        if ( _doLeftTrigger )
            _doLeftTrigger ( _onLeftTriggerHandler, _gamepadState[ _currentGamepadState ].Gamepad.bLeftTrigger * INPUT_INV_TRIGGER_VALUE );

        if ( _doRightTrigger )
            _doRightTrigger ( _onRightTriggerHandler, _gamepadState[ _currentGamepadState ].Gamepad.bRightTrigger * INPUT_INV_TRIGGER_VALUE );
    }

    GXUByte oldGamepadState = ( _currentGamepadState == 0u ) ? 1u : 0u;

    if ( _gamepadState[ _currentGamepadState ].dwPacketNumber == _gamepadState[ oldGamepadState ].dwPacketNumber ) return;

    TestGamepadButton ( XINPUT_GAMEPAD_A, GX_INPUT_XBOX_A );
    TestGamepadButton ( XINPUT_GAMEPAD_B, GX_INPUT_XBOX_B );
    TestGamepadButton ( XINPUT_GAMEPAD_X, GX_INPUT_XBOX_X );
    TestGamepadButton ( XINPUT_GAMEPAD_Y, GX_INPUT_XBOX_Y );
    TestGamepadButton ( XINPUT_GAMEPAD_DPAD_UP, GX_INPUT_XBOX_CROSS_UP );
    TestGamepadButton ( XINPUT_GAMEPAD_DPAD_DOWN, GX_INPUT_XBOX_CROSS_DOWN );
    TestGamepadButton ( XINPUT_GAMEPAD_DPAD_LEFT, GX_INPUT_XBOX_CROSS_LEFT );
    TestGamepadButton ( XINPUT_GAMEPAD_DPAD_RIGHT, GX_INPUT_XBOX_CROSS_RIGHT );
    TestGamepadButton ( XINPUT_GAMEPAD_LEFT_SHOULDER, GX_INPUT_XBOX_SHOULDER_LEFT );
    TestGamepadButton ( XINPUT_GAMEPAD_RIGHT_SHOULDER, GX_INPUT_XBOX_SHOULDER_RIGHT );
    TestGamepadButton ( XINPUT_GAMEPAD_LEFT_THUMB, GX_INPUT_XBOX_THUMB_LEFT );
    TestGamepadButton ( XINPUT_GAMEPAD_RIGHT_THUMB, GX_INPUT_XBOX_THUMB_RIGHT );
    TestGamepadButton ( XINPUT_GAMEPAD_START, GX_INPUT_XBOX_START );
    TestGamepadButton ( XINPUT_GAMEPAD_BACK, GX_INPUT_XBOX_BACK );

    _currentGamepadState = oldGamepadState;
}

GXBool GXCALL GXInput::InitXInputLibrary ()
{
    gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );

    if ( !gx_GXEngineDLLModuleHandle )
    {
        GXLogA ( "%i\n", GetLastError () );
        GXLogA ( "GXInput::InitXInputLibrary::Error - Не удалось загрузить GXEngine.dll\n" );
        return GX_FALSE;
    }

    PFNGXXINPUTINITPROC GXXInputInit = reinterpret_cast<PFNGXXINPUTINITPROC> ( reinterpret_cast<GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXXInputInit" ) ) );

    if ( !GXXInputInit )
    {
        GXLogA ( "GXInput::InitXInputLibrary::Error - Не удалось найти функцию GXXInputInit\n" );
        return GX_FALSE;
    }

    GXXInputFunctions out;
    out.XInputGetState = &_xInputGetState;
    out.XInputEnable = &_xInputEnable;
    GXXInputInit ( out );

    return GX_TRUE;
}

GXBool GXCALL GXInput::DestroyXInputLibrary ()
{
    if ( !gx_GXEngineDLLModuleHandle )
    {
        GXLogA ( "GXInput::DestroyXInputLibrary::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll\n" );
        return GX_FALSE;
    }

    if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
    {
        GXLogA ( "GXInput::DestroyXInputLibrary::Error - Не удалось выгрузить библиотеку GXEngine.dll\n" );
        return GX_FALSE;
    }

    return GX_TRUE;
}
