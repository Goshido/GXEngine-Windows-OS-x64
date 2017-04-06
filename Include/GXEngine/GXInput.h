//version 1.14

#ifndef GX_INPUT
#define GX_INPUT


#include <GXCommon/GXThread.h>
#include <GXEngineDLL/GXXInput.h>


#define GX_INPUT_TOTAL_KEYBOARD_KEYS		256 * 2
#define GX_INPUT_TOTAL_GAMEPAD_KEYS			14


#define VK_KEY_0	0x30
#define VK_KEY_1	0x31
#define VK_KEY_2	0x32
#define VK_KEY_3	0x33
#define VK_KEY_4	0x34
#define VK_KEY_5	0x35
#define VK_KEY_6	0x36
#define VK_KEY_7	0x37
#define VK_KEY_8	0x38
#define VK_KEY_9	0x39

#define VK_KEY_A	0x41
#define VK_KEY_B	0x42
#define VK_KEY_C	0x43
#define VK_KEY_D	0x44
#define VK_KEY_E	0x45
#define VK_KEY_F	0x46
#define VK_KEY_G	0x47
#define VK_KEY_H	0x48
#define VK_KEY_I	0x49
#define VK_KEY_J	0x4A
#define VK_KEY_K	0x4B
#define VK_KEY_L	0x4C
#define VK_KEY_M	0x4D
#define VK_KEY_N	0x4E
#define VK_KEY_O	0x4F
#define VK_KEY_P	0x50
#define VK_KEY_Q	0x51
#define VK_KEY_R	0x52
#define VK_KEY_S	0x53
#define VK_KEY_T	0x54
#define VK_KEY_U	0x55
#define VK_KEY_V	0x56
#define VK_KEY_W	0x57
#define VK_KEY_X	0x58
#define VK_KEY_Y	0x59
#define VK_KEY_Z	0x5A

#define GX_INPUT_XBOX_A					0
#define GX_INPUT_XBOX_B					1
#define GX_INPUT_XBOX_X					2
#define GX_INPUT_XBOX_Y					3
#define GX_INPUT_XBOX_CROSS_UP			4
#define GX_INPUT_XBOX_CROSS_DOWN		5
#define GX_INPUT_XBOX_CROSS_LEFT		6
#define GX_INPUT_XBOX_CROSS_RIGHT		7
#define GX_INPUT_XBOX_SHOULDER_LEFT		8
#define GX_INPUT_XBOX_SHOULDER_RIGHT	9
#define GX_INPUT_XBOX_THUMB_LEFT		10
#define GX_INPUT_XBOX_THUMB_RIGHT		11
#define GX_INPUT_XBOX_START				12
#define GX_INPUT_XBOX_BACK				13


enum class eGXInputButtonState
{
	Down,
	Up
};

union GXInputMouseFlags
{
	struct
	{
		GXUChar lmb 		: 1;
		GXUChar mmb 		: 1;
		GXUChar rmb 		: 1;
		GXUChar reserved 	: 5;
	};
	GXUByte allflags;
};


typedef GXVoid ( GXCALL* PFNGXTYPEPROC ) ( GXWChar symbol, GXVoid* handler );
typedef GXVoid ( GXCALL* PFNGXKEYPROC ) ( GXVoid* handler );
typedef GXVoid ( GXCALL* PFNGXMOUSEMOVEPROC ) ( GXInt win_x, GXInt win_y );
typedef GXVoid ( GXCALL* PFNGXMOUSEBUTTONSPROC ) ( GXInputMouseFlags mouseflags );
typedef GXVoid ( GXCALL* PFNGXMOUSEWHEELPROC ) ( GXInt steps );
typedef GXVoid ( GXCALL* PFNGXSTICKPROC ) ( GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXTRIGGERPROC ) ( GXFloat value );

enum class eGXInputDevice
{
	Keyboard,
	Mouse,
	xboxController
};

