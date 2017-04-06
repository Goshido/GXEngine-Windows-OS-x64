#include <GXEngine_Editor_Mobile/EMUIOpenFile.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXEngine/GXLocale.h>


#define EM_PANEL_LEFT_BOTTOM_X			7.6f
#define EM_PANEL_LEFT_BOTTOM_Y			5.8f
#define EM_PANEL_WIDTH					13.4f
#define EM_PANEL_HEIGHT					9.5f
#define EM_PANEL_HEADER_HEIGHT			0.5f

#define EM_BTN_OK_LEFT_BOTTOM_X			2.0f
#define EM_BTN_OK_TEXT					L"OpenFile->OK"
#define EM_BTN_CANCEL_LEFT_BOTTOM_X		3.9f
#define EM_BTN_CANCEL_TEXT				L"OpenFile->Cancel"
#define EM_BTN_BOTTOM_Y					0.15f
#define EM_BTN_WIDTH					1.7f
#define EM_BTN_HEIGHT					0.55f

#define EM_STT_FILE_PATH_LEFT_BOTTOM_X	0.2f
#define EM_STT_FILE_PATH_LEFT_BOTTOM_Y	1.0f
#define EM_STT_FILE_PATH_HEIGHT			0.3f

#define EM_SEP_BOTTOM_OFFSET_Y			0.9f
#define EM_SEP_TOP_OFFSET_Y				1.2f
#define EM_SEP_HEIGHT					0.06f
#define EM_SEP_OFFSET_X					0.2f

#define EM_LST_OFFSET_LEFT				0.2f
#define EM_LST_OFFSET_RIGHT				0.7f
#define EM_LST_OFFSET_BOTTOM			1.1f
#define EM_LST_OFFSET_TOP				1.3f

#define EM_PATH_BUFFER_SIZE				4096


EMUIOpenFile::EMUIOpenFile ():
EMUI ( nullptr )
{
	mainPanel = new EMUIDraggableArea ( nullptr );
	mainPanel->SetHeaderHeight ( EM_PANEL_HEADER_HEIGHT * gx_ui_Scale );
	mainPanel->SetOnResizeCallback ( this, &EMUIOpenFile::OnResize );

	okButton = new EMUIButton ( mainPanel );
	cancelButton = new EMUIButton ( mainPanel );
	filePathStaticText = new EMUIStaticText ( mainPanel );
	bottomSeparator = new EMUISeparator ( mainPanel );
	topSeparator = new EMUISeparator ( mainPanel );
	fileListBox = new EMUIFileListBox ( mainPanel );

	GXLocale* locale = GXLocale::GetInstance ();

	okButton->SetCaption ( locale->GetString ( EM_BTN_OK_TEXT ) );
	okButton->SetOnLeftMouseButtonCallback ( this, &EMUIOpenFile::OnButton );

	cancelButton->SetCaption ( locale->GetString ( EM_BTN_CANCEL_TEXT ) );
	cancelButton->SetOnLeftMouseButtonCallback ( this, &EMUIOpenFile::OnButton );

	filePathStaticText->SetText ( 0 );
	filePathStaticText->SetAlingment ( GX_UI_TEXT_ALIGNMENT_LEFT );
	
	fileListBox->SetOnItemSelectedCallback ( this, &EMUIOpenFile::OnItemSelected );
	fileListBox->SetOnItemDoubleClickedCallbak ( this, &EMUIOpenFile::OnItemDoubleClicked );

	GXGetCurrentDirectory ( &rootDirectory );
	rootDirectoryPathOffset = GXWcslen ( rootDirectory );
	currentDirectory = nullptr;
	UpdateDirectory ( rootDirectory );

	mainPanel->Resize ( EM_PANEL_LEFT_BOTTOM_X * gx_ui_Scale, EM_PANEL_LEFT_BOTTOM_Y * gx_ui_Scale, EM_PANEL_WIDTH * gx_ui_Scale, EM_PANEL_HEIGHT * gx_ui_Scale );

	mainPanel->Hide ();
}

EMUIOpenFile::~EMUIOpenFile ()
{
	free ( rootDirectory );
	GXSafeFree ( currentDirectory );

	delete fileListBox;
	delete bottomSeparator;
	delete topSeparator;
	delete filePathStaticText;
	delete okButton;
	delete cancelButton;
	delete mainPanel;
}

GXWidget* EMUIOpenFile::GetWidget () const
{
	return mainPanel->GetWidget ();
}

GXVoid EMUIOpenFile::Browse ( PFNEMONBROWSEFILEPROC callback )
{
	if ( !callback ) return;

	OnBrowseFile = callback;
	mainPanel->Show ();
	mainPanel->ToForeground ();
}

