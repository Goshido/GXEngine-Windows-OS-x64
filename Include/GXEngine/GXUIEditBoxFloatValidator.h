//version 1.0

#ifndef GX_UI_EDIT_BOX_FLOAT_VALIDATOR
#define GX_UI_EDIT_BOX_FLOAT_VALIDATOR


#include "GXTextValidator.h"
#include "GXUIEditBox.h"


class GXUIEditBoxFloatValidator : public GXTextValidator
{
	private:
		GXUIEditBox&	widget;

		GXFloat			minimumValue;
		GXFloat			maximumValue;

	public:
		explicit GXUIEditBoxFloatValidator ( const GXWChar* defaultValidText, GXUIEditBox& editBox, GXFloat minimumValue, GXFloat maximumValue );
		~GXUIEditBoxFloatValidator () override;

		GXBool Validate ( const GXWChar* text ) override;
};


#endif //GX_UI_EDIT_BOX_FLOAT_VALIDATOR
