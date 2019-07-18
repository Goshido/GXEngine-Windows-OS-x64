#ifndef EM_UI_MENU
#define EM_UI_MENU


#include "EMUI.h"
#include "EMUIPopup.h"
#include <GXEngine/GXUIMenu.h>


class EMUIMenu final : public EMUI
{
    private:
        GXUIMenu*       _widget;

    public:
        explicit EMUIMenu ( EMUI* parent );
        ~EMUIMenu () override;

        GXWidget* GetWidget () const override;

        GXVoid AddItem ( const GXWChar* name, EMUIPopup* popup );
        GXVoid SetLocation ( GXFloat leftBottomX, GXFloat leftBottomY );
        GXFloat GetHeight () const;

    private:
        EMUIMenu () = delete;
        EMUIMenu ( const EMUIMenu &other ) = delete;
        EMUIMenu& operator = ( const EMUIMenu &other ) = delete;
};


#endif // EM_UI_MENU
