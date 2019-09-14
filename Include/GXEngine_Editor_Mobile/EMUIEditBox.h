#ifndef EM_UI_EDIT_BOX
#define EM_UI_EDIT_BOX


#include "EMUI.h"
#include <GXEngine/GXUIEditBox.h>


class EMUIEditBox final : public EMUI
{
    private:
        GXUIEditBox     _widget;

    public:
        explicit EMUIEditBox ( EMUI* parent );
        ~EMUIEditBox () override;

        GXWidget* GetWidget () override;

        GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );

        GXVoid SetText ( const GXString &text );
        const GXString& GetText ();

        GXVoid SetAlignment ( eGXUITextAlignment alignment );
        eGXUITextAlignment GetAlignment () const;

        GXVoid SetFont ( const GXString &fontFile, GXUShort fontSize );
        GXFont* GetFont ();

        GXVoid SetValidator ( GXTextValidator &validator );
        GXTextValidator* GetValidator () const;

        GXVoid SetOnFinishEditingCallback ( GXVoid* context, GXUIEditBoxOnCommitHandler callback );

    private:
        EMUIEditBox () = delete;
        EMUIEditBox ( const EMUIEditBox &other ) = delete;
        EMUIEditBox& operator = ( const EMUIEditBox &other ) = delete;
};


#endif // EM_UI_EDIT_BOX
