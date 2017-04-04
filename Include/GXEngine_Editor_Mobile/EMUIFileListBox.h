#ifndef EMUI_FILE_LIST_BOX
#define EMUI_FILE_LIST_BOX


#include "EMUI.h"
#include <GXEngine/GXUIListBox.h>


enum class eEMUIFileListBoxItemType : GXUByte
{
	Folder,
	File
};

struct EMUIFileListBoxItem
{
	eEMUIFileListBoxItemType	type;
	GXWChar*					name;

	explicit EMUIFileListBoxItem ( eEMUIFileListBoxItemType type, const GXWChar* name );
	~EMUIFileListBoxItem ();
};

class EMUIFileListBox : public EMUI
{
	private:
		GXUIListBox*	widget;

	public:
		explicit EMUIFileListBox ( EMUI* parent );
		~EMUIFileListBox () override;

		GXWidget* GetWidget () const override;

		GXVoid Resize ( GXFloat leftBottomX, GXFloat leftBottomY, GXFloat width, GXFloat height );
		GXVoid AddFolder ( const GXWChar* name );
		GXVoid AddFile ( const GXWChar* name );
		GXVoid AddItems ( const EMUIFileListBoxItem* itemArray, GXUInt items );
		GXVoid Clear ();
		GXVoid Redraw ();
		const GXVoid* GetSelectedItem () const;
		GXVoid SetOnItemSelectedCallback ( GXVoid* handler, PFNGXUILISTBOXONITEMSELECTEDPROC callback );
		GXVoid SetOnItemDoubleClickedCallbak ( GXVoid* handler, PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC callback );

	private:
		static GXVoid GXCALL ItemDestructor ( GXVoid* itemData );
};


#endif //EMUI_FILE_LIST_BOX
