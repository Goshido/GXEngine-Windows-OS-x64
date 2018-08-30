#include <GXEngine_Editor_Mobile/EMUIOpenFile.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXEngine/GXLocale.h>


#define PANEL_LEFT_BOTTOM_X				7.6f
#define PANEL_LEFT_BOTTOM_Y				5.8f
#define PANEL_WIDTH						13.4f
#define PANEL_HEIGHT					9.5f
#define PANEL_HEADER_HEIGHT				0.5f

#define BTN_OK_LEFT_BOTTOM_X			2.0f
#define BTN_OK_TEXT						L"OpenFile->OK"
#define BTN_CANCEL_LEFT_BOTTOM_X		3.9f
#define BTN_CANCEL_TEXT					L"OpenFile->Cancel"
#define BTN_BOTTOM_Y					0.15f
#define BTN_WIDTH						1.7f
#define BTN_HEIGHT						0.55f

#define STT_FILE_PATH_LEFT_BOTTOM_X		0.2f
#define STT_FILE_PATH_LEFT_BOTTOM_Y		1.0f
#define STT_FILE_PATH_HEIGHT			0.3f

#define SEP_BOTTOM_OFFSET_Y				0.9f
#define SEP_TOP_OFFSET_Y				1.2f
#define SEP_HEIGHT						0.06f
#define SEP_OFFSET_X					0.2f

#define LST_OFFSET_LEFT					0.2f
#define LST_OFFSET_RIGHT				0.7f
#define LST_OFFSET_BOTTOM				1.1f
#define LST_OFFSET_TOP					1.3f

#define PATH_BUFFER_SIZE				4096u

//---------------------------------------------------------------------------------------------------------------------

