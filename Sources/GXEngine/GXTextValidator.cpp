// version 1.2

#include <GXEngine/GXTextValidator.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


GXTextValidator::GXTextValidator ( const GXWChar* defaultValidText )
{
    if ( defaultValidText )
    {
        GXWcsclone ( &_oldValidText, defaultValidText );
        return;
    }

    defaultValidText = nullptr;
}

GXTextValidator::~GXTextValidator ()
{
    GXSafeFree ( _oldValidText );
}
