#ifndef EM_UI_STATIC_TEXT
#define EM_UI_STATIC_TEXT


#include "EMUI.h"
#include <GXEngine/GXUIStaticText.h>


class EMUIStaticText final : public EMUI
{
    private:
        GXUIStaticText*     _widget;

    public:
        explicit EMUIStaticText ( EMUI* parent );
        ~EMUIStaticText () override;

        GXWidget* GetWidget () const override;

        GXVoid SetText ( const GXWChar* text );
        const GXWChar* GetText () const;

        GXVoid SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
        const GXColorRGB& GetTextColor () const;

        GXVoid SetAlingment ( eGXUITextAlignment alignment );
        eGXUITextAlignment GetAlignment () const;

        GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );

    private:
        EMUIStaticText () = delete;
        EMUIStaticText ( const EMUIStaticText &other ) = delete;
        EMUIStaticText& operator = ( const EMUIStaticText &other ) = delete;
};


#endif // EM_UI_STATIC_TEXT
