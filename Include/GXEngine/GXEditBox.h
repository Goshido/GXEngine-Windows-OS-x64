//version 1.2

#ifndef GX_EDIT_BOX
#define GX_EDIT_BOX


#include <GXEngine/GXHudSurface.h>


typedef GXVoid ( GXCALL* PFNGXONTEXTCHANGEDPROC ) ();


enum eGXTextAlignment
{
	GX_CENTER,
	GX_LEFT,
	GX_RIGHT
};


struct GXEditBoxStyle
{
	GXWChar*				fontFile;
	GXUInt					fontSize;
	GXVec4					fontColor;
	GXBool					kerning;
	GXVec4					selectColor;
	eGXTextAlignment		alignment;
	GXBool					multiline;
	PFNGXONTEXTCHANGEDPROC	OnTextChanged;

	GXEditBoxStyle ();
};


class GXEditBox
{
	private:
		GXHudSurface*	surface;
		GLuint			texture;

		GXUShort		width;
		GXUShort		height;
		GXVec3			location;
		GXMat4			rotation;

		GXWChar*		text;
		GXUShort		numSymbols;
		GXUShort		totalSymbols;

		GXUShort		carriage;
		GXUShort		selection;

		FT_Face			font;
		GXEditBoxStyle	style;
		GXBool			isStyleChanged;

		GXFloat			timer;
		
		GXBool			isDelete;
		GXBool			isUpdate;
		GXBool			isEdit;
		
	public:
		GXEditBox ( GXUShort width, GXUShort height );
		GXVoid Delete ();

		GXVoid CaptureInput ();
		GXVoid ReleaseInput ();

		GXVoid SetTextStyle ( const GXEditBoxStyle &style );
		const GXWChar* GetText ();

		GXVoid AddText ( const GXWChar* text );
		GXVoid MarkAll ();
		GXVoid ReplaceText ( const GXWChar* text );
		
		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetRotation ( const GXMat4 &rotation );

		GXVoid Draw ();
		GXVoid Update ( GXFloat delta );

	private:
		~GXEditBox ();
		GXVoid UpdateContent ();

		static GXVoid GXCALL AddSymbol ( GXWChar symbol );

		static GXVoid GXCALL Backspace ();
		static GXVoid GXCALL Del ();
		static GXVoid GXCALL MoveCarriageForward ();
		static GXVoid GXCALL MoveCarriageBack ();

		static GXVoid GXCALL Home ();
		static GXVoid GXCALL End ();

		static GXVoid GXCALL Copy ();
		static GXVoid GXCALL Paste ();
		static GXVoid GXCALL Cut ();

		static GXVoid GXCALL SelectAll ();
		static GXVoid GXCALL DeleteSelected ();
		static GXVoid GXCALL TextChanged ();
};


#endif //GX_EDIT_BOX