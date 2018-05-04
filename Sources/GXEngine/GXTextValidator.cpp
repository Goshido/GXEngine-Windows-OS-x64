// version 1.1

#include <GXEngine/GXTextValidator.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


GXTextValidator::GXTextValidator ( const GXWChar* defaultValidText )
{
	if ( !defaultValidText )
		defaultValidText = nullptr;
	else
		GXWcsclone ( &oldValidText, defaultValidText );
}

GXTextValidator::~GXTextValidator ()
{
	GXSafeFree ( oldValidText );
}
