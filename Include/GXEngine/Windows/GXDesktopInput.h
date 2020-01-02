// version 1.0

#ifndef GX_DESKTOP_INPUT_WINDOWS
#define GX_DESKTOP_INPUT_WINDOWS


#include <GXEngine/GXDesktopInput.h>
#include <GXCommon/GXSmartLock.h>
#include <GXCommon/GXAVLTree.h>


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

class GXDesktopInput final : public GXAbstractDesktopInput
{
    private:
        GXAVLTree                   _keyboardMapper;

        GXSmartLock                 _smartLock;
        GXKeyNode                   _keyMap[ GX_DESKTOP_INPUT_TOTAL_KEYBOARD_KEYS ];

        static GXDesktopInput*      _instance;

    public:
        static GXDesktopInput& GXCALL GetInstance ();
        ~GXDesktopInput () override;

        GXVoid BindKeyboardKey ( GXVoid* context, GXKeyHandler handler, eGXKeyboardKey key, eGXButtonState state ) override;
        GXVoid UnbindKeyboardKey ( eGXKeyboardKey key, eGXButtonState state ) override;

        GXVoid LockKeyboard ( GXVoid* context, GXSymbolHandler handler ) override;
        GXVoid UnlockKeyboard () override;

        // Note GXEngine treats (0, 0) point at left bottom corner.
        GXVoid BindMouseMove ( GXVoid* context, GXMouseMoveHandler handler ) override;
        GXVoid UnbindMouseMove () override;

        GXVoid BindMouseButton ( GXVoid* context, GXKeyHandler handler, eGXMouseButton button, eGXButtonState state ) override;
        GXVoid UnbindMouseButton ( eGXMouseButton button, eGXButtonState state ) override;

        GXVoid BindMouseScroll ( GXVoid* context, GXMouseScrollHandler handler ) override;
        GXVoid UnbindMouseScroll ( GXVoid* context, GXMouseScrollHandler handler ) override;

    private:
        GXDesktopInput ();

        GXVoid InitKeyMappers ();

        GXDesktopInput ( const GXDesktopInput &other ) = delete;
        GXDesktopInput& operator = ( const GXDesktopInput &other ) = delete;
};


#endif // GX_DESKTOP_INPUT_WINDOWS
