// version 1.0

#ifndef GX_DESKTOP_INPUT
#define GX_DESKTOP_INPUT


#include <GXCommon/GXStrings.h>


enum class eGXKeyboardKey : GXUShort
{
    Q = 0u,
    W,
    E,
    R,
    T,
    Y,
    U,
    I,
    O,
    P,
    A,
    S,
    D,
    F,
    G,
    H,
    J,
    K,
    L,
    Z,
    X,
    C,
    V,
    B,
    N,
    M,
    Space,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Zero,
    Minus,
    Equal,
    Backspace,
    Esc,
    Tilde,
    Tab,
    CapsLock,
    LeftShift,
    LeftCtrl,
    LeftAlt,
    RightAlt,
    RightCtrl,
    RightShift,
    Enter,
    BackSlash,
    ContextMenu,
    OpeningSquareBracket,
    ClosingSquareBracket,
    Semicolon,
    Quote,
    Less,
    Greater,
    Slash,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    PrintScreen,
    ScrollLock,
    Pause,
    Insert,
    Home,
    PageUp,
    Delete,
    End,
    PageDown,
    Up,
    Left,
    Down,
    Right,
    NumLock,
    NumpadDivide,
    NumpadMultiply,
    NumpadMinus,
    NumpadPlus,
    NumpadEnter,
    NumpadDot,
    Numpad0,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9
};

enum class eGXMouseButton : GXUByte
{
    LeftButton = 0u,
    MiddleButton,
    RightButton,
    ScrollUp,
    ScrollDown,
    Mouse4,
    Mouse5
};

enum class eGXButtonState : GXUByte
{
    Down = 0u,
    Up
};

constexpr const GXUPointer GX_DESKTOP_INPUT_TOTAL_KEYBOARD_KEYS = static_cast<GXUPointer> ( static_cast<GXUShort> ( eGXKeyboardKey::Numpad9 ) ) + 1u;;
constexpr const GXUPointer GX_DESKTOP_INPUT_TOTAL_MOUSE_BUTTONS = static_cast<GXUPointer> ( static_cast<GXUShort> ( eGXMouseButton::Mouse5 ) ) + 1u;
constexpr const GXUPointer GX_DESKTOP_INPUT_TOTAL_BUTTON_STATES = static_cast<GXUPointer> ( static_cast<GXUShort> ( eGXButtonState::Up ) ) + 1u;

typedef GXVoid ( GXCALL* GXKeyHandler ) ( GXVoid* context );
typedef GXVoid ( GXCALL* GXMouseButtonHandler ) ( GXVoid* context, GXInt x, GXInt y );
typedef GXVoid ( GXCALL* GXSymbolHandler ) ( GXVoid* context, GXStringSymbol symbol );

// Note that there is no universal GUI axis convention (see Windows vs macOS).
// So GXEngine treats (0, 0) point at left bottom corner.
typedef GXVoid ( GXCALL* GXMouseMoveHandler ) ( GXVoid* context, GXInt x, GXInt y, GXInt deltaX, GXInt deltaY );

// Note tick value is platform specific for now. Sorry for that.
typedef GXVoid ( GXCALL* GXMouseScrollHandler ) ( GXVoid* context, GXInt ticks, GXInt x, GXInt y );

//---------------------------------------------------------------------------------------------------------------------

class GXKeyBind final
{
    public:
        GXVoid*         _context;
        GXKeyHandler    _handler;

    public:
        GXKeyBind () = default;
        GXKeyBind ( const GXKeyBind &other );

        ~GXKeyBind () = default;

        GXVoid Init ( GXVoid* context, GXKeyHandler handler );
        GXVoid Reset ();

        GXKeyBind& operator = ( const GXKeyBind &other );
};

//---------------------------------------------------------------------------------------------------------------------

class GXMouseButtonBind final
{
    public:
        GXVoid*                 _context;
        GXMouseButtonHandler    _handler;

    public:
        GXMouseButtonBind () = default;
        GXMouseButtonBind ( const GXMouseButtonBind &other );

        ~GXMouseButtonBind () = default;

        GXVoid Init ( GXVoid* context, GXMouseButtonHandler handler );
        GXVoid Reset ();

        GXMouseButtonBind& operator = ( const GXMouseButtonBind &other );
};

//---------------------------------------------------------------------------------------------------------------------

// It's is base class for desktop input backends
class GXAbstractDesktopInput : public GXMemoryInspector
{
    public:
        virtual GXVoid Start () = 0;
        virtual GXVoid Shutdown () = 0;

        virtual GXVoid BindKeyboardKey ( GXVoid* context, GXKeyHandler handler, eGXKeyboardKey key, eGXButtonState state ) = 0;
        virtual GXVoid UnbindKeyboardKey ( eGXKeyboardKey key, eGXButtonState state ) = 0;

        virtual GXVoid LockKeyboard ( GXVoid* context, GXSymbolHandler handler ) = 0;
        virtual GXVoid UnlockKeyboard () = 0;

        // Note GXEngine treats (0, 0) point at left bottom corner.
        virtual GXVoid BindMouseMove ( GXVoid* context, GXMouseMoveHandler handler ) = 0;
        virtual GXVoid UnbindMouseMove () = 0;

        virtual GXVoid BindMouseButton ( GXVoid* context, GXMouseButtonHandler handler, eGXMouseButton button, eGXButtonState state ) = 0;
        virtual GXVoid UnbindMouseButton ( eGXMouseButton button, eGXButtonState state ) = 0;

        virtual GXVoid BindMouseScroll ( GXVoid* context, GXMouseScrollHandler handler ) = 0;
        virtual GXVoid UnbindMouseScroll () = 0;

        // This method wraps all OS specific.
        virtual GXVoid* ProcessOSMessage ( GXVoid* message ) = 0;

    protected:
        GXAbstractDesktopInput ();
        virtual ~GXAbstractDesktopInput () = default;

    private:
        GXAbstractDesktopInput ( const GXAbstractDesktopInput &other ) = delete;
        GXAbstractDesktopInput& operator = ( const GXAbstractDesktopInput &other ) = delete;
};


#ifdef __GNUC__

#include "Posix/GXDesktopInput.h"

#else

#include "Windows/GXDesktopInput.h"

#endif // __GNU__


#endif // GX_DESKTOP_INPUT
