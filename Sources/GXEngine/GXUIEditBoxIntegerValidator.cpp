// version 1.5

#include <GXEngine/GXUIEditBoxIntegerValidator.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


GXUIEditBoxIntegerValidator::GXUIEditBoxIntegerValidator ( const GXString &defaultValidText, GXUIEditBox &editBox, GXBigInt minimumValue, GXBigInt maximumValue ):
    GXTextValidator ( defaultValidText ),
    _editBox ( editBox ),
    _minimumValue ( minimumValue ),
    _maximumValue ( maximumValue )
{
    Validate ( _editBox.GetText () );
}

GXUIEditBoxIntegerValidator::~GXUIEditBoxIntegerValidator ()
{
    // NOTHING
}

GXBool GXUIEditBoxIntegerValidator::Validate ( const GXString &text )
{
    if ( text.IsNull () || text.IsEmpty () )
    {
        _editBox.SetText ( _oldValidText );
        return GX_FALSE;
    }

    GXUPointer i = 0u;

    if ( text[ i ] == '-' )
        ++i;

    const GXUPointer symbols = text.GetSymbolCount ();

    for ( ; i < symbols; ++i )
    {
        const GXStringSymbol symbol = text[ i ];

        if ( isdigit ( static_cast<int> ( symbol.ToCodePoint () ) ) ) continue;

        _editBox.SetText ( _oldValidText );
        return GX_FALSE;
    }

    GXBigInt currentValue;
    sscanf_s ( static_cast<const GXChar*> ( text ), "%lli", &currentValue );

    if ( currentValue < _minimumValue || currentValue > _maximumValue )
    {
        _editBox.SetText ( _oldValidText );
        return GX_FALSE;
    }

    _oldValidText = text;
    return GX_TRUE;
}
