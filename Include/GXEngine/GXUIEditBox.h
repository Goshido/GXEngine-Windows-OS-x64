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

		GXFloat				textLeftOffset;
		GXFloat				textRightOffset;

		GXFont*				font;
		GXInt				cursor;			//index before symbol
		GXInt				selection;		//index before symbol
		eGXUITextAlignment	alignment;

		HCURSOR				editCursor;
		HCURSOR				arrowCursor;
		const HCURSOR*		currentCursor;

	public:
		GXUIEditBox ( GXWidget* parent );
		virtual ~GXUIEditBox ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXFloat GetCursorOffset ();
		GXFloat GetSelectionBeginOffset ();
		GXFloat GetSelectionEndOffset ();

		GXVoid SetTextLeftOffset ( GXFloat offset );
		GXFloat GetTextLeftOffset ();

		GXVoid SetTextRightOffset ( GXFloat offset );
		GXFloat GetTextRightOffset ();

		GXVoid SetText ( const GXWChar* text );
		const GXWChar* GetText ();

		GXVoid SetAlignment ( eGXUITextAlignment alignment );
		eGXUITextAlignment GetAlignment ();

		GXVoid SetFont ( const GXWChar* fontFile, GXUShort fontSize );
		GXFont* GetFont ();

		GXBool IsActive ();

	private:
		GXInt GetSelectionPosition ( const GXVec2 &mousePosition );
		GXFloat GetSelectionOffset ( GXUInt symbolIndex );

		GXVoid LockInput ();
		GXVoid ReleaseInput ();

		GXVoid UpdateCursor ( GXInt newCursor );

		GXVoid CopyText ();

		static GXVoid GXCALL OnEnd ( GXVoid* handler );
		static GXVoid GXCALL OnHome ( GXVoid* handler );
		static GXVoid GXCALL OnDel ( GXVoid* handler );
		static GXVoid GXCALL OnBackspace ( GXVoid* handler );
		static GXVoid GXCALL OnLeftArrow ( GXVoid* handler );
		static GXVoid GXCALL OnRightArrow ( GXVoid* handler );
		static GXVoid GXCALL OnType ( GXWChar symbol, GXVoid* handler );
};


#endif //GX_UI_EDIT_BOX
