// version 1.20

#ifndef GX_INPUT
#define GX_INPUT


#include <GXCommon/GXThread.h>
#include <GXEngineDLL/GXXInput.h>


#define GX_INPUT_TOTAL_KEYBOARD_KEYS    256u * 2u
#define GX_INPUT_TOTAL_GAMEPAD_KEYS     14u


#define VK_KEY_0                        0x30
#define VK_KEY_1                        0x31
#define VK_KEY_2                        0x32
#define VK_KEY_3                        0x33
#define VK_KEY_4                        0x34
#define VK_KEY_5                        0x35
#define VK_KEY_6                        0x36
#define VK_KEY_7                        0x37
#define VK_KEY_8                        0x38
#define VK_KEY_9                        0x39

#define VK_KEY_A                        0x41
#define VK_KEY_B                        0x42
#define VK_KEY_C                        0x43
#define VK_KEY_D                        0x44
#define VK_KEY_E                        0x45
#define VK_KEY_F                        0x46
#define VK_KEY_G                        0x47
#define VK_KEY_H                        0x48
#define VK_KEY_I                        0x49
#define VK_KEY_J                        0x4A
#define VK_KEY_K                        0x4B
#define VK_KEY_L                        0x4C
#define VK_KEY_M                        0x4D
#define VK_KEY_N                        0x4E
#define VK_KEY_O                        0x4F
#define VK_KEY_P                        0x50
#define VK_KEY_Q                        0x51
#define VK_KEY_R                        0x52
#define VK_KEY_S                        0x53
#define VK_KEY_T                        0x54
#define VK_KEY_U                        0x55
#define VK_KEY_V                        0x56
#define VK_KEY_W                        0x57
#define VK_KEY_X                        0x58
#define VK_KEY_Y                        0x59
#define VK_KEY_Z                        0x5A

#define GX_INPUT_XBOX_A                 0
#define GX_INPUT_XBOX_B                 1
#define GX_INPUT_XBOX_X                 2
#define GX_INPUT_XBOX_Y                 3
#define GX_INPUT_XBOX_CROSS_UP          4
#define GX_INPUT_XBOX_CROSS_DOWN        5
#define GX_INPUT_XBOX_CROSS_LEFT        6
#define GX_INPUT_XBOX_CROSS_RIGHT       7
#define GX_INPUT_XBOX_SHOULDER_LEFT     8
#define GX_INPUT_XBOX_SHOULDER_RIGHT    9
#define GX_INPUT_XBOX_THUMB_LEFT        10
#define GX_INPUT_XBOX_THUMB_RIGHT       11
#define GX_INPUT_XBOX_START             12
#define GX_INPUT_XBOX_BACK              13

//---------------------------------------------------------------------------------------------------------------------

enum class eGXInputButtonState : GXUByte
{
    Down,
    Up
};

enum class eGXInputDevice: GXUByte
{
    Keyboard,
    Mouse,
    XBOXController
};

struct GXInputMouseFlags
{
    GXBool      _leftMouseButton;
    GXBool      _middleMouseButton;
    GXBool      _rightMouseButton;
};

typedef GXVoid ( GXCALL* GXInputTypeHandler ) ( GXVoid* context, GXWChar symbol );
typedef GXVoid ( GXCALL* GXInputKeyHandler ) ( GXVoid* context );
typedef GXVoid ( GXCALL* GXInputMouseMoveHandler ) ( GXVoid* context, GXInt windowX, GXInt windowY );
typedef GXVoid ( GXCALL* GXInputMouseButtonHandler ) ( GXVoid* context, GXInputMouseFlags mouseflags );
typedef GXVoid ( GXCALL* GXInputMouseHandler ) ( GXVoid* context, GXInt steps );
typedef GXVoid ( GXCALL* GXInputStickHandler ) ( GXVoid* context, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* GXInputTriggerHandler ) ( GXVoid* context, GXFloat value );

class GXInput final
{
    private:
        static GXThread*                    _thread;

        static GXBool                       _loopFlag;

