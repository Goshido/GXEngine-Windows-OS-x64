#ifndef BB_LABEL
#define BB_LABEL


#include <GXEngine/GXUIStaticText.h>
#include <GXCommon/GXMemory.h>


class BBLabel final : public GXMemoryInspector
{
    private:
        GXUIStaticText      staticText;

    public:
        explicit BBLabel ( GXWidget* parent );
        ~BBLabel () override;

        GXVoid SetText ( const GXWChar* text );
        GXVoid SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
        GXVoid SetAlignment ( eGXUITextAlignment alignment );

        const GXWChar* GetText () const;
        const GXColorRGB& GetTextColor () const;
        eGXUITextAlignment GetAlignment () const;

        GXVoid Resize ( GXFloat x, GXFloat y, GXFloat width, GXFloat height );

    private:
        BBLabel () = delete;
        BBLabel ( const BBLabel &other ) = delete;
        BBLabel& operator = ( const BBLabel &other ) = delete;
};


#endif // BB_LABEL
