// version 1.4

#ifndef GX_UI_EDIT_BOX_FLOAT_VALIDATOR
#define GX_UI_EDIT_BOX_FLOAT_VALIDATOR


#include "GXTextValidator.h"
#include "GXUIEditBox.h"


class GXUIEditBoxFloatValidator final : public GXTextValidator
{
    private:
        GXUIEditBox&    _editBox;

        GXFloat         _minimumValue;
        GXFloat         _maximumValue;

    public:
        explicit GXUIEditBoxFloatValidator ( const GXString &defaultValidText, GXUIEditBox &editBox, GXFloat minimumValue, GXFloat maximumValue );
        ~GXUIEditBoxFloatValidator () override;

        GXBool Validate ( const GXString &text ) override;

    private:
        GXUIEditBoxFloatValidator () = delete;
        GXUIEditBoxFloatValidator ( const GXUIEditBoxFloatValidator &other ) = delete;
        GXUIEditBoxFloatValidator& operator = ( const GXUIEditBoxFloatValidator &other ) = delete;
};


#endif // GX_UI_EDIT_BOX_FLOAT_VALIDATOR
