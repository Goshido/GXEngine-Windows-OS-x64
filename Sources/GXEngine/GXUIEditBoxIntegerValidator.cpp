//version 1.0

#include <GXEngine/GXUIEditBoxIntegerValidator.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


GXUIEditBoxIntegerValidator::GXUIEditBoxIntegerValidator ( const GXWChar* defaultValidText, GXUIEditBox& editBox, GXBigInt minimumValue, GXBigInt maximumValue ) :
GXTextValidator ( defaultValidText ), widget ( editBox )
{
	this->minimumValue = minimumValue;
	this->maximumValue = maximumValue;

	Validate ( widget.GetText () );
}

GXUIEditBoxIntegerValidator::~GXUIEditBoxIntegerValidator ()
{
	GXSafeFree ( oldValidText );
}

GXBool GXUIEditBoxIntegerValidator::Validate ( const GXWChar* text )
{
	if ( !text )
	{
		widget.SetText ( oldValidText );
		return GX_FALSE;
	}

	GXUInt i = 0;
	if ( text[ i ] == L'-' )
		i++;

	GXUInt symbols = GXWcslen ( text );
	for ( ; i < symbols; i++ )
	{
		if ( !isdigit ( (int)text[ i ] ) )
		{
			widget.SetText ( oldValidText );
			return GX_FALSE;
		}
	}

	GXBigInt currentValue;
	swscanf_s ( text, L"%lli", &currentValue );

	if ( currentValue < minimumValue || currentValue > maximumValue )
	{
		widget.SetText ( oldValidText );
		return GX_FALSE;
	}

	GXSafeFree ( oldValidText );
	GXWcsclone ( &oldValidText, text );

	return GX_TRUE;
}