        static GXVoid*                      _keysHandlers[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
        static GXInputKeyHandler            _KeysMapping[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];
        static GXBool                       _keysMask[ GX_INPUT_TOTAL_KEYBOARD_KEYS ];

        static GXVoid*                      _onTypeContext;
        static GXInputTypeHandler           _onType;
        static GXWChar                      _symbol;

        static XINPUT_STATE                 _gamepadState[ 2u ];
        static GXUChar                      _currentGamepadState;

        static GXVoid*                      _gamepadKeysHandlers[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2u ];
        static GXInputKeyHandler            _gamepadKeysMapping[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2u ];
        static GXByte                       _gamepadKeysMask[ GX_INPUT_TOTAL_GAMEPAD_KEYS * 2u ];

        static GXVoid*                      _onLeftTriggerContext;
        static GXInputTriggerHandler        _doLeftTrigger;

        static GXVoid*                      _onRightTriggerContext;
        static GXInputTriggerHandler        _doRightTrigger;

        static GXVoid*                      _onLeftStickContext;
        static GXInputStickHandler          _doLeftStick;

        static GXVoid*                      _onRightStickContext;
        static GXInputStickHandler          _doRightStick;

        static GXVoid*                      _onMouseMoveContext;
        static GXInputMouseMoveHandler      _doMouseMoving;

        static GXVoid*                      _onMouseButtonContext;
        static GXInputMouseButtonHandler    _doMouseButton;

        static GXVoid*                      _onMouseWheelContext;
        static GXInputMouseHandler          _doMouseWheel;

        static GXInputMouseFlags            _mouseflags;

        static eGXInputDevice               _activeInputDevice;

        static PFNXINPUTGETSTATEPROC        _xInputGetState;
        static PFNXINPUTENABLEPROC          _xInputEnable;

        static GXInput*                     _instance;

    public:
        static GXInput& GXCALL GetInstance ();
        ~GXInput ();

        GXVoid Start ();
        GXVoid Shutdown ();

        GXVoid BindKeyCallback ( GXVoid* context, GXInputKeyHandler callback, GXInt virtualKeyCode, eGXInputButtonState state );
        GXVoid UnbindKeyCallback ( GXInt virtualKeyCode, eGXInputButtonState state );

        GXVoid BindTypeCallback ( GXVoid* context, GXInputTypeHandler callback );
        GXVoid UnbindTypeCallback ();

        GXVoid BindMouseMoveCallback ( GXVoid* context, GXInputMouseMoveHandler callback );
        GXVoid UnbindMouseMoveCallback ();

        GXVoid BindMouseButtonCallback ( GXVoid* context, GXInputMouseButtonHandler callback );
        GXVoid UnbindMouseButtonCallback ();

        GXVoid BindMouseWheelCallback ( GXVoid* context, GXInputMouseHandler callback );
        GXVoid UnbindMouseWheelCallback ();

        GXVoid BindGamepadKeyCallback ( GXVoid* context, GXInputKeyHandler callback, GXInt gamepadKey, eGXInputButtonState state );
        GXVoid UnbindGamepadKeyCallback ( GXInt gamepadKey, eGXInputButtonState state );

        GXVoid BindLeftTriggerCallback ( GXVoid* context, GXInputTriggerHandler callback );
        GXVoid UnbindLeftTriggerCallback ();

        GXVoid BindRightTriggerCallback ( GXVoid* context, GXInputTriggerHandler callback );
        GXVoid UnbindRightTriggerCallback ();
        
        GXVoid BindLeftStickCallback ( GXVoid* context, GXInputStickHandler callback );
        GXVoid UnbindLeftStickCallback ();

        GXVoid BindRightStickCallback ( GXVoid* context, GXInputStickHandler callback );
        GXVoid UnbindRightStickCallback ();

        static LRESULT CALLBACK InputProc ( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

    private:
        GXInput ();

        static GXUPointer GXTHREADCALL InputLoop ( GXVoid* args, GXThread &inputThread );

        static GXBool GXCALL IsGamepadConnected ( GXDword gamepadID );
        static GXVoid GXCALL TestGamepadButton ( GXDword buttonFlag, GXUChar buttonID );
        static GXVoid GXCALL UpdateGamepad ();

        static GXBool GXCALL InitXInputLibrary ();
        static GXBool GXCALL DestroyXInputLibrary ();

        GXInput ( const GXInput &other ) = delete;
        GXInput& operator = ( const GXInput &other ) = delete;
};


#endif // GX_INPUT
