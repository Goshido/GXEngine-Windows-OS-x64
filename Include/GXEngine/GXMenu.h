//version 1.4

#ifndef GX_MENU
#define	GX_MENU


#include "GXHudSurface.h"
#include "GXFont.h"


typedef GXVoid ( GXCALL* PFNGXONSELECTPROC ) ();
typedef GXVoid ( GXCALL* PFNGXONFOCUSPROC ) ( GXUInt item );


struct GXMenuStyle
{
	GXBool			showSelector;
	GXBool			showBackground;

	GXVec4			selectColor;
	GXVec4			disableColor;
	GXVec4			inactiveColor;

	GXWChar*		fontFile;
	GXUInt			fontSize;
	GXUShort		stripe;
	GXFloat			selectorOffset;
	GXUShort		fontOffset;
	GXUShort		padding;

	GXMenuStyle ();
};


class GXMenu
{
	private:
		GXUShort				width;
		GXUShort				height;

		GXUInt					selectorWidth;

		FT_Face					font;
		GXMenuStyle				style;

		GXHudSurface*			surface;
		GXMaterialInfo			matInfo;

		GXUInt					totalItems;
		GXWChar**				itemNames;
		GXUChar*				itemAccess;
		PFNGXONSELECTPROC*		itemCallbacks;

		GXUInt					selected;
		GXBool					isEnable;

		GXVec2					selectorFrom;
		GXVec2					selectorNow;
		GXVec2					selectorTo;
		GXFloat					travelTimer;

		GXBool					isInvalid;
		GXBool					isDelete;

		GXVec3					location;
		GXMat4					rotation;

		PFNGXONFOCUSPROC		OnFocus;

	public:
		GXMenu ( GXUShort width, GXUShort height, PFNGXONFOCUSPROC onFocusFunc = 0 );
		GXVoid Delete ();

		GXVoid Reset ();

		GXVoid EnableItem ( GXUInt item );
		GXVoid EnableMenu ();

		GXVoid DisableItem ( GXUInt item );
		GXVoid DisableMenu ();

		GXVoid OnSelect ();
		GXVoid OnFocusPrevious ();
		GXVoid OnFocusNext ();
		GXVoid OnSetCursorPosition ( GXUShort x, GXUShort y );

		GXVoid AddItem ( const GXWChar* name, PFNGXONSELECTPROC callback );
		GXVoid AddGap ();

		GXVoid Update ( GXFloat delta );
		GXVoid Draw ();

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetRotation ( const GXMat4 &rotation );

		GXVoid SetStyle ( const GXMenuStyle &style );
		GXVoid SetFocus ( GXUInt item );
		GXVoid SetVoidFocus ();

		GXUInt GetFocus ();
		GXUInt GetTotalItems ();
		const GXWChar* GetItemText ( GXUInt item );

		GXBool IsFocused ();

	private:
		~GXMenu ();

		GXVoid UpdateSurface ();
};


#endif //GX_MENU