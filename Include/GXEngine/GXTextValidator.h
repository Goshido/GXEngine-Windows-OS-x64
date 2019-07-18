// version 1.2

#ifndef GX_TEXT_VALIDATOR
#define GX_TEXT_VALIDATOR


#include <GXCommon/GXTypes.h>


class GXTextValidator
{
    protected:
        GXWChar*    _oldValidText;

    public:
        explicit GXTextValidator ( const GXWChar* defaultValidText );
        virtual ~GXTextValidator ();

        virtual GXBool Validate ( const GXWChar* text ) = 0;

    private:
        GXTextValidator () = delete;
        GXTextValidator ( const GXTextValidator &other ) = delete;
        GXTextValidator& operator = ( const GXTextValidator &other ) = delete;
};


#endif // GX_TEXT_VALIDATOR
