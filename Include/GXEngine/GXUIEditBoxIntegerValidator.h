// version 1.5

#ifndef GX_UI_EDIT_BOX_INTEGER_VALIDATOR
#define GX_UI_EDIT_BOX_INTEGER_VALIDATOR


#include "GXTextValidator.h"
#include "GXUIEditBox.h"


class GXUIEditBoxIntegerValidator final : public GXTextValidator
{
    private:
        GXUIEditBox&    _editBox;

        GXBigInt        _minimumValue;
        GXBigInt        _maximumValue;

    public:
        explicit GXUIEditBoxIntegerValidator ( const GXString &defaultValidText, GXUIEditBox& editBox, GXBigInt minimumValue, GXBigInt maximumValue );
        ~GXUIEditBoxIntegerValidator () override;

        GXBool Validate ( const GXString &text ) override;

    private:
        GXUIEditBoxIntegerValidator () = delete;
        GXUIEditBoxIntegerValidator ( const GXUIEditBoxIntegerValidator &other ) = delete;
        GXUIEditBoxIntegerValidator& operator = ( const GXUIEditBoxIntegerValidator &other ) = delete;
};


#endif // GX_UI_EDIT_BOX_INTEGER_VALIDATOR
