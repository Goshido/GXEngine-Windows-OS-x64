#ifndef EM_UI_SEPARATOR
#define EM_UI_SEPARATOR


#include "EMUI.h"


class EMUISeparator final : public EMUI
{
    private:
        GXWidget*       _widget;

    public:
        explicit EMUISeparator ( EMUI* parent );
        ~EMUISeparator () override;

        GXWidget* GetWidget () const override;

        GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );

    private:
        EMUISeparator () = delete;
        EMUISeparator ( const EMUISeparator &other ) = delete;
        EMUISeparator& operator = ( const EMUISeparator &other ) = delete;
};


#endif // EM_UI_SEPARATOR