GXVoid EMUIOpenFile::UpdateDirectory ( const GXWChar* folder )
{
	if ( currentDirectory )
	{
		GXUInt size = ( GXWcslen ( currentDirectory ) + GXWcslen ( folder ) + 2 ) * sizeof ( GXWChar );
		GXWChar* newCurrentDirectory = (GXWChar*)malloc ( size );
		wsprintfW ( newCurrentDirectory, L"%s/%s", currentDirectory, folder );
		free ( currentDirectory );
		currentDirectory = newCurrentDirectory;
	}
	else
	{
		GXWcsclone ( &currentDirectory, rootDirectory );
	}

	GXUInt totalItems = 0;
	EMUIFileListBoxItem* items = nullptr;
	GXUInt itemIndex = 0;
	GXDirectoryInfo directoryInfo;

	if ( GXGetDirectoryInfo ( directoryInfo, currentDirectory ) )
	{
		free ( currentDirectory );
		GXWcsclone ( &currentDirectory, directoryInfo.absolutePath );
		fileListBox->Clear ();
		filePathStaticText->SetText ( GetRelativePath () );

		if ( GXWcscmp ( directoryInfo.absolutePath, rootDirectory ) == 0 )
		{
			totalItems = directoryInfo.totalFiles + directoryInfo.totalFolders - 2; // Exclude . and .. directories

			if ( totalItems == 0 ) return;

			items = (EMUIFileListBoxItem*)malloc ( totalItems * sizeof ( EMUIFileListBoxItem ) );
			GXBool isRootNotExcluded = GX_TRUE;
			GXBool isWorkingDirectoryNotExcluded = GX_TRUE;

			for ( GXUInt i = 0; i < directoryInfo.totalFolders; i++ )
			{
				if ( isRootNotExcluded && GXWcscmp ( directoryInfo.folderNames[ i ], L".." ) == 0 )
				{
					isRootNotExcluded = GX_FALSE;
					continue;
				}

				if ( isWorkingDirectoryNotExcluded && GXWcscmp ( directoryInfo.folderNames[ i ], L"." ) == 0 )
				{
					isWorkingDirectoryNotExcluded = GX_FALSE;
					continue;
				}

				items[ itemIndex ].type = eEMUIFileListBoxItemType::Folder;
				items[ itemIndex ].name = (GXWChar*)directoryInfo.folderNames[ i ];
				itemIndex++;
			}
		}
		else
		{
			totalItems = directoryInfo.totalFiles + directoryInfo.totalFolders - 1; // Exclude . directory

			if ( totalItems == 0 ) return;

			items = (EMUIFileListBoxItem*)malloc ( totalItems * sizeof ( EMUIFileListBoxItem ) );
			GXBool isWorkingDirectoryNotExcluded = GX_TRUE;

			for ( GXUInt i = 0; i < directoryInfo.totalFolders; i++ )
			{
				if ( isWorkingDirectoryNotExcluded && GXWcscmp ( directoryInfo.folderNames[ i ], L"." ) == 0 )
				{
					isWorkingDirectoryNotExcluded = GX_FALSE;
					continue;
				}

				items[ itemIndex ].type = eEMUIFileListBoxItemType::Folder;
				items[ itemIndex ].name = (GXWChar*)directoryInfo.folderNames[ i ];
				itemIndex++;
			}
		}

		for ( GXUInt i = 0; i < directoryInfo.totalFiles; i++ )
		{
			items[ itemIndex ].type = eEMUIFileListBoxItemType::File;
			items[ itemIndex ].name = (GXWChar*)directoryInfo.fileNames[ i ];
			itemIndex++;
		}

		fileListBox->AddItems ( items, totalItems );
		free ( items );
		fileListBox->Redraw ();
	}
	else
	{
		GXLogW ( L"EMUIOpenFile::UpdateDirectory::Error - Can't open directory %s\n", currentDirectory );
		GXSafeFree ( currentDirectory );
	}
}

const GXWChar* EMUIOpenFile::GetRelativePath () const
{
	if ( GXWcscmp ( currentDirectory, rootDirectory ) == 0 )
		return currentDirectory + rootDirectoryPathOffset;

	return currentDirectory + rootDirectoryPathOffset + 1;
}

