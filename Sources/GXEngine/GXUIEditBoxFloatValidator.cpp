// version 1.1

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

GXUIEditBoxFloatValidator::GXUIEditBoxFloatValidator ( const GXWChar* defaultValidText, GXUIEditBox& editBox, GXFloat minimumValue, GXFloat maximumValue ):
	GXTextValidator ( defaultValidText ),
	editBox ( editBox ),
	minimumValue ( minimumValue ),
	maximumValue ( maximumValue )
{
	Validate ( this->editBox.GetText () );
}

GXUIEditBoxFloatValidator::~GXUIEditBoxFloatValidator ()
{
	GXSafeFree ( oldValidText );
}

GXBool GXUIEditBoxFloatValidator::Validate ( const GXWChar* text )
{
	if ( !text )
	{
		editBox.SetText ( oldValidText );
		return GX_FALSE;
	}

	eParserState state = eParserState::LeadSign;
	GXUInt symbols = GXWcslen ( text );
	GXUInt i = 0u;

	while ( i < symbols )
	{
		switch ( state )
		{
			case eParserState::LeadSign:
			{
				if ( text[ i ] == L'-' )
				{
					state = eParserState::IntegerPart;
					i++;
				}
				else if ( isdigit ( static_cast<int> ( text[ i ] ) ) )
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
				if ( isdigit ( static_cast<int> ( text[ i ] ) ) )
				{
					i++;
				}
				else if ( text[ i ] == L'.' )
				{
					state = eParserState::FractionalPart;
					i++;
				}
				else
				{
					state = eParserState::InvalidFloatNumber;
				}
			}
			break;

			case eParserState::FractionalPart:
			{
				if ( isdigit ( static_cast<int> ( text[ i ] ) ) )
				{
					i++;
				}
				else if ( text[ i ] == L'e' || text[ i ] == L'E' )
				{
					state = eParserState::ExponentSign;
					i++;
				}
				else
				{
					state = eParserState::InvalidFloatNumber;
				}
			}
			break;

			case eParserState::ExponentSign:
			{
				if ( text[ i ] == L'+' || text[ i ] == L'-' )
				{
					state = eParserState::ExponentValue;
					i++;
				}
				else
				{
					state = eParserState::InvalidFloatNumber;
				}
			}
			break;

			case eParserState::ExponentValue:
			{
				if ( isdigit ( static_cast<int> ( text[ i ] ) ) )
				{
					i++;
				}
				else
				{
					state = eParserState::InvalidFloatNumber;
				}
			}
			break;
				
			case eParserState::InvalidFloatNumber:
			default:
				editBox.SetText ( oldValidText );
			return GX_FALSE;
		}
	}

	GXFloat currentValue;
	swscanf_s ( text, L"%g", &currentValue );

	if ( currentValue < minimumValue || currentValue > maximumValue )
	{
		editBox.SetText ( oldValidText );
		return GX_FALSE;
	}

	GXSafeFree ( oldValidText );
	GXWcsclone ( &oldValidText, text );

	return GX_TRUE;
}
