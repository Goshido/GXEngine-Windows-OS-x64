#ifndef EM_UI_EDIT_BOX
#define EM_UI_EDIT_BOX


#include "EMUI.h"
#include <GXEngine/GXUIEditBox.h>


class EMUIEditBox : public EMUI
{
	private:
		GXUIEditBox*	widget;

	public:
		explicit EMUIEditBox ( EMUI* parent );
		~EMUIEditBox () override;

		GXWidget* GetWidget () const override;

		GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );

		GXVoid SetText ( const GXWChar* text );
		const GXWChar* GetText () const;

		GXVoid SetAlignment ( eGXUITextAlignment alignment );
		eGXUITextAlignment GetAlignment () const;

		GXVoid SetFont ( const GXWChar* fontFile, GXUShort fontSize );
		GXFont* GetFont () const;

		GXVoid SetValidator ( GXTextValidator& validator );
		GXTextValidator* GetValidator () const;

		GXVoid SetOnFinishEditingCallback ( GXVoid* handler, PFNGXUIEDITBOXONFINISHEDITINGPROC callback );

	private:
		EMUIEditBox () = delete;
		EMUIEditBox ( const EMUIEditBox &other ) = delete;
		EMUIEditBox& operator = ( const EMUIEditBox &other ) = delete;
};


#endif // EM_UI_EDIT_BOX
