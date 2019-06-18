// version 1.4

#include <GXEngine/GXUIEditBoxIntegerValidator.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


GXUIEditBoxIntegerValidator::GXUIEditBoxIntegerValidator ( const GXWChar* defaultValidText, GXUIEditBox& editBox, GXBigInt minimumValue, GXBigInt maximumValue ):
    GXTextValidator ( defaultValidText ),
    _editBox ( editBox ),
    _minimumValue ( minimumValue ),
    _maximumValue ( maximumValue )
{
    Validate ( this->_editBox.GetText () );
}

GXUIEditBoxIntegerValidator::~GXUIEditBoxIntegerValidator ()
{
    GXSafeFree ( _oldValidText );
}

GXBool GXUIEditBoxIntegerValidator::Validate ( const GXWChar* text )
{
    if ( !text )
    {
        _editBox.SetText ( _oldValidText );
        return GX_FALSE;
    }

    GXUPointer i = 0u;

    if ( text[ i ] == L'-' )
        ++i;

    GXUPointer symbols = GXWcslen ( text );

    for ( ; i < symbols; ++i )
    {
        if ( !isdigit ( static_cast<int> ( text[ i ] ) ) )
        {
            _editBox.SetText ( _oldValidText );
            return GX_FALSE;
        }
    }

    GXBigInt currentValue;
    swscanf_s ( text, L"%lli", &currentValue );

    if ( currentValue < _minimumValue || currentValue > _maximumValue )
    {
        _editBox.SetText ( _oldValidText );
        return GX_FALSE;
    }

    GXSafeFree ( _oldValidText );
    GXWcsclone ( &_oldValidText, text );

    return GX_TRUE;
}
