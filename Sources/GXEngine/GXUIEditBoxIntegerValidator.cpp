// version 1.1

#include <GXEngine/GXUIEditBoxIntegerValidator.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


GXUIEditBoxIntegerValidator::GXUIEditBoxIntegerValidator ( const GXWChar* defaultValidText, GXUIEditBox& editBox, GXBigInt minimumValue, GXBigInt maximumValue ):
	GXTextValidator ( defaultValidText ),
	editBox ( editBox ),
	minimumValue ( minimumValue ),
	maximumValue ( maximumValue )
{
	Validate ( this->editBox.GetText () );
}

GXUIEditBoxIntegerValidator::~GXUIEditBoxIntegerValidator ()
{
	GXSafeFree ( oldValidText );
}

GXBool GXUIEditBoxIntegerValidator::Validate ( const GXWChar* text )
{
	if ( !text )
	{
		editBox.SetText ( oldValidText );
		return GX_FALSE;
	}

	GXUInt i = 0u;

	if ( text[ i ] == L'-' )
		i++;

	GXUInt symbols = GXWcslen ( text );

	for ( ; i < symbols; i++ )
	{
		if ( !isdigit ( static_cast<int> ( text[ i ] ) ) )
		{
			editBox.SetText ( oldValidText );
			return GX_FALSE;
		}
	}

	GXBigInt currentValue;
	swscanf_s ( text, L"%lli", &currentValue );

	if ( currentValue < minimumValue || currentValue > maximumValue )
	{
		editBox.SetText ( oldValidText );
		return GX_FALSE;
	}

	GXSafeFree ( oldValidText );
	GXWcsclone ( &oldValidText, text );

	return GX_TRUE;
}
