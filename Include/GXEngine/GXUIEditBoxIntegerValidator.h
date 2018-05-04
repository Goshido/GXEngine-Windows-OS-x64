// version 1.1

#ifndef GX_UI_EDIT_BOX_INTEGER_VALIDATOR
#define GX_UI_EDIT_BOX_INTEGER_VALIDATOR


#include "GXTextValidator.h"
#include "GXUIEditBox.h"


class GXUIEditBoxIntegerValidator : public GXTextValidator
{
	private:
		GXUIEditBox&	editBox;

		GXBigInt		minimumValue;
		GXBigInt		maximumValue;

	public:
		explicit GXUIEditBoxIntegerValidator ( const GXWChar* defaultValidText, GXUIEditBox& editBox, GXBigInt minimumValue, GXBigInt maximumValue );
		~GXUIEditBoxIntegerValidator () override;

		GXBool Validate ( const GXWChar* text ) override;

	private:
		GXUIEditBoxIntegerValidator () = delete;
		GXUIEditBoxIntegerValidator ( const GXUIEditBoxIntegerValidator &other ) = delete;
		GXUIEditBoxIntegerValidator& operator = ( const GXUIEditBoxIntegerValidator &other ) = delete;
};


#endif // GX_UI_EDIT_BOX_INTEGER_VALIDATOR
