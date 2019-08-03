// version 1.6

#ifndef GX_UI_INPUT
#define GX_UI_INPUT


#include "GXWidget.h"


class GXUIInput;
typedef GXVoid ( GXCALL* GXUIInputOnMouseMoveHandler ) ( GXVoid* context, GXUIInput &input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* GXUIInputOnMouseButtonHandler ) ( GXVoid* context, GXUIInput &input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* GXUIInputOnDoubleClickHandler ) ( GXVoid* handler, GXUIInput &input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* GXUIInputOnScrollHandler ) ( GXVoid* handler, GXUIInput &input, GXFloat scroll, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* GXUIInputOnMouseOverHandler ) ( GXVoid* handler, GXUIInput &input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* GXUIInputOnMouseLeaveHandler ) ( GXVoid* handler, GXUIInput &input, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* GXUIInputOnKeyHandler ) ( GXVoid* handler, GXUIInput &input, GXInt key );


class GXUIInput final : public GXWidget
{
    private:
        GXUIInputOnMouseButtonHandler       _onLMBDown;
        GXUIInputOnMouseButtonHandler       _onLMBUp;
        GXUIInputOnMouseButtonHandler       _onMMBDown;
        GXUIInputOnMouseButtonHandler       _onMMBUp;
        GXUIInputOnMouseButtonHandler       _onRMBDown;
        GXUIInputOnMouseButtonHandler       _onRMBUp;

        GXUIInputOnDoubleClickHandler       _onDoubleClick;
        GXUIInputOnScrollHandler            _onMouseScroll;
        GXUIInputOnMouseMoveHandler         _onMouseMove;
        GXUIInputOnMouseOverHandler         _onMouseOver;
        GXUIInputOnMouseLeaveHandler        _onMouseLeave;

        GXUIInputOnKeyHandler               _onKeyDown;
        GXUIInputOnKeyHandler               _onKeyUp;

        GXVoid*                             _handler;

    public:
        explicit GXUIInput ( GXWidget* parent, GXBool isNeedRegister );
        ~GXUIInput () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

        GXVoid SetOnLeftMouseButtonDownCallback ( GXUIInputOnMouseButtonHandler callback );
        GXVoid SetOnLeftMouseButtonUpCallback ( GXUIInputOnMouseButtonHandler callback );
        GXVoid SetOnMiddleMouseButtonDownCallback ( GXUIInputOnMouseButtonHandler callback );
        GXVoid SetOnMiddleMouseButtonUpCallback ( GXUIInputOnMouseButtonHandler callback );
        GXVoid SetOnRightMouseButtonDownCallback ( GXUIInputOnMouseButtonHandler callback );
        GXVoid SetOnRightMouseButtonUpCallback ( GXUIInputOnMouseButtonHandler callback );

        GXVoid SetOnDoubleClickCallback ( GXUIInputOnDoubleClickHandler callback );
        GXVoid SetOnMouseMoveCallback ( GXUIInputOnMouseMoveHandler callback );
        GXVoid SetOnMouseScrollCallback ( GXUIInputOnScrollHandler callback );
        GXVoid SetOnMouseOverCallback ( GXUIInputOnMouseOverHandler callback );
        GXVoid SetOnMouseLeaveCallback ( GXUIInputOnMouseLeaveHandler callback );

        GXVoid SetOnKeyDownCallback ( GXUIInputOnKeyHandler callback );
        GXVoid SetOnKeyUpCallback ( GXUIInputOnKeyHandler callback );

        GXVoid SetHandler ( GXVoid* handler );

    private:
        GXUIInput () = delete;
        GXUIInput ( const GXUIInput &other ) = delete;
        GXUIInput& operator = ( const GXUIInput &other ) = delete;
};


#endif // GX_UI_INPUT