GXVoid GXCALL EMUIOpenFile::OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state != eGXMouseButtonState::Up ) return;

	EMUIOpenFile* main = (EMUIOpenFile*)handler;

	if ( button == main->okButton->GetWidget () )
	{
		EMUIFileListBoxItem* i = (EMUIFileListBoxItem*)main->fileListBox->GetSelectedItem ();
		if ( i && i->type == eEMUIFileListBoxItemType::File )
			main->OnBrowseFile ( main->filePathStaticText->GetText () );

		main->mainPanel->Hide ();
	}
	else if ( button == main->cancelButton->GetWidget () )
	{
		EMUIOpenFile* main = (EMUIOpenFile*)handler;
		main->mainPanel->Hide ();
	}
}

GXVoid GXCALL EMUIOpenFile::OnItemSelected ( GXVoid* handler, GXUIListBox* listBox, const GXVoid* item )
{
	EMUIOpenFile* main = (EMUIOpenFile*)handler;
	const EMUIFileListBoxItem* element = (const EMUIFileListBoxItem*)item;

	switch ( element->type )
	{
		case eEMUIFileListBoxItemType::File:
		{
			if ( GXWcscmp ( main->currentDirectory, main->rootDirectory ) == 0 )
			{
				main->filePathStaticText->SetText ( element->name );
			}
			else
			{
				GXUInt symbols = main->rootDirectoryPathOffset;
				symbols += 1;									// '/' symbol
				symbols += GXWcslen ( element->name );
				symbols += 1;									// '\0' symbol

				GXWChar* buf = (GXWChar*)malloc ( symbols * sizeof ( GXWChar ) );
				wsprintfW ( buf, L"%s/%s", main->GetRelativePath (), element->name );
				main->filePathStaticText->SetText ( buf );
				free ( buf );
			}
		}
		break;

		case eEMUIFileListBoxItemType::Folder:
			main->filePathStaticText->SetText ( main->GetRelativePath () );
		break;
	}
}

GXVoid GXCALL EMUIOpenFile::OnItemDoubleClicked ( GXVoid* handler, GXUIListBox* listBox, const GXVoid* item )
{
	EMUIOpenFile* main = (EMUIOpenFile*)handler;
	const EMUIFileListBoxItem* element = (const EMUIFileListBoxItem*)item;

	switch ( element->type )
	{
		case eEMUIFileListBoxItemType::File:
			OnButton ( main, (GXUIButton*)main->okButton->GetWidget (), 0.0f, 0.0f, eGXMouseButtonState::Up );
		break;

		case eEMUIFileListBoxItemType::Folder:
			main->UpdateDirectory ( element->name );
		break;
	}
}

GXVoid GXCALL EMUIOpenFile::OnResize ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height )
{
	EMUIOpenFile* main = (EMUIOpenFile*)handler;

	main->cancelButton->Resize ( width - EM_BTN_CANCEL_LEFT_BOTTOM_X * gx_ui_Scale, EM_BTN_BOTTOM_Y * gx_ui_Scale, EM_BTN_WIDTH * gx_ui_Scale, EM_BTN_HEIGHT * gx_ui_Scale );
	main->okButton->Resize ( width - EM_BTN_OK_LEFT_BOTTOM_X * gx_ui_Scale, EM_BTN_BOTTOM_Y * gx_ui_Scale, EM_BTN_WIDTH * gx_ui_Scale, EM_BTN_HEIGHT * gx_ui_Scale );

	main->filePathStaticText->Resize ( EM_STT_FILE_PATH_LEFT_BOTTOM_X * gx_ui_Scale, height - EM_STT_FILE_PATH_LEFT_BOTTOM_Y * gx_ui_Scale, width - 2.0f * EM_STT_FILE_PATH_LEFT_BOTTOM_X * gx_ui_Scale, EM_STT_FILE_PATH_HEIGHT * gx_ui_Scale );

	main->bottomSeparator->Resize ( EM_SEP_OFFSET_X * gx_ui_Scale, EM_SEP_BOTTOM_OFFSET_Y * gx_ui_Scale, width - 2.0f * EM_SEP_OFFSET_X * gx_ui_Scale, EM_SEP_HEIGHT * gx_ui_Scale );
	main->topSeparator->Resize ( EM_SEP_OFFSET_X * gx_ui_Scale, height - EM_SEP_TOP_OFFSET_Y * gx_ui_Scale, width - 2.0f * EM_SEP_OFFSET_X * gx_ui_Scale, EM_SEP_HEIGHT * gx_ui_Scale );
	
	main->fileListBox->Resize ( EM_LST_OFFSET_LEFT * gx_ui_Scale, EM_LST_OFFSET_BOTTOM * gx_ui_Scale, width - ( EM_LST_OFFSET_LEFT + EM_LST_OFFSET_RIGHT ) * gx_ui_Scale, height - ( EM_LST_OFFSET_BOTTOM + EM_LST_OFFSET_TOP ) * gx_ui_Scale );
}