EMUIOpenFile::EMUIOpenFile ():
	EMUI ( nullptr ),
	currentDirectory ( nullptr )
{
	GXGetCurrentDirectory ( &rootDirectory );
	rootDirectoryPathOffset = GXWcslen ( rootDirectory );

	mainPanel = new EMUIDraggableArea ( nullptr );
	mainPanel->SetHeaderHeight ( PANEL_HEADER_HEIGHT * gx_ui_Scale );
	mainPanel->SetOnResizeCallback ( this, &EMUIOpenFile::OnResize );

	okButton = new EMUIButton ( mainPanel );
	cancelButton = new EMUIButton ( mainPanel );
	filePathStaticText = new EMUIStaticText ( mainPanel );
	bottomSeparator = new EMUISeparator ( mainPanel );
	topSeparator = new EMUISeparator ( mainPanel );
	fileListBox = new EMUIFileListBox ( mainPanel );

	GXLocale& locale = GXLocale::GetInstance ();

	okButton->SetCaption ( locale.GetString ( BTN_OK_TEXT ) );
	okButton->SetOnLeftMouseButtonCallback ( this, &EMUIOpenFile::OnButton );

	cancelButton->SetCaption ( locale.GetString ( BTN_CANCEL_TEXT ) );
	cancelButton->SetOnLeftMouseButtonCallback ( this, &EMUIOpenFile::OnButton );

	filePathStaticText->SetText ( nullptr );
	filePathStaticText->SetAlingment ( eGXUITextAlignment::Left );
	
	fileListBox->SetOnItemSelectedCallback ( this, &EMUIOpenFile::OnItemSelected );
	fileListBox->SetOnItemDoubleClickedCallbak ( this, &EMUIOpenFile::OnItemDoubleClicked );

	UpdateDirectory ( rootDirectory );

	mainPanel->Resize ( PANEL_LEFT_BOTTOM_X * gx_ui_Scale, PANEL_LEFT_BOTTOM_Y * gx_ui_Scale, PANEL_WIDTH * gx_ui_Scale, PANEL_HEIGHT * gx_ui_Scale );
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
		GXUPointer size = ( GXWcslen ( currentDirectory ) + GXWcslen ( folder ) + 2 ) * sizeof ( GXWChar );
		GXWChar* newCurrentDirectory = static_cast<GXWChar*> ( malloc ( size ) );
		wsprintfW ( newCurrentDirectory, L"%s/%s", currentDirectory, folder );
		free ( currentDirectory );
		currentDirectory = newCurrentDirectory;
	}
	else
	{
		GXWcsclone ( &currentDirectory, rootDirectory );
	}

	GXUInt totalItems = 0u;
	EMUIFileListBoxItem* items = nullptr;
	GXUInt itemIndex = 0u;
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

			if ( totalItems == 0u ) return;

			items = static_cast<EMUIFileListBoxItem*> ( malloc ( totalItems * sizeof ( EMUIFileListBoxItem ) ) );
			GXBool isRootNotExcluded = GX_TRUE;
			GXBool isWorkingDirectoryNotExcluded = GX_TRUE;

			for ( GXUInt i = 0u; i < directoryInfo.totalFolders; ++i )
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

				items[ itemIndex ].SetType ( eEMUIFileListBoxItemType::Folder );
				items[ itemIndex ].SetName ( directoryInfo.folderNames[ i ] );
				++itemIndex;
			}
		}
		else
		{
			totalItems = directoryInfo.totalFiles + directoryInfo.totalFolders - 1; // Exclude . directory

			if ( totalItems == 0u ) return;

			items = static_cast<EMUIFileListBoxItem*> ( malloc ( totalItems * sizeof ( EMUIFileListBoxItem ) ) );
			GXBool isWorkingDirectoryNotExcluded = GX_TRUE;

			for ( GXUInt i = 0u; i < directoryInfo.totalFolders; ++i )
			{
				if ( isWorkingDirectoryNotExcluded && GXWcscmp ( directoryInfo.folderNames[ i ], L"." ) == 0 )
				{
					isWorkingDirectoryNotExcluded = GX_FALSE;
					continue;
				}

				items[ itemIndex ].SetType ( eEMUIFileListBoxItemType::Folder );
				items[ itemIndex ].SetName ( directoryInfo.folderNames[ i ] );
				++itemIndex;
			}
		}

		for ( GXUInt i = 0u; i < directoryInfo.totalFiles; ++i )
		{
			items[ itemIndex ].SetType ( eEMUIFileListBoxItemType::File );
			items[ itemIndex ].SetName ( directoryInfo.fileNames[ i ] );
			++itemIndex;
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

GXVoid GXCALL EMUIOpenFile::OnButton ( GXVoid* handler, GXUIButton& button, GXFloat /*x*/, GXFloat /*y*/, eGXMouseButtonState state )
{
	if ( state != eGXMouseButtonState::Up ) return;

	EMUIOpenFile* main = static_cast<EMUIOpenFile*> ( handler );

	if ( &button == main->okButton->GetWidget () )
	{
		const EMUIFileListBoxItem* i = static_cast<const EMUIFileListBoxItem*> ( main->fileListBox->GetSelectedItem () );

		if ( i && i->GetType () == eEMUIFileListBoxItemType::File )
			main->OnBrowseFile ( main->filePathStaticText->GetText () );

		main->mainPanel->Hide ();
	}
	else if ( &button == main->cancelButton->GetWidget () )
	{
		main->mainPanel->Hide ();
	}
}

GXVoid GXCALL EMUIOpenFile::OnItemSelected ( GXVoid* handler, GXUIListBox& /*listBox*/, const GXVoid* item )
{
	EMUIOpenFile* main = static_cast<EMUIOpenFile*> ( handler );
	const EMUIFileListBoxItem* element = static_cast<const EMUIFileListBoxItem*> ( item );

	switch ( element->GetType () )
	{
		case eEMUIFileListBoxItemType::File:
		{
			if ( GXWcscmp ( main->currentDirectory, main->rootDirectory ) == 0 )
			{
				main->filePathStaticText->SetText ( element->GetName () );
			}
			else
			{
				GXUInt symbols = main->rootDirectoryPathOffset;
				symbols += 1u;									// '/' symbol
				symbols += GXWcslen ( element->GetName () );
				symbols += 1u;									// '\0' symbol

				GXWChar* buf = static_cast<GXWChar*> ( malloc ( symbols * sizeof ( GXWChar ) ) );
				wsprintfW ( buf, L"%s/%s", main->GetRelativePath (), element->GetName () );
				main->filePathStaticText->SetText ( buf );
				free ( buf );
			}
		}
		break;

		case eEMUIFileListBoxItemType::Folder:
			main->filePathStaticText->SetText ( main->GetRelativePath () );
		break;

		default:
			//NOTHING
		break;
	}
}

GXVoid GXCALL EMUIOpenFile::OnItemDoubleClicked ( GXVoid* handler, GXUIListBox& /*listBox*/, const GXVoid* item )
{
	EMUIOpenFile* main = static_cast<EMUIOpenFile*> ( handler );
	const EMUIFileListBoxItem* element = static_cast<const EMUIFileListBoxItem*> ( item );

	switch ( element->GetType () )
	{
		case eEMUIFileListBoxItemType::File:
			OnButton ( main, *( static_cast<GXUIButton*> ( main->okButton->GetWidget () ) ), 0.0f, 0.0f, eGXMouseButtonState::Up );
		break;

		case eEMUIFileListBoxItemType::Folder:
			main->UpdateDirectory ( element->GetName () );
		break;
	}
}

GXVoid GXCALL EMUIOpenFile::OnResize ( GXVoid* handler, GXUIDragableArea& /*area*/, GXFloat width, GXFloat height )
{
	EMUIOpenFile* main = static_cast<EMUIOpenFile*> ( handler );

	main->cancelButton->Resize ( width - BTN_CANCEL_LEFT_BOTTOM_X * gx_ui_Scale, BTN_BOTTOM_Y * gx_ui_Scale, BTN_WIDTH * gx_ui_Scale, BTN_HEIGHT * gx_ui_Scale );
	main->okButton->Resize ( width - BTN_OK_LEFT_BOTTOM_X * gx_ui_Scale, BTN_BOTTOM_Y * gx_ui_Scale, BTN_WIDTH * gx_ui_Scale, BTN_HEIGHT * gx_ui_Scale );

	main->filePathStaticText->Resize ( STT_FILE_PATH_LEFT_BOTTOM_X * gx_ui_Scale, height - STT_FILE_PATH_LEFT_BOTTOM_Y * gx_ui_Scale, width - 2.0f * STT_FILE_PATH_LEFT_BOTTOM_X * gx_ui_Scale, STT_FILE_PATH_HEIGHT * gx_ui_Scale );

	main->bottomSeparator->Resize ( SEP_OFFSET_X * gx_ui_Scale, SEP_BOTTOM_OFFSET_Y * gx_ui_Scale, width - 2.0f * SEP_OFFSET_X * gx_ui_Scale, SEP_HEIGHT * gx_ui_Scale );
	main->topSeparator->Resize ( SEP_OFFSET_X * gx_ui_Scale, height - SEP_TOP_OFFSET_Y * gx_ui_Scale, width - 2.0f * SEP_OFFSET_X * gx_ui_Scale, SEP_HEIGHT * gx_ui_Scale );
	
	main->fileListBox->Resize ( LST_OFFSET_LEFT * gx_ui_Scale, LST_OFFSET_BOTTOM * gx_ui_Scale, width - ( LST_OFFSET_LEFT + LST_OFFSET_RIGHT ) * gx_ui_Scale, height - ( LST_OFFSET_BOTTOM + LST_OFFSET_TOP ) * gx_ui_Scale );
}
