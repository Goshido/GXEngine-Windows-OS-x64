#ifndef EM_UI_OPEN_FILE
#define EM_UI_OPEN_FILE


#include "EMUIButton.h"
#include "EMUIDraggableArea.h"
#include "EMUIStaticText.h"
#include "EMUISeparator.h"
#include "EMUIFileListBox.h"
#include <GXCommon/GXFileSystem.h>


typedef GXVoid ( GXCALL* PFNEMONBROWSEFILEPROC ) ( const GXWChar* filePath );

class EMUIOpenFile : public EMUI
{
	friend GXVoid GXCALL EMUIOpenFile::OnButton ( GXFloat x, GXFloat y, eGXMouseButtonState state );
	friend GXVoid GXCALL EMUIOpenFile::OnItemSelected ( GXVoid* handler, GXUIListBox* listBox, const GXVoid* item );
	friend GXVoid GXCALL EMUIOpenFile::OnItemDoubleClicked ( GXVoid* handler, GXUIListBox* listBox, const GXVoid* item );
	friend GXVoid GXCALL EMUIOpenFile::OnResize ( GXUIDragableArea* area, GXFloat width, GXFloat height );

	private:
		EMUIDraggableArea*		mainPanel;
		EMUIButton*				okButton;
		EMUIButton*				cancelButton;
		EMUIStaticText*			filePathStaticText;
		EMUISeparator*			bottomSeparator;
		EMUISeparator*			topSeparator;
		EMUIFileListBox*		fileListBox;

		GXWChar*				rootDirectory;
		GXUInt					rootDirectoryPathOffset;
		GXDirectoryInfo			directoryInfo;

		PFNEMONBROWSEFILEPROC	OnBrowseFile;

	public:
		EMUIOpenFile ();
		~EMUIOpenFile () override;

		GXWidget* GetWidget () const override;

		GXVoid Browse ( PFNEMONBROWSEFILEPROC callback );

	private:
		GXVoid UpdateDirectory ( const GXWChar* folder );
		const GXWChar* GetRelativePath () const;

		static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state );
		static GXVoid GXCALL OnItemSelected ( GXVoid* handler, GXUIListBox* listBox, const GXVoid* item );
		static GXVoid GXCALL OnItemDoubleClicked ( GXVoid* handler, GXUIListBox* listBox, const GXVoid* item );
		static GXVoid GXCALL OnResize ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height );
};


#endif //EM_UI_OPEN_FILE
