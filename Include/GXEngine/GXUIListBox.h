//version 1.0

#ifndef GX_UI_LIST_BOX
#define GX_UI_LIST_BOX


#include "GXWidget.h"

class GXUIListBox;
typedef GXVoid ( GXCALL* PFNGXUILISTBOXITEMDESTRUCTORPROC ) ( GXVoid* itemData );
typedef GXVoid ( GXCALL* PFNGXUILISTBOXONITEMSELECTEDPROC ) ( GXVoid* handler, GXUIListBox* listBox, const GXVoid* item );
typedef GXVoid ( GXCALL* PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC ) ( GXVoid* handler, GXUIListBox* listBox, const GXVoid* item );

struct GXUIListBoxItem
{
	GXUIListBoxItem*		next;
	GXUIListBoxItem*		prev;

	GXBool					isSelected;
	GXBool					isHighlighted;

	GXVoid*					data;
};

class GXUIListBox : public GXWidget
{
	private:
		GXUIListBoxItem*						itemHead;
		GXUIListBoxItem*						itemTail;
		GXFloat									itemHeight;
		GXUInt									totalItems;
		PFNGXUILISTBOXITEMDESTRUCTORPROC		DestroyItem;
		PFNGXUILISTBOXONITEMSELECTEDPROC		OnItemSelected;
		PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC	OnItemDoubleClicked;
		GXVoid*									itemSelectedHandler;
		GXVoid*									itemDoubleClickedHandler;

		GXFloat									viewportOffset;
		GXFloat									viewportSize;

	public:
		GXUIListBox ( GXWidget* parent, PFNGXUILISTBOXITEMDESTRUCTORPROC itemDestructor );
		virtual ~GXUIListBox ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid AddItem ( GXVoid* itemData );
		GXVoid AddItems ( GXVoid** itemData, GXUInt items );
		GXVoid RemoveItem ( GXUInt itemIndex );
		GXVoid RemoveAllItems ();
		GXUIListBoxItem* GetItems ();
		GXVoid* GetSelectedItem ();
		GXUInt GetTotalItems ();
		GXBool IsItemVisible ( GXUInt itemIndex );
		GXFloat GetItemLocalOffsetY ( GXUInt itemIndex );	//From widget bottom edge

		GXVoid Redraw ();

		GXVoid SetOnItemSelectedCallback ( GXVoid* handler, PFNGXUILISTBOXONITEMSELECTEDPROC callback );
		GXVoid SetOnItemDoubleClickedCallback ( GXVoid* handler, PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC callback );

		GXFloat GetViewportOffset ();
		GXFloat GetViewportSize ();
		GXVoid SetViewportOffset ( GXFloat offset );
		GXVoid SetViewportOffsetImmediately ( GXFloat offset );

		GXVoid SetItemHeight ( GXFloat height );
		GXFloat GetItemHeight ();

	private:
		GXUIListBoxItem* FindItem ( const GXVec2 &mousePosition );
		GXUIListBoxItem* GetHighlightedItem ();
		GXBool ResetHighlight ( const GXVec2 &mousePosition );	//Return true if need render update
		GXBool IsAbleToScroll ();
		GXFloat GetTotalHeight ();
};


#endif //GX_UI_LIST_BOX
