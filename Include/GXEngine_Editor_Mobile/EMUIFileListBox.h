#ifndef EMUI_FILE_LIST_BOX
#define EMUI_FILE_LIST_BOX


#include "EMUI.h"
#include <GXEngine/GXUIListBox.h>


enum eEMUIFileListBoxItemType : GXUByte
{
	EM_UI_FILE_BOX_ITEM_TYPE_FOLDER,
	EM_UI_FILE_BOX_ITEM_TYPE_FILE
};

struct EMUIFileListBoxItem
{
	eEMUIFileListBoxItemType	type;
	GXWChar*					name;

	EMUIFileListBoxItem ( eEMUIFileListBoxItemType type, const GXWChar* name );
	~EMUIFileListBoxItem ();
};

class EMUIFileListBox : public EMUI
{
	private:
		GXUIListBox*	widget;

	public:
		EMUIFileListBox ( EMUI* parent );
		virtual ~EMUIFileListBox ();

		virtual GXWidget* GetWidget ();
		virtual GXVoid OnDrawMask ();

		GXVoid Resize ( GXFloat leftBottomX, GXFloat leftBottomY, GXFloat width, GXFloat height );
		GXVoid AddFolder ( const GXWChar* name );
		GXVoid AddFile ( const GXWChar* name );
		GXVoid AddItems ( const EMUIFileListBoxItem* itemArray, GXUInt items );
		GXVoid Clear ();
		GXVoid Redraw ();
		const GXVoid* GetSelectedItem ();
		GXVoid SetOnItemSelectedCallback ( GXVoid* handler, PFNGXUILISTBOXONITEMSELECTEDPROC callback );
		GXVoid SetOnItemDoubleClickedCallbak ( GXVoid* handler, PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC callback );

	private:
		static GXVoid GXCALL ItemDestructor ( GXVoid* itemData );
};


#endif //EMUI_FILE_LIST_BOX