class GXInput
{
	private:
		static GXThread*				thread;

		static GXBool					loopFlag;

		static GXBool					keysMask[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
		static PFNGXKEYPROC				KeysMapping[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
		static GXVoid*					keysHandlers[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];

		static PFNGXTYPEPROC			OnType;
		static GXVoid*					onTypeHandler;
		static GXWChar					symbol;

		static XINPUT_STATE				gamepadState[ 2 ];
		static GXUChar					currentGamepadState;

		static PFNGXKEYPROC				GamepadKeysMapping[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];
		static GXByte					gamepadKeysMask[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];
		static GXVoid*					gamepadKeysHandlers[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2 ];

		static PFNGXTRIGGERPROC			DoLeftTrigger;
		static PFNGXTRIGGERPROC			DoRightTrigger;

		static PFNGXSTICKPROC			DoLeftStick;
		static PFNGXSTICKPROC			DoRightStick;

		static PFNGXMOUSEMOVEPROC		DoMouseMoving;
		static PFNGXMOUSEBUTTONSPROC	DoMouseButtons;
		static PFNGXMOUSEWHEELPROC		DoMouseWheel;
		static GXInputMouseFlags 		mouseflags;

		static eGXInputDevice			activeInputDevice;

		static PFNXINPUTGETSTATEPROC	XInputGetState;
		static PFNXINPUTENABLEPROC		XInputEnable;

		static GXInput*					instance;

	public:
		~GXInput ();

		GXVoid Start ();
		GXVoid Suspend ();
		GXVoid Shutdown ();

		GXVoid BindKeyFunc ( PFNGXKEYPROC callback, GXVoid* handler, GXInt vk_key, eGXInputButtonState eState );
		GXVoid UnBindKeyFunc ( GXInt vk_key, eGXInputButtonState eState );

		GXVoid BindTypeFunc ( PFNGXTYPEPROC callback, GXVoid* handler );
		GXVoid UnBindTypeFunc ();

		GXVoid BindMouseMoveFunc ( PFNGXMOUSEMOVEPROC callback );
		GXVoid UnBindMouseMoveFunc ();

		GXVoid BindMouseButtonsFunc ( PFNGXMOUSEBUTTONSPROC callback );
		GXVoid UnBindMouseButtonsFunc ();

		GXVoid BindMouseWheelFunc ( PFNGXMOUSEWHEELPROC callback );
		GXVoid UnBindMouseWheelFunc ();

		GXVoid BindGamepadKeyFunc ( PFNGXKEYPROC callback, GXVoid* handler, GXInt gamepad_key, eGXInputButtonState eState );
		GXVoid UnBindGamepadKeyFunc ( GXInt gamepad_key, eGXInputButtonState eState );

		GXVoid BindLeftTriggerFunc ( PFNGXTRIGGERPROC callback );
		GXVoid UnBindLeftTriggerFunc ();
		GXVoid BindRightTriggerFunc ( PFNGXTRIGGERPROC callback );
		GXVoid UnBindRightTriggerFunc ();
		
		GXVoid BindLeftStickFunc ( PFNGXSTICKPROC callback );
		GXVoid UnBindLeftStickFunc ();
		GXVoid BindRightStickFunc ( PFNGXSTICKPROC callback );
		GXVoid UnBindRightStickFunc ();

		static GXInput* GXCALL GetInstance ();

		static LRESULT CALLBACK InputProc ( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

	private:
		GXInput ();

		static GXDword GXTHREADCALL InputLoop ( GXVoid* args );

		static GXBool GXCALL IsGamepadConnected ( GXDword gamepadID );
		static GXVoid GXCALL TestGamepadButton ( GXDword buttonFlag, GXUChar buttonID );
		static GXVoid GXCALL UpdateGamepad ();

		static GXBool GXCALL InitXInputLibrary ();
		static GXBool GXCALL DestroyXInputLibrary ();
};


#endif //GX_INPUT
