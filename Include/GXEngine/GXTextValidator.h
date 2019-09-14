// version 1.3

#ifndef GX_TEXT_VALIDATOR
#define GX_TEXT_VALIDATOR


#include <GXCommon/GXStrings.h>


class GXTextValidator
{
    protected:
        GXString    _oldValidText;

    public:
        explicit GXTextValidator ( const GXString &defaultValidText );
        virtual ~GXTextValidator ();

        virtual GXBool Validate ( const GXString &text ) = 0;

    private:
        GXTextValidator () = delete;
        GXTextValidator ( const GXTextValidator &other ) = delete;
        GXTextValidator& operator = ( const GXTextValidator &other ) = delete;
};


#endif // GX_TEXT_VALIDATOR
