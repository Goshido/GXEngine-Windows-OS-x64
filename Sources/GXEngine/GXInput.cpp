// version 1.15

#include <GXEngine/GXInput.h>
#include <GXEngine/GXCore.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXEngineDLL/GXEngineAPI.h>
#include <GXCommon/GXLogger.h>
#include <Windowsx.h>


#define	GX_INPUT_THREAD_SLEEP		40

#define GX_INPUT_INV_STICK_VALUE	3.0519e-5f
#define GX_INPUT_INV_TRIGGER_VALUE	3.9216e-3f


extern HMODULE gx_GXEngineDLLModuleHandle;


GXThread*				GXInput::thread = nullptr;
GXBool					GXInput::loopFlag = false;

GXVoid*					GXInput::keysHandlers[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
PFNGXKEYPROC			GXInput::KeysMapping[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
GXBool					GXInput::keysMask[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];

GXVoid*					GXInput::onTypeHandler = nullptr;
PFNGXTYPEPROC			GXInput::OnType = nullptr;
GXWChar					GXInput::symbol;

XINPUT_STATE			GXInput::gamepadState[ 2 ];
GXUByte					GXInput::currentGamepadState;

GXVoid*					GXInput::gamepadKeysHandlers[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];
PFNGXKEYPROC			GXInput::GamepadKeysMapping[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];
GXByte					GXInput::gamepadKeysMask[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];

GXVoid*					GXInput::onLeftTriggerHandler = nullptr;
PFNGXTRIGGERPROC		GXInput::DoLeftTrigger = nullptr;

GXVoid*					GXInput::onRightTriggerHandler = nullptr;
PFNGXTRIGGERPROC		GXInput::DoRightTrigger = nullptr;

GXVoid*					GXInput::onLeftStickHandler = nullptr;
PFNGXSTICKPROC			GXInput::DoLeftStick = nullptr;

GXVoid*					GXInput::onRightStickTrigger = nullptr;
PFNGXSTICKPROC			GXInput::DoRightStick = nullptr;

GXVoid*					GXInput::onMouseMoveHandler = nullptr;
PFNGXMOUSEMOVEPROC		GXInput::DoMouseMoving = nullptr;

GXVoid*					GXInput::onMouseButtonHandler = nullptr;
PFNGXMOUSEBUTTONSPROC	GXInput::DoMouseButton = nullptr;

GXVoid*					GXInput::onMouseWheelHandler = nullptr;
PFNGXMOUSEWHEELPROC		GXInput::DoMouseWheel = nullptr;

GXInputMouseFlags		GXInput::mouseflags;

eGXInputDevice			GXInput::activeInputDevice = eGXInputDevice::Keyboard;

PFNXINPUTGETSTATEPROC	GXInput::XInputGetState = nullptr;
PFNXINPUTENABLEPROC		GXInput::XInputEnable = nullptr;

GXInput*				GXInput::instance = nullptr;


GXInput::~GXInput ()
{
	DestroyXInputLibrary ();
	delete thread;
}

GXVoid GXInput::Start ()
{
	thread->Start ();
}

GXVoid GXInput::Shutdown ()
{
	loopFlag = GX_FALSE;
	thread->Join ();
}

GXVoid GXInput::BindKeyCallback ( GXVoid* handler, PFNGXKEYPROC callback, GXInt vk_key, eGXInputButtonState eState )
{
	GXUShort i = ( eState == eGXInputButtonState::Down ) ? (GXUShort)( vk_key * 2 ) : (GXUShort)( ( vk_key * 2 ) + 1 );

	keysMask[ i ] = GX_FALSE;
	KeysMapping[ i ] = callback;
	keysHandlers[ i ] = handler;
}

GXVoid GXInput::UnbindKeyCallback ( GXInt vk_key, eGXInputButtonState eState  )
{
	GXUShort i = ( eState == eGXInputButtonState::Down ) ? (GXUShort)( vk_key * 2 ) : (GXUShort)( ( vk_key * 2 ) + 1 );

	keysMask[ i ] = GX_FALSE;
	KeysMapping[ i ] = nullptr;
	keysHandlers[ i ] = nullptr;
}

GXVoid GXInput::BindTypeCallback ( GXVoid* handler, PFNGXTYPEPROC callback )
{
	onTypeHandler = handler;
	OnType = callback;
	symbol = 0;
}

GXVoid GXInput::UnbindTypeCallback ()
{
	onTypeHandler = nullptr;
	OnType = nullptr;
	symbol = 0;
}

GXVoid GXInput::BindMouseMoveCallback ( GXVoid* handler, PFNGXMOUSEMOVEPROC callback )
{
	onMouseMoveHandler = handler;
	DoMouseMoving = callback;
}

GXVoid GXInput::UnbindMouseMoveCallback ()
{
	onMouseMoveHandler = nullptr;
	DoMouseMoving = nullptr;
}

GXVoid GXInput::BindMouseButtonCallback ( GXVoid* handler, PFNGXMOUSEBUTTONSPROC callback )
{
	onMouseButtonHandler = handler;
	DoMouseButton = callback;
}

GXVoid GXInput::UnbindMouseButtonCallback ()
{
	onMouseButtonHandler = nullptr;
	DoMouseButton = nullptr;
}

GXVoid GXInput::BindMouseWheelCallback ( GXVoid* handler, PFNGXMOUSEWHEELPROC callback )
{
	onMouseWheelHandler = handler;
	DoMouseWheel = callback;
}

GXVoid GXInput::UnbindMouseWheelCallback ()
{
	onMouseWheelHandler = nullptr;
	DoMouseWheel = nullptr;
}

GXVoid GXInput::BindGamepadKeyCallback ( GXVoid* handler, PFNGXKEYPROC callback, GXInt gamepad_key, eGXInputButtonState eState )
{
	GXUByte i = ( eState == eGXInputButtonState::Down ) ? (GXUByte)( gamepad_key * 2 ) : (GXUByte)( ( gamepad_key * 2 ) + 1 );

	gamepadKeysHandlers[ i ] = handler;
	GamepadKeysMapping[ i ] = callback;
	gamepadKeysMask[ i ] = GX_FALSE;
}

GXVoid GXInput::UnbindGamepadKeyCallback ( GXInt gamepad_key, eGXInputButtonState eState )
{
	GXUByte i = ( eState == eGXInputButtonState::Down ) ? (GXUByte)( gamepad_key * 2 ) : (GXUByte)( ( gamepad_key * 2 ) + 1 );

	gamepadKeysHandlers[ i ] = nullptr;
	GamepadKeysMapping[ i ] = nullptr;
	gamepadKeysMask[ i ] = GX_FALSE;
}

GXVoid GXInput::BindLeftTriggerCallback ( GXVoid* handler, PFNGXTRIGGERPROC callback )
{
	onLeftTriggerHandler = handler;
	DoLeftTrigger = callback;
}

GXVoid GXInput::UnbindLeftTriggerCallback ()
{
	onLeftTriggerHandler = nullptr;
	DoLeftTrigger = nullptr;
}

GXVoid GXInput::BindRightTriggerCallback ( GXVoid* handler, PFNGXTRIGGERPROC callback )
{
	onRightTriggerHandler = handler;
	DoRightTrigger = callback;
}

GXVoid GXInput::UnbindRightTriggerCallback ()
{
	onRightTriggerHandler = nullptr;
	DoRightTrigger = nullptr;
}

GXVoid GXInput::BindLeftStickCallback ( GXVoid* handler, PFNGXSTICKPROC callback )
{
	onLeftStickHandler = handler;
	DoLeftStick = callback;
}

GXVoid GXInput::UnbindLeftStickCallback ()
{
	onLeftStickHandler = nullptr;
	DoLeftStick = nullptr;
}

GXVoid GXInput::BindRightStickCallback ( GXVoid* handler, PFNGXSTICKPROC callback )
{
	onRightTriggerHandler = handler;
	DoRightStick = callback;
}

GXVoid GXInput::UnbindRightStickCallback ()
{
	onRightTriggerHandler = nullptr;
	DoRightStick = nullptr;
}

GXInput& GXCALL GXInput::GetInstance ()
{
	if ( !instance )
		instance = new GXInput ();

	return *instance;
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
			activeInputDevice = eGXInputDevice::Keyboard;
			keysMask[ wParam << 1 ] = GX_TRUE;

			GXTouchSurface::GetInstance ().OnKeyDown ( (GXInt)wParam );

			if ( !OnType ) return 0;

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
			if ( ToUnicode ( (UINT)wParam, (UINT)lParam, inputKeys, buff, 20, 0 ) )
				OnType ( onTypeHandler, buff[ 0 ] );
		}
		return 0;

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			GXTouchSurface::GetInstance ().OnKeyUp ( (GXInt)wParam );
			keysMask[ ( wParam << 1 ) + 1 ] = GX_TRUE;
		}
		return 0;

		case WM_SIZE:
			// core->NotifyResize ( LOWORD ( lParam ), HIWORD ( lParam ) );
		return 0;
		
		case WM_LBUTTONDOWN:
		{
			if ( DoMouseButton )
			{
				mouseflags.lmb = 1;
				DoMouseButton ( onMouseButtonHandler, mouseflags );
			}

			GXVec2 pos;
			pos.SetX ( (GXFloat)LOWORD ( lParam ) );
			pos.SetY ( (GXFloat)( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

			GXTouchSurface::GetInstance ().OnLeftMouseButtonDown ( pos );
		}
		return 0;
		
		case WM_RBUTTONDOWN:
		{
			if ( DoMouseButton )
			{
				mouseflags.rmb = 1;
				DoMouseButton ( onMouseButtonHandler, mouseflags );
			}

			GXVec2 pos;
			pos.SetX ( (GXFloat)LOWORD ( lParam ) );
			pos.SetY ( (GXFloat)( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

			GXTouchSurface::GetInstance ().OnRightMouseButtonDown ( pos );
		}
		return 0;

		case WM_MBUTTONDOWN:
		{
			if ( DoMouseButton )
			{
				mouseflags.mmb = 1;
				DoMouseButton ( onMouseButtonHandler, mouseflags );
			}

			GXVec2 pos;
			pos.SetX ( (GXFloat)LOWORD ( lParam ) );
			pos.SetY ( (GXFloat)( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

			GXTouchSurface::GetInstance ().OnMiddleMouseButtonDown ( pos );
		}
		return 0;
		
		case WM_LBUTTONUP:
		{
			if ( DoMouseButton )
			{
				mouseflags.lmb = 0;
				DoMouseButton ( onMouseButtonHandler, mouseflags );
			}

			GXVec2 pos;
			pos.SetX ( (GXFloat)LOWORD ( lParam ) );
			pos.SetY ( (GXFloat)( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

			GXTouchSurface::GetInstance ().OnLeftMouseButtonUp ( pos );
		}
		return 0;
		
		case WM_RBUTTONUP:
		{
			if ( DoMouseButton )
			{
				mouseflags.rmb = 0;
				DoMouseButton ( onMouseButtonHandler, mouseflags );
			}

			GXVec2 pos;
			pos.SetX ( (GXFloat)LOWORD ( lParam ) );
			pos.SetY ( (GXFloat)( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

			GXTouchSurface::GetInstance ().OnRightMouseButtonUp ( pos );
		}
		return 0;

		case WM_MBUTTONUP:
		{
			if ( DoMouseButton )
			{
				mouseflags.mmb = 0;
				DoMouseButton ( onMouseButtonHandler, mouseflags );
			}

			GXVec2 pos;
			pos.SetX ( (GXFloat)LOWORD ( lParam ) );
			pos.SetY ( (GXFloat)( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

			GXTouchSurface::GetInstance ().OnMiddleMouseButtonUp ( pos );
		}
		return 0;
		
		case WM_MOUSEMOVE:
		{
			if ( DoMouseMoving )
				DoMouseMoving ( onMouseMoveHandler, LOWORD ( lParam ), HIWORD ( lParam ) );

			GXVec2 pos;
			pos.SetX ( (GXFloat)LOWORD ( lParam ) );
			pos.SetY ( (GXFloat)( GXRenderer::GetInstance ().GetHeight () - HIWORD ( lParam ) ) );

			GXTouchSurface::GetInstance ().OnMouseMove ( pos );
		}
		return 0;

		case WM_MOUSEWHEEL:
		{
			GXInt steps = GET_WHEEL_DELTA_WPARAM ( wParam ) / WHEEL_DELTA;
			if ( DoMouseWheel )
				DoMouseWheel ( onMouseWheelHandler, steps );

			POINT posRaw;
			posRaw.x = GET_X_LPARAM ( lParam );
			posRaw.y = GET_Y_LPARAM ( lParam );
			ScreenToClient ( hwnd, &posRaw );

			GXVec2 pos;
			pos.SetX ( (GXFloat)posRaw.x );
			pos.SetY ( (GXFloat)( GXRenderer::GetInstance ().GetHeight () - posRaw.y ) );

			GXTouchSurface::GetInstance ().OnScroll ( pos, (GXFloat)steps );
		}
		return 0;

		case WM_SETCURSOR:
			// NOTHING
		return 0;

		case WM_LBUTTONDBLCLK:
		{
			GXVec2 pos;
			pos.SetX ( (GXFloat)GET_X_LPARAM ( lParam ) );
			pos.SetY ( (GXFloat)( GXRenderer::GetInstance ().GetHeight () - GET_Y_LPARAM ( lParam ) ) );

			GXTouchSurface::GetInstance ().OnDoubleClick ( pos );
		}
		return 0;
	}

	return DefWindowProc ( hwnd, msg, wParam, lParam );
}

GXInput::GXInput ()
{
	activeInputDevice = eGXInputDevice::Keyboard;

	for ( GXInt i = 0; i < GX_INPUT_TOTAL_KEYBOARD_KEYS; i++ )
	{
		keysMask[ i ] = GX_FALSE;
		KeysMapping[ i ] = nullptr;
	}

	onTypeHandler = nullptr;
	OnType = nullptr;
	symbol = 0;
	
	mouseflags.lmb = false;
	mouseflags.mmb = false;
	mouseflags.rmb = false;

	onMouseMoveHandler = nullptr;
	DoMouseMoving = nullptr;

	onMouseButtonHandler = nullptr;
	DoMouseButton = nullptr;

	onMouseWheelHandler = nullptr;
	DoMouseWheel = nullptr;

	GXInt limit = GX_INPUT_TOTAL_GAMEPAD_KEYS * 2;
	for ( GXInt i = 0; i < limit; i++ )
	{
		gamepadKeysHandlers[ i ] = nullptr;
		GamepadKeysMapping[ i ] = nullptr;
		gamepadKeysMask[ i ] = GX_FALSE;
	}

	onLeftStickHandler = onRightStickTrigger = nullptr;
	DoLeftStick = DoRightStick = nullptr;

	onLeftTriggerHandler = onRightTriggerHandler = nullptr;
	DoLeftTrigger = DoRightTrigger = nullptr;

	currentGamepadState = 0;
	gamepadState[ 0 ].Gamepad.wButtons = gamepadState[ 1 ].Gamepad.wButtons = 0;

	InitXInputLibrary ();

	loopFlag = GX_TRUE;
	thread = new GXThread ( &InputLoop, nullptr );
}

GXUPointer GXTHREADCALL GXInput::InputLoop ( GXVoid* /*args*/, GXThread& /*thread*/ )
{
	while ( loopFlag )
	{
		UpdateGamepad ();

		switch ( activeInputDevice )
		{
			case eGXInputDevice::Keyboard:
			{
				for ( GXUShort i = 0; i < GX_INPUT_TOTAL_KEYBOARD_KEYS; i++ )
				{
					if ( keysMask[ i ] && KeysMapping[ i ] )
					{
						KeysMapping[ i ] ( keysHandlers[ i ] );
						keysMask[ i ] = GX_FALSE;
					}
				}

				if ( OnType && symbol )
					OnType ( onTypeHandler, symbol );
			}
			break;

			case eGXInputDevice::Mouse:
				// NOTHING
			break;

			case eGXInputDevice::xboxController:
			{
				for ( GXInt i = 0;  i < GX_INPUT_TOTAL_GAMEPAD_KEYS * 2; i++ )
				{
					if ( GamepadKeysMapping[ i ] && gamepadKeysMask[ i ] )
					{
						GamepadKeysMapping[ i ] ( gamepadKeysHandlers[ i ] );
						gamepadKeysMask[ i ] = GX_FALSE;
					}
				}
			}
			break;
		}

		Sleep ( GX_INPUT_THREAD_SLEEP );
	}

	return 0;
}

GXBool GXInput::IsGamepadConnected ( GXDword gamepadID )
{
	XINPUT_STATE state;

	if ( XInputGetState ( gamepadID, &state ) == ERROR_DEVICE_NOT_CONNECTED ) 
		return GX_FALSE;
	else
		return GX_TRUE;
}

GXVoid GXInput::TestGamepadButton ( GXDword buttonFlag, GXUChar buttonID )
{
	GXUByte oldGamepadState = ( currentGamepadState == 0 ) ? (GXUByte)1 : (GXUByte)0;

	if ( ( gamepadState[ currentGamepadState ].Gamepad.wButtons & buttonFlag ) && !( gamepadState[ oldGamepadState ].Gamepad.wButtons & buttonFlag ) )
	{
		activeInputDevice = eGXInputDevice::xboxController;
		gamepadKeysMask[ buttonID << 1 ] = GX_TRUE;
	}
	else if ( !( gamepadState[ currentGamepadState ].Gamepad.wButtons & buttonFlag ) && ( gamepadState[ oldGamepadState ].Gamepad.wButtons & buttonFlag ) )
	{
		gamepadKeysMask[ ( buttonID << 1 ) + 1 ] = GX_TRUE;
	}
}

GXVoid GXInput::UpdateGamepad ()
{
	if ( XInputGetState ( 0, &gamepadState[ currentGamepadState ] ) == ERROR_DEVICE_NOT_CONNECTED ) return;
	
	if ( activeInputDevice == eGXInputDevice::xboxController )
	{
		if ( DoLeftStick )
			DoLeftStick ( onLeftStickHandler, gamepadState[ currentGamepadState ].Gamepad.sThumbLX * GX_INPUT_INV_STICK_VALUE, gamepadState[ currentGamepadState ].Gamepad.sThumbLY * GX_INPUT_INV_STICK_VALUE );

		if ( DoRightStick )
			DoRightStick ( onRightStickTrigger, gamepadState[ currentGamepadState ].Gamepad.sThumbRX * GX_INPUT_INV_STICK_VALUE, gamepadState[ currentGamepadState ].Gamepad.sThumbRY * GX_INPUT_INV_STICK_VALUE );

		if ( DoLeftTrigger )
			DoLeftTrigger ( onLeftTriggerHandler, gamepadState[ currentGamepadState ].Gamepad.bLeftTrigger * GX_INPUT_INV_TRIGGER_VALUE );

		if ( DoRightTrigger )
			DoRightTrigger ( onRightTriggerHandler, gamepadState[ currentGamepadState ].Gamepad.bRightTrigger * GX_INPUT_INV_TRIGGER_VALUE );
	}

	GXUByte oldGamepadState = ( currentGamepadState == 0 ) ? (GXUByte)1 : (GXUByte)0;
	if ( gamepadState[ currentGamepadState ].dwPacketNumber == gamepadState[ oldGamepadState ].dwPacketNumber ) return;

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

	currentGamepadState = oldGamepadState;
}

GXBool GXCALL GXInput::InitXInputLibrary ()
{
	gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"%i\n", GetLastError () );
		GXLogW ( L"GXInput::InitXInputLibrary::Error - Не удалось загрузить GXEngine.dll\n" );
		return GX_FALSE;
	}

	PFNGXXINPUTINITPROC GXXInputInit = reinterpret_cast <PFNGXXINPUTINITPROC> ( reinterpret_cast <GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXXInputInit" ) ) );
	if ( !GXXInputInit )
	{
		GXLogW ( L"GXInput::InitXInputLibrary::Error - Не удалось найти функцию GXXInputInit\n" );
		return GX_FALSE;
	}

	GXXInputFunctions out;
	out.XInputGetState = &XInputGetState;
	out.XInputEnable = &XInputEnable;
	GXXInputInit ( out );

	return GX_TRUE;
}

GXBool GXCALL GXInput::DestroyXInputLibrary ()
{
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXInput::DestroyXInputLibrary::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll\n" );
		return GX_FALSE;
	}

	if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
	{
		GXLogW ( L"GXInput::DestroyXInputLibrary::Error - Не удалось выгрузить библиотеку GXEngine.dll\n" );
		return GX_FALSE;
	}

	return GX_TRUE;
}
