#ifndef EM_UI_BUTTON
#define EM_UI_BUTTON


#include "EMUI.h"
#include <GXEngine/GXUIButton.h>


class EMUIButton final : public EMUI
{
    private:
        GXUIButton*     _widget;

    public:
        explicit EMUIButton ( EMUI* parent );
        ~EMUIButton () override;

        GXWidget* GetWidget () const override;

        GXVoid Enable ();
        GXVoid Disable ();

        GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );

        GXVoid SetCaption ( const GXWChar* caption );

        GXVoid Show ();
        GXVoid Hide ();

        GXVoid SetOnLeftMouseButtonCallback ( GXVoid* context, GXUIButtonOnMouseButtonHandler callback );

    private:
        EMUIButton () = delete;
        EMUIButton ( const EMUIButton &other ) = delete;
        EMUIButton& operator = ( const EMUIButton &other ) = delete;
};


#endif // EM_UI_BUTTON
