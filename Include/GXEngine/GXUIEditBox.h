//version 1.0

#ifndef GX_UI_EDIT_BOX
#define GX_UI_EDIT_BOX


#include "GXUIStaticText.h"
#include "GXFont.h"


class GXUIEditBox : public GXWidget
{
	friend GXVoid GXCALL GXUIEditBox::OnEnd ( GXVoid* handler );
	friend GXVoid GXCALL GXUIEditBox::OnHome ( GXVoid* handler );
	friend GXVoid GXCALL GXUIEditBox::OnDel ( GXVoid* handler );
	friend GXVoid GXCALL GXUIEditBox::OnBackspace ( GXVoid* handler );
	friend GXVoid GXCALL GXUIEditBox::OnLeftArrow ( GXVoid* handler );
	friend GXVoid GXCALL GXUIEditBox::OnRightArrow ( GXVoid* handler );
	friend GXVoid GXCALL GXUIEditBox::OnType ( GXWChar symbol, GXVoid* handler );

	private:
		GXWChar*			text;
		GXWChar*			workingBuffer;
		GXUInt				textSymbols;
		GXUInt				maxSymbols;

		GXFloat				textLeftOffset;
		GXFloat				textRightOffset;

		GXFont				font;
		GXInt				cursor;			//index before symbol
		GXInt				selection;		//index before symbol
		eGXUITextAlignment	alignment;

		HCURSOR				editCursor;
		HCURSOR				arrowCursor;
		const HCURSOR*		currentCursor;

	public:
		GXUIEditBox ( GXWidget* parent );
		~GXUIEditBox () override;

		GXVoid OnMessage ( GXUInt message, const GXVoid* data ) override;

		GXFloat GetCursorOffset () const;
		GXFloat GetSelectionBeginOffset () const;
		GXFloat GetSelectionEndOffset () const;

		GXVoid SetTextLeftOffset ( GXFloat offset );
		GXFloat GetTextLeftOffset () const;

		GXVoid SetTextRightOffset ( GXFloat offset );
		GXFloat GetTextRightOffset () const;

		GXVoid SetText ( const GXWChar* text );
		const GXWChar* GetText () const;

		GXVoid SetAlignment ( eGXUITextAlignment alignment );
		eGXUITextAlignment GetAlignment () const;

		GXVoid SetFont ( const GXWChar* fontFile, GXUShort fontSize );
		GXFont* GetFont ();

		GXBool IsActive ();

	private:
		GXInt GetSelectionPosition ( const GXVec2 &mousePosition ) const;
		GXFloat GetSelectionOffset ( GXUInt symbolIndex ) const;

		GXVoid LockInput ();
		GXVoid ReleaseInput ();

		GXVoid UpdateCursor ( GXInt newCursor );

		GXVoid CopyText ();
		GXVoid PasteText ( const GXWChar* text );
		GXBool DeleteText ();

		static GXVoid GXCALL OnEnd ( GXVoid* handler );
		static GXVoid GXCALL OnHome ( GXVoid* handler );
		static GXVoid GXCALL OnDel ( GXVoid* handler );
		static GXVoid GXCALL OnBackspace ( GXVoid* handler );
		static GXVoid GXCALL OnLeftArrow ( GXVoid* handler );
		static GXVoid GXCALL OnRightArrow ( GXVoid* handler );
		static GXVoid GXCALL OnType ( GXWChar symbol, GXVoid* handler );
};


#endif //GX_UI_EDIT_BOX
