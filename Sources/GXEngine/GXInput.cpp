//version 1.11

#include <GXEngine/GXInput.h>
#include <GXEngine/GXGlobals.h>
#include <Windowsx.h>


#define	GX_INPUT_THREAD_SLEEP		40

#define GX_INPUT_INV_STICK_VALUE	3.0519e-5f
#define GX_INPUT_INV_TRIGGER_VALUE	3.9216e-3f

extern HMODULE gx_GXEngineDLLModuleHandle;

PFNXINPUTGETSTATEPROC	GXXInputGetState = 0;
PFNXINPUTENABLEPROC		GXXInputEnable = 0;


GXBool GXCALL GXXInputInit ()
{
	gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"%i\n", GetLastError() );
		GXLogW ( L"GXInput::XInputInit::Error - Не удалось загрузить GXEngine.dll\n" );
		return GX_FALSE;
	}

	PFNGXXINPUTINITPROC GXXInputInit = (PFNGXXINPUTINITPROC)GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXXInputInit" );
	if ( !GXXInputInit )
	{
		GXLogW ( L"GXInput::XInputInit::Error - Не удалось найти функцию GXXInputInit\n" );
		return GX_FALSE;
	}

	GXXInputFunctions out;
	out.XInputGetState = &GXXInputGetState;
	out.XInputEnable = &GXXInputEnable;
	GXXInputInit ( out );

	return GX_TRUE;
}	

GXBool GXCALL GXXInputDestroy ()
{
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXInput::XInputDestroy::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll\n" );
		return GX_FALSE;
	}

	if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
	{
		GXLogW ( L"GXInput::XInputDestroy::Error - Не удалось выгрузить библиотеку GXEngine.dll\n" );
		return GX_FALSE;
	}

	return GX_TRUE;
}

//--------------------------------------------------------------------------------------------------

