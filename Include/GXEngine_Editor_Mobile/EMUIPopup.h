#ifndef EM_UI_POPUP
#define EM_UI_POPUP


#include "EMUI.h"
#include <GXEngine/GXUIPopup.h>


class EMUIPopup final : public EMUI
{
    private:
        GXUIPopup       _widget;

    public:
        explicit EMUIPopup ( EMUI* parent );
        ~EMUIPopup () override;

        GXWidget* GetWidget () override;

        GXVoid AddItem ( GXString name, GXVoid* context, GXUIPopupActionHandler action );

        GXVoid EnableItem ( GXUByte itemIndex );
        GXVoid DisableItem ( GXUByte itemIndex );

        GXVoid SetLocation ( GXFloat x, GXFloat y );

        GXVoid Show ( EMUI* owner );

    private:
        EMUIPopup ( const EMUIPopup & other ) = delete;
        EMUIPopup& operator = ( const EMUIPopup & other ) = delete;
};


#endif // EM_UI_POPUP
