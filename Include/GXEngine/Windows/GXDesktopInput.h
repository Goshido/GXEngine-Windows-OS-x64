// version 1.0

#ifndef GX_DESKTOP_INPUT_WINDOWS
#define GX_DESKTOP_INPUT_WINDOWS


#include <GXEngine/GXDesktopInput.h>
#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXSmartLock.h>
#include <GXCommon/GXThread.h>


class GXKeyNode final : public GXAVLTreeNode
{
    public:
        WPARAM              _virtualCode;
        eGXKeyboardKey      _key;

    public:
        GXKeyNode () = default;
        explicit GXKeyNode ( WPARAM virtualCode );
        explicit GXKeyNode ( WPARAM virtualCode, eGXKeyboardKey key );
        ~GXKeyNode () = default;

        static eGXCompareResult Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );

    private:
        GXKeyNode ( const GXKeyNode &other ) = delete;
        GXKeyNode& operator = ( const GXKeyNode &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

class GXInputKeyAction final
{
    public:
        GXKeyBind               _bind;
        GXInputKeyAction*       _next;

    public:
        GXInputKeyAction () = default;
        GXInputKeyAction ( const GXInputKeyAction &other );
        ~GXInputKeyAction () = default;

        GXInputKeyAction& operator = ( const GXInputKeyAction &other );
};

//---------------------------------------------------------------------------------------------------------------------

class GXInputMouseButtonAction final
{
    public:
        GXMouseButtonBind               _bind;
        GXInputMouseButtonAction*       _next;

    public:
        GXInputMouseButtonAction () = default;
        GXInputMouseButtonAction ( const GXInputMouseButtonAction &other );
        ~GXInputMouseButtonAction () = default;

        GXInputMouseButtonAction& operator = ( const GXInputMouseButtonAction &other );
};

//---------------------------------------------------------------------------------------------------------------------

// WM_MOUSEMOVE, WM_LBUTTONDOWN, WM_LBUTTONUP, WM_RBUTTONDOWN,
// WM_RBUTTONUP, WM_MBUTTONDOWN, WM_MBUTTONUP, WM_MOUSEWHEEL,
// WM_KEYDOWN, WM_SYSKEYDOWN, WM_KEYUP, WM_SYSKEYUP
constexpr const GXUPointer GX_DESKTOP_INPUT_TOTAL_OS_MESSAGE_TYPES = 12u;

class GXDesktopInput;
typedef LRESULT ( GXDesktopInput::* GXDesktopInputHandler ) ( const MSG &message );

class GXOSMessageNode final : public GXAVLTreeNode
{
    public:
        UINT                        _osType;
        GXDesktopInputHandler       _handler;

    public:
        GXOSMessageNode () = default;
        explicit GXOSMessageNode ( UINT osType );
        explicit GXOSMessageNode ( UINT osType, GXDesktopInputHandler handler );
        ~GXOSMessageNode () = default;

        static eGXCompareResult Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );

    private:
        GXOSMessageNode ( const GXOSMessageNode &other ) = delete;
        GXOSMessageNode& operator = ( const GXOSMessageNode &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

class GXDesktopInput final : public GXAbstractDesktopInput
{
    private:
        // Optimization stuff.
        GXAVLTree                       _keyboardMapper;
        GXAVLTree                       _messageTypeMapper;

        GXInputKeyAction*               _readyKeyActions;
        GXInputMouseButtonAction*       _readyMouseActions;

        GXInt                           _commitedMouseX;
        GXInt                           _commitedMouseY;
        GXInt                           _mouseScrollTicks;
        GXInt                           _lastMouseX;
        GXInt                           _lastMouseY;

        GXMouseMoveHandler              _mouseMoveHandler;
        GXVoid*                         _mouseMoveContext;
        GXBool                          _isMouseMoveEvent;

        GXMouseScrollHandler            _mouseScrollHandler;
        GXVoid*                         _mouseScrollContext;
        GXBool                          _isMouseScrollEvent;

        GXBool                          _isLoop;
        GXThread*                       _thread;

        GXSmartLock                     _smartLock;

        GXKeyBind                       _keyDownBinds[ GX_DESKTOP_INPUT_TOTAL_KEYBOARD_KEYS ];
        GXKeyBind                       _keyUpBinds[ GX_DESKTOP_INPUT_TOTAL_KEYBOARD_KEYS ];

        // Note Windows OS spams key down events if user holds key.
        // It is not very useful for key bind cases, but it is useful for typing cases.
        // So this field will filter excessive key down events for key down handlers.
        eGXButtonState                  _keyDownFilters[ GX_DESKTOP_INPUT_TOTAL_KEYBOARD_KEYS ];

        GXMouseButtonBind               _mouseButtonDownBinds[ GX_DESKTOP_INPUT_TOTAL_MOUSE_BUTTONS ];
        GXMouseButtonBind               _mouseButtonUpBinds[ GX_DESKTOP_INPUT_TOTAL_MOUSE_BUTTONS ];

        // Optimization stuff.
        GXKeyNode                       _keyMap[ GX_DESKTOP_INPUT_TOTAL_KEYBOARD_KEYS ];
        GXOSMessageNode                 _osMessageMap[ GX_DESKTOP_INPUT_TOTAL_OS_MESSAGE_TYPES ];
        GXInputKeyAction                _keyActionPool[ GX_DESKTOP_INPUT_TOTAL_BUTTON_STATES * GX_DESKTOP_INPUT_TOTAL_KEYBOARD_KEYS ];
        GXInputKeyAction*               _freeKeyActions;

        GXInputMouseButtonAction        _mouseButtonActionPool[ GX_DESKTOP_INPUT_TOTAL_BUTTON_STATES * GX_DESKTOP_INPUT_TOTAL_MOUSE_BUTTONS ];
        GXInputMouseButtonAction*       _freeMouseButtonActions;

        static GXDesktopInput*          _instance;

    public:
        static GXDesktopInput& GXCALL GetInstance ();
        ~GXDesktopInput () override;

        GXVoid Start () override;
        GXVoid Shutdown () override;

        GXVoid BindKeyboardKey ( GXVoid* context, GXKeyHandler handler, eGXKeyboardKey key, eGXButtonState state ) override;
        GXVoid UnbindKeyboardKey ( eGXKeyboardKey key, eGXButtonState state ) override;

        GXVoid LockKeyboard ( GXVoid* context, GXSymbolHandler handler ) override;
        GXVoid UnlockKeyboard () override;

        // Note GXEngine treats (0, 0) point at left bottom corner.
        GXVoid BindMouseMove ( GXVoid* context, GXMouseMoveHandler handler ) override;
        GXVoid UnbindMouseMove () override;

        GXVoid BindMouseButton ( GXVoid* context, GXMouseButtonHandler handler, eGXMouseButton button, eGXButtonState state ) override;
        GXVoid UnbindMouseButton ( eGXMouseButton button, eGXButtonState state ) override;

        GXVoid BindMouseScroll ( GXVoid* context, GXMouseScrollHandler handler ) override;
        GXVoid UnbindMouseScroll () override;

        GXVoid* ProcessOSMessage ( GXVoid* message ) override;

    private:
        GXDesktopInput ();

        // Note "_smartLock" must be locked.
        GXVoid AddAction ( const GXKeyBind &bind );
        GXVoid AddAction ( const GXMouseButtonBind &bind );

        GXVoid ExecuteKeyEvents ();
        GXVoid ExecuteMouseButtonEvents ();
        GXVoid ExecuteMouseMoveEvents ();
        GXVoid ExecuteMouseScrollEvents ();

        GXVoid InitActionPool ();
        GXVoid InitBinds ();
        GXVoid InitKeyMappers ();
        GXVoid InitOSMessageMapper ();

        // "ignoreIfEqual" prevents spamming. Method will check "_keyDownFilters" field
        // to make decision to invoke key handler or not.
        LRESULT HandleKeyInternal ( GXKeyBind const* const& allBinds, const MSG &message, eGXButtonState ignoreIfEqual );

        LRESULT HandleMouseButtonInternal ( GXMouseButtonBind const* const& allBinds, eGXMouseButton button, const MSG &message );

        // Handlers:
        // Keyboard:
        LRESULT HandleKeyDown ( const MSG &message );
        LRESULT HandleKeyUp ( const MSG &message );

        // Mouse:
        LRESULT HandleMouseLeftButtonDown ( const MSG &message );
        LRESULT HandleMouseLeftButtonUp ( const MSG &message );
        LRESULT HandleMouseMiddleButtonDown ( const MSG &message );
        LRESULT HandleMouseMiddleButtonUp ( const MSG &message );
        LRESULT HandleMouseRightButtonDown ( const MSG &message );
        LRESULT HandleMouseRightButtonUp ( const MSG &message );
        LRESULT HandleMouseMove ( const MSG &message );
        LRESULT HandleMouseScroll ( const MSG &message );

        WPARAM ResolveNativeKeyVirtualCode ( const MSG &message ) const;

        static GXUPointer GXTHREADCALL ThreadFunction ( GXVoid* argument, GXThread &thread );

        GXDesktopInput ( const GXDesktopInput &other ) = delete;
        GXDesktopInput& operator = ( const GXDesktopInput &other ) = delete;
};


#endif // GX_DESKTOP_INPUT_WINDOWS
