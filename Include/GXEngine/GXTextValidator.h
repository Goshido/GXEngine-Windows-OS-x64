//version 1.0

#ifndef GX_TEXT_VALIDATOR
#define GX_TEXT_VALIDATOR


#include <GXCommon/GXTypes.h>


class GXTextValidator
{
	protected:
		GXWChar*	oldValidText;

	public:
		explicit GXTextValidator ( const GXWChar* defaultValidText );
		virtual ~GXTextValidator ();

		virtual GXBool Validate ( const GXWChar* text ) = 0;
};


#endif //GX_TEXT_VALIDATOR