GXCore*					GXInput::core = 0;
GXThread*				GXInput::thread = 0;
GXBool					GXInput::keysMask[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
PFNGXKEYPROC			GXInput::KeysMapping[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
GXVoid*					GXInput::keysHandlers[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
PFNGXTYPEPROC			GXInput::OnType;
GXWChar					GXInput::symbol;
GXVoid*					GXInput::onTypeHandler;
XINPUT_STATE			GXInput::gamepadState[ 2 ];
GXUChar					GXInput::currentGamepadState;
PFNGXKEYPROC			GXInput::GamepadKeysMapping[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];
GXByte					GXInput::gamepadKeysMask[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];
GXVoid*					GXInput::gamepadKeysHandlers[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];
PFNGXTRIGGERPROC		GXInput::DoLeftTrigger;
PFNGXTRIGGERPROC		GXInput::DoRightTrigger;
PFNGXSTICKPROC			GXInput::DoLeftStick;
PFNGXSTICKPROC			GXInput::DoRightStick;
GXBool					GXInput::loopFlag;
PFNGXMOUSEMOVEPROC		GXInput::DoMouseMoving;
PFNGXMOUSEBUTTONSPROC	GXInput::DoMouseButtons;
PFNGXMOUSEWHEELPROC		GXInput::DoMouseWheel;
EGXInputMouseFlags		GXInput::mouseflags;


enum eGXInputDevice
{
	KEYBOARD,
	MOUSE,
	XBOX_CONTROLLER
};

eGXInputDevice			gx_inputActiveDevice = KEYBOARD;


GXInput::GXInput ( GXCore* core )
{
	this->core = core;
	for ( GXInt i = 0; i < GX_INPUT_TOTAL_KEYBOARD_KEYS; i++ )
	{
		keysMask[ i ] = GX_FALSE;
		KeysMapping[ i ] = 0;
	}

	OnType = 0;
	symbol = 0;
	onTypeHandler = 0;
	
	mouseflags.allflags = 0;
	DoMouseMoving = 0;
	DoMouseButtons = 0;
	DoMouseWheel = 0;

	for ( GXInt i = 0; i < GX_INPUT_TOTAL_GAMEPAD_KEYS; i++ )
	{
		GamepadKeysMapping[ i << 1 ] = GamepadKeysMapping [ ( i << 1 ) + 1 ] = 0;
		gamepadKeysMask[ i << 1 ] = gamepadKeysMask[ ( i << 1 ) + 1 ] = GX_FALSE;
	}

	DoLeftStick = DoRightStick = 0;
	DoLeftTrigger = DoRightTrigger = 0;
	currentGamepadState = 0;
	gamepadState[ 0 ].Gamepad.wButtons = gamepadState[ 1 ].Gamepad.wButtons = 0;
	GXXInputInit ();

	loopFlag = GX_TRUE;
	thread = new GXThread ( &InputLoop, 0, GX_SUSPEND );
}

GXInput::~GXInput ()
{
	delete thread;
}

GXVoid GXInput::Start ()
{
	thread->Resume ();
	GXXInputDestroy ();
}

GXVoid GXInput::Suspend ()
{
	thread->Suspend ();
}

GXVoid GXInput::Shutdown ()
{
	loopFlag = GX_FALSE;
	thread->Resume ();
	thread->Join ();
}

GXVoid GXInput::BindKeyFunc ( PFNGXKEYPROC callback, GXVoid* handler, GXInt vk_key, EGXInputButtonState eState )
{
	GXUShort i = ( eState == INPUT_DOWN ) ? vk_key << 1 : ( vk_key << 1 ) + 1;

	keysMask[ i ] = GX_FALSE;
	KeysMapping[ i ] = callback;
	keysHandlers[ i ] = handler;
}

GXVoid GXInput::UnBindKeyFunc ( GXInt vk_key, EGXInputButtonState eState  )
{
	GXUShort i = ( eState == INPUT_DOWN ) ? vk_key << 1 : ( vk_key << 1 ) + 1;

	keysMask[ i ] = GX_FALSE;
	KeysMapping[ ( eState == INPUT_DOWN ) ? vk_key * 2 : vk_key * 2 + 1 ] = 0;
}

GXVoid GXInput::BindTypeFunc ( PFNGXTYPEPROC callback, GXVoid* handler )
{
	symbol = 0;
	onTypeHandler = handler;
	OnType = callback;
}

GXVoid GXInput::UnBindTypeFunc ()
{
	symbol = 0;
	OnType = 0;
}

GXVoid GXInput::BindMouseMoveFunc ( PFNGXMOUSEMOVEPROC callback )
{
	DoMouseMoving = callback;
}

GXVoid GXInput::UnBindMouseMoveFunc ()
{
	DoMouseMoving = 0;
}

GXVoid GXInput::BindMouseButtonsFunc ( PFNGXMOUSEBUTTONSPROC callback )
{
	DoMouseButtons = callback;
}

GXVoid GXInput::UnBindMouseButtonsFunc ()
{
	DoMouseButtons = 0;
}

GXVoid GXInput::BindMouseWheelFunc ( PFNGXMOUSEWHEELPROC callback )
{
	DoMouseWheel = callback;
}

GXVoid GXInput::UnBindMouseWheelFunc ()
{
	DoMouseWheel = 0;
}

GXVoid GXInput::BindGamepadKeyFunc ( PFNGXKEYPROC callback, GXVoid* handler, GXInt gamepad_key, EGXInputButtonState eState )
{
	GXUChar i = ( eState == INPUT_DOWN ) ? gamepad_key << 1 : ( gamepad_key << 1 ) + 1;

	gamepadKeysMask[ i ] = GX_FALSE;
	GamepadKeysMapping[ i ] = callback;
	gamepadKeysHandlers[ i ] = handler;
}

GXVoid GXInput::UnBindGamepadKeyFunc ( GXInt gamepad_key, EGXInputButtonState eState )
{
	GXUChar i = ( eState == INPUT_DOWN ) ? gamepad_key << 1 : ( gamepad_key << 1 ) + 1;

	gamepadKeysMask[ i ] = GX_FALSE;
	GamepadKeysMapping[ i ] = 0;
}

GXVoid GXInput::BindLeftTriggerFunc ( PFNGXTRIGGERPROC callback )
{
	DoLeftTrigger = callback;
}

GXVoid GXInput::UnBindLeftTriggerFunc ()
{
	DoLeftTrigger = 0;
}

GXVoid GXInput::BindRightTriggerFunc ( PFNGXTRIGGERPROC callback )
{
	DoRightTrigger = callback;
}

GXVoid GXInput::UnBindRightTriggerFunc ()
{
	DoRightTrigger = 0;
}

GXVoid GXInput::BindLeftStickFunc ( PFNGXSTICKPROC callback )
{
	DoLeftStick = callback;
}

GXVoid GXInput::UnBindLeftStickFunc ()
{
	DoLeftStick = 0;
}

GXVoid GXInput::BindRightStickFunc ( PFNGXSTICKPROC callback )
{
	DoRightStick = callback;
}

GXVoid GXInput::UnBindRightStickFunc ()
{
	DoRightStick = 0;
}

BYTE gx_inputKeys[ 256 ];
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
			core->Exit ();
			PostQuitMessage ( 0 );
		return 0;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			gx_inputActiveDevice = KEYBOARD;
			keysMask[ wParam << 1 ] = GX_TRUE;

			if ( !OnType ) return 0;

			GXBool tests[ 5 ];
			tests[ 0 ] = wParam == VK_SPACE;
			tests[ 1 ] = ( wParam >= VK_KEY_0 ) && ( wParam <= VK_KEY_9 );
			tests[ 2 ] = ( wParam >= VK_KEY_A ) && ( wParam <= VK_KEY_Z );
			tests[ 3 ] = ( wParam >= VK_OEM_1 ) && ( wParam <= VK_OEM_3 );
			tests[ 4 ] = ( wParam >= VK_OEM_4 ) && ( wParam <= VK_OEM_8 );

			GXBool result = tests[ 0 ] || tests[ 1 ] || tests[ 2 ] || tests[ 3 ] || tests[ 4 ];
			if ( !result ) return 0;

			GetKeyboardState ( gx_inputKeys );
			GXWChar buff[ 20 ];
			if ( ToUnicode ( (UINT)wParam, (UINT)lParam, gx_inputKeys, buff, 20, 0 ) )
				OnType ( buff[ 0 ], onTypeHandler );
		}
		return 0;

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			keysMask[ ( wParam << 1 ) + 1 ] = GX_TRUE;
		}
		return 0;

		case WM_SIZE:
			//core->NotifyResize ( LOWORD ( lParam ), HIWORD ( lParam ) );
		return 0;
		
		case WM_LBUTTONDOWN:
		{
			if ( DoMouseButtons )
			{
				mouseflags.lmb = 1;
				DoMouseButtons ( mouseflags );
			}

			GXVec2 pos;
			pos.x = (GXFloat)LOWORD ( lParam );
			pos.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - HIWORD ( lParam ) );

			gx_Core->GetTouchSurface ()->OnLeftMouseButtonDown ( pos );
		}
		return 0;
		
		case WM_RBUTTONDOWN:
		{
			if ( DoMouseButtons )
			{
				mouseflags.rmb = 1;
				DoMouseButtons ( mouseflags );
			}

			GXVec2 pos;
			pos.x = (GXFloat)LOWORD ( lParam );
			pos.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - HIWORD ( lParam ) );

			gx_Core->GetTouchSurface ()->OnRightMouseButtonDown ( pos );
		}
		return 0;

		case WM_MBUTTONDOWN:
		{
			if ( DoMouseButtons )
			{
				mouseflags.mmb = 1;
				DoMouseButtons ( mouseflags );
			}

			GXVec2 pos;
			pos.x = (GXFloat)LOWORD ( lParam );
			pos.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - HIWORD ( lParam ) );

			gx_Core->GetTouchSurface ()->OnMiddleMouseButtonDown ( pos );
		}
		return 0;
		
		case WM_LBUTTONUP:
		{
			if ( DoMouseButtons )
			{
				mouseflags.lmb = 0;
				DoMouseButtons ( mouseflags );
			}

			GXVec2 pos;
			pos.x = (GXFloat)LOWORD ( lParam );
			pos.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - HIWORD ( lParam ) );

			gx_Core->GetTouchSurface ()->OnLeftMouseButtonUp ( pos );
		}
		return 0;
		
		case WM_RBUTTONUP:
		{
			if ( DoMouseButtons )
			{
				mouseflags.rmb = 0;
				DoMouseButtons ( mouseflags );
			}

			GXVec2 pos;
			pos.x = (GXFloat)LOWORD ( lParam );
			pos.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - HIWORD ( lParam ) );

			gx_Core->GetTouchSurface ()->OnRightMouseButtonUp ( pos );
		}
		return 0;

		case WM_MBUTTONUP:
		{
			if ( DoMouseButtons )
			{
				mouseflags.mmb = 0;
				DoMouseButtons ( mouseflags );
			}

			GXVec2 pos;
			pos.x = (GXFloat)LOWORD ( lParam );
			pos.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - HIWORD ( lParam ) );

			gx_Core->GetTouchSurface ()->OnMiddleMouseButtonUp ( pos );
		}
		return 0;
		
		case WM_MOUSEMOVE:
		{
			if ( DoMouseMoving )
				DoMouseMoving ( LOWORD ( lParam ), HIWORD ( lParam ) );

			GXVec2 pos;
			pos.x = (GXFloat)LOWORD ( lParam );
			pos.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - HIWORD ( lParam ) );

			gx_Core->GetTouchSurface ()->OnMouseMove ( pos );
		}
		return 0;

		case WM_MOUSEWHEEL:
		{
			GXInt steps = GET_WHEEL_DELTA_WPARAM ( wParam ) / WHEEL_DELTA;
			if ( DoMouseWheel )
				DoMouseWheel ( steps );

			POINT posRaw;
			posRaw.x = GET_X_LPARAM ( lParam );
			posRaw.y = GET_Y_LPARAM ( lParam );
			ScreenToClient ( hwnd, &posRaw );

			GXVec2 pos;
			pos.x = (GXFloat)posRaw.x;
			pos.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - posRaw.y );

			gx_Core->GetTouchSurface ()->OnScroll ( pos, (GXFloat)steps );
		}
		return 0;

		case WM_SETCURSOR:
			//NOTHING
		return 0;

		case WM_LBUTTONDBLCLK:
		{
			GXVec2 pos;
			pos.x = (GXFloat)GET_X_LPARAM ( lParam ); 
			pos.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - GET_Y_LPARAM ( lParam ) ); 

			gx_Core->GetTouchSurface ()->OnDoubleClick ( pos );
		}
		return 0;
	}

	return DefWindowProc ( hwnd, msg, wParam, lParam );
}

