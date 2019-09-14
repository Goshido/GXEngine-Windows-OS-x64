// version 1.4

#include <GXEngine/GXUIEditBoxFloatValidator.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>


enum eParserState : GXUByte
{
    LeadSign,
    IntegerPart,
    FractionalPart,
    ExponentSign,
    ExponentValue,
    InvalidFloatNumber
};

//---------------------------------------------------------------------------------------------------------------------

GXUIEditBoxFloatValidator::GXUIEditBoxFloatValidator ( const GXString &defaultValidText, GXUIEditBox& editBox, GXFloat minimumValue, GXFloat maximumValue ):
    GXTextValidator ( defaultValidText ),
    _editBox ( editBox ),
    _minimumValue ( minimumValue ),
    _maximumValue ( maximumValue )
{
    Validate ( _editBox.GetText () );
}

GXUIEditBoxFloatValidator::~GXUIEditBoxFloatValidator ()
{
    // NOTHING
}

GXBool GXUIEditBoxFloatValidator::Validate ( const GXString &text )
{
    if ( text.IsNull () )
    {
        _editBox.SetText ( _oldValidText );
        return GX_FALSE;
    }

    eParserState state = eParserState::LeadSign;
    GXUPointer symbols = GXWcslen ( text );
    GXUPointer i = 0u;

    while ( i < symbols )
    {
        const GXStringSymbol symbol = text[ i ];

        switch ( state )
        {
            case eParserState::LeadSign:
            {
                if ( symbol == '-' )
                {
                    state = eParserState::IntegerPart;
                    ++i;
                }
                else if ( isdigit ( static_cast<int> ( symbol.ToCodePoint () ) ) )
                {
                    state = eParserState::IntegerPart;
                }
                else
                {
                    state = eParserState::InvalidFloatNumber;
                }
            }
            break;

            case eParserState::IntegerPart:
            {
                if ( isdigit ( static_cast<int> ( symbol.ToCodePoint () ) ) )
                {
                    ++i;
                }
                else if ( symbol == '.' )
                {
                    state = eParserState::FractionalPart;
                    ++i;
                }
                else
                {
                    state = eParserState::InvalidFloatNumber;
                }
            }
            break;

            case eParserState::FractionalPart:
            {
                if ( isdigit ( static_cast<int> ( symbol.ToCodePoint () ) ) )
                {
                    ++i;
                }
                else if ( symbol == 'e' || symbol == 'E' )
                {
                    state = eParserState::ExponentSign;
                    ++i;
                }
                else
                {
                    state = eParserState::InvalidFloatNumber;
                }
            }
            break;

            case eParserState::ExponentSign:
            {
                if ( symbol == '+' || symbol == '-' )
                {
                    state = eParserState::ExponentValue;
                    ++i;
                }
                else
                {
                    state = eParserState::InvalidFloatNumber;
                }
            }
            break;

            case eParserState::ExponentValue:
            {
                if ( isdigit ( static_cast<int> ( symbol.ToCodePoint () ) ) )
                {
                    ++i;
                }
                else
                {
                    state = eParserState::InvalidFloatNumber;
                }
            }
            break;
                
            case eParserState::InvalidFloatNumber:
            default:
                _editBox.SetText ( _oldValidText );
            return GX_FALSE;
        }
    }

    GXFloat currentValue;
    swscanf_s ( text, L"%g", &currentValue );

    if ( currentValue < _minimumValue || currentValue > _maximumValue )
    {
        _editBox.SetText ( _oldValidText );
        return GX_FALSE;
    }

    _oldValidText = text;
    return GX_TRUE;
}