GXDword GXTHREADCALL GXInput::InputLoop ( GXVoid* args )
{
	while ( loopFlag )
	{
		UpdateGamepad ();

		switch ( gx_inputActiveDevice )
		{
			case KEYBOARD:
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
					OnType ( symbol, onTypeHandler );
			}
			break;

			case XBOX_CONTROLLER:
			default:
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

	if ( GXXInputGetState ( gamepadID, &state ) == ERROR_DEVICE_NOT_CONNECTED ) 
		return GX_FALSE;
	else
		return GX_TRUE;
}

GXVoid GXInput::TestGamepadButton ( GXDword buttonFlag, GXUChar buttonID )
{
	GXUChar oldGamepadState = ( currentGamepadState == 0 ) ? 1 : 0;

	if ( ( gamepadState[ currentGamepadState ].Gamepad.wButtons & buttonFlag ) && !( gamepadState[ oldGamepadState ].Gamepad.wButtons & buttonFlag ) )
	{
		gx_inputActiveDevice = XBOX_CONTROLLER;
		gamepadKeysMask[ buttonID << 1 ] = GX_TRUE;
	}
	else if ( !( gamepadState[ currentGamepadState ].Gamepad.wButtons & buttonFlag ) && ( gamepadState[ oldGamepadState ].Gamepad.wButtons & buttonFlag ) )
		gamepadKeysMask[ ( buttonID << 1 ) + 1 ] = GX_TRUE;
}

GXVoid GXInput::UpdateGamepad ()
{
	if ( GXXInputGetState ( 0, &gamepadState[ currentGamepadState ] ) == ERROR_DEVICE_NOT_CONNECTED ) return;
	
	if ( gx_inputActiveDevice == XBOX_CONTROLLER )
	{
		if ( DoLeftStick )
			DoLeftStick ( gamepadState[ currentGamepadState ].Gamepad.sThumbLX * GX_INPUT_INV_STICK_VALUE, gamepadState[ currentGamepadState ].Gamepad.sThumbLY * GX_INPUT_INV_STICK_VALUE ); 

		if ( DoRightStick )
			DoRightStick ( gamepadState[ currentGamepadState ].Gamepad.sThumbRX * GX_INPUT_INV_STICK_VALUE, gamepadState[ currentGamepadState ].Gamepad.sThumbRY * GX_INPUT_INV_STICK_VALUE );

		if ( DoLeftTrigger )
			DoLeftTrigger ( gamepadState[ currentGamepadState ].Gamepad.bLeftTrigger * GX_INPUT_INV_TRIGGER_VALUE );

		if ( DoRightTrigger )
			DoRightTrigger ( gamepadState[ currentGamepadState ].Gamepad.bRightTrigger * GX_INPUT_INV_TRIGGER_VALUE );
	}

	GXUChar oldGamepadState = ( currentGamepadState == 0 ) ? 1 : 0;
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
