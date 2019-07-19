#include <GXEngine_Editor_Mobile/EMUIOpenFile.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXEngine/GXLocale.h>


#define PANEL_LEFT_BOTTOM_X             7.6f
#define PANEL_LEFT_BOTTOM_Y             5.8f
#define PANEL_WIDTH                     13.4f
#define PANEL_HEIGHT                    9.5f
#define PANEL_HEADER_HEIGHT             0.5f

#define BTN_OK_LEFT_BOTTOM_X            2.0f
#define BTN_OK_TEXT                     L"OpenFile->OK"
#define BTN_CANCEL_LEFT_BOTTOM_X        3.9f
#define BTN_CANCEL_TEXT                 L"OpenFile->Cancel"
#define BTN_BOTTOM_Y                    0.15f
#define BTN_WIDTH                       1.7f
#define BTN_HEIGHT                      0.55f

#define STT_FILE_PATH_LEFT_BOTTOM_X     0.2f
#define STT_FILE_PATH_LEFT_BOTTOM_Y     1.0f
#define STT_FILE_PATH_HEIGHT            0.3f

#define SEP_BOTTOM_OFFSET_Y             0.9f
#define SEP_TOP_OFFSET_Y                1.2f
#define SEP_HEIGHT                      0.06f
#define SEP_OFFSET_X                    0.2f

#define LST_OFFSET_LEFT                 0.2f
#define LST_OFFSET_RIGHT                0.7f
#define LST_OFFSET_BOTTOM               1.1f
#define LST_OFFSET_TOP                  1.3f

#define PATH_BUFFER_SIZE                4096u

//---------------------------------------------------------------------------------------------------------------------

EMUIOpenFile::EMUIOpenFile ():
    EMUI ( nullptr ),
    _currentDirectory ( nullptr ),
    _rootDirectory ( GXGetCurrentDirectory () )
{
    _rootDirectoryPathOffset = _rootDirectory.GetSymbolCount ();

    _mainPanel = new EMUIDraggableArea ( nullptr );
    _mainPanel->SetHeaderHeight ( PANEL_HEADER_HEIGHT * gx_ui_Scale );
    _mainPanel->SetOnResizeCallback ( this, &EMUIOpenFile::OnResize );

    _okButton = new EMUIButton ( _mainPanel );
    _cancelButton = new EMUIButton ( _mainPanel );
    _filePathStaticText = new EMUIStaticText ( _mainPanel );
    _bottomSeparator = new EMUISeparator ( _mainPanel );
    _topSeparator = new EMUISeparator ( _mainPanel );
    _fileListBox = new EMUIFileListBox ( _mainPanel );

    const GXLocale& locale = GXLocale::GetInstance ();

    _okButton->SetCaption ( locale.GetString ( BTN_OK_TEXT ) );
    _okButton->SetOnLeftMouseButtonCallback ( this, &EMUIOpenFile::OnButton );

    _cancelButton->SetCaption ( locale.GetString ( BTN_CANCEL_TEXT ) );
    _cancelButton->SetOnLeftMouseButtonCallback ( this, &EMUIOpenFile::OnButton );

    _filePathStaticText->SetText ( nullptr );
    _filePathStaticText->SetAlingment ( eGXUITextAlignment::Left );
    
    _fileListBox->SetOnItemSelectedCallback ( this, &EMUIOpenFile::OnItemSelected );
    _fileListBox->SetOnItemDoubleClickedCallbak ( this, &EMUIOpenFile::OnItemDoubleClicked );

    UpdateDirectory ( _rootDirectory );

    _mainPanel->Resize ( PANEL_LEFT_BOTTOM_X * gx_ui_Scale, PANEL_LEFT_BOTTOM_Y * gx_ui_Scale, PANEL_WIDTH * gx_ui_Scale, PANEL_HEIGHT * gx_ui_Scale );
    _mainPanel->Hide ();
}

EMUIOpenFile::~EMUIOpenFile ()
{
    GXSafeFree ( _currentDirectory );

    delete _fileListBox;
    delete _bottomSeparator;
    delete _topSeparator;
    delete _filePathStaticText;
    delete _okButton;
    delete _cancelButton;
    delete _mainPanel;
}

GXWidget* EMUIOpenFile::GetWidget () const
{
    return _mainPanel->GetWidget ();
}

GXVoid EMUIOpenFile::Browse ( EMUIOpenFileBrowseFileHandler callback )
{
    if ( !callback ) return;

    _onBrowseFile = callback;
    _mainPanel->Show ();
    _mainPanel->ToForeground ();
}

GXVoid EMUIOpenFile::UpdateDirectory ( const GXWChar* folder )
{
    if ( _currentDirectory )
    {
        const GXUPointer size = ( GXWcslen ( _currentDirectory ) + GXWcslen ( folder ) + 2u ) * sizeof ( GXWChar );
        GXWChar* newCurrentDirectory = static_cast<GXWChar*> ( malloc ( size ) );
        wsprintfW ( newCurrentDirectory, L"%s/%s", _currentDirectory, folder );
        free ( _currentDirectory );
        _currentDirectory = newCurrentDirectory;
    }
    else
    {
        GXWcsclone ( &_currentDirectory, _rootDirectory );
    }

    GXUInt totalItems = 0u;
    EMUIFileListBoxItem* items = nullptr;
    GXUInt itemIndex = 0u;
    GXDirectoryInfo directoryInfo;

    if ( GXGetDirectoryInfo ( directoryInfo, _currentDirectory ) )
    {
        free ( _currentDirectory );
        GXWcsclone ( &_currentDirectory, directoryInfo._absolutePath );
        _fileListBox->Clear ();
        _filePathStaticText->SetText ( GetRelativePath () );

        if ( _rootDirectory == directoryInfo._absolutePath )
        {
            totalItems = directoryInfo._totalFiles + directoryInfo._totalFolders - 2; // Exclude . and .. directories

            if ( totalItems == 0u ) return;

            items = static_cast<EMUIFileListBoxItem*> ( malloc ( totalItems * sizeof ( EMUIFileListBoxItem ) ) );
            GXBool isRootNotExcluded = GX_TRUE;
            GXBool isWorkingDirectoryNotExcluded = GX_TRUE;

            for ( GXUInt i = 0u; i < directoryInfo._totalFolders; ++i )
            {
                if ( isRootNotExcluded && GXWcscmp ( directoryInfo._folderNames[ i ], L".." ) == 0 )
                {
                    isRootNotExcluded = GX_FALSE;
                    continue;
                }

                if ( isWorkingDirectoryNotExcluded && GXWcscmp ( directoryInfo._folderNames[ i ], L"." ) == 0 )
                {
                    isWorkingDirectoryNotExcluded = GX_FALSE;
                    continue;
                }

                items[ itemIndex ].SetType ( eEMUIFileListBoxItemType::Folder );
                items[ itemIndex ].SetName ( directoryInfo._folderNames[ i ] );
                ++itemIndex;
            }
        }
        else
        {
            totalItems = directoryInfo._totalFiles + directoryInfo._totalFolders - 1; // Exclude . directory

            if ( totalItems == 0u ) return;

            items = static_cast<EMUIFileListBoxItem*> ( malloc ( totalItems * sizeof ( EMUIFileListBoxItem ) ) );
            GXBool isWorkingDirectoryNotExcluded = GX_TRUE;

            for ( GXUInt i = 0u; i < directoryInfo._totalFolders; ++i )
            {
                if ( isWorkingDirectoryNotExcluded && GXWcscmp ( directoryInfo._folderNames[ i ], L"." ) == 0 )
                {
                    isWorkingDirectoryNotExcluded = GX_FALSE;
                    continue;
                }

                items[ itemIndex ].SetType ( eEMUIFileListBoxItemType::Folder );
                items[ itemIndex ].SetName ( directoryInfo._folderNames[ i ] );
                ++itemIndex;
            }
        }

        for ( GXUInt i = 0u; i < directoryInfo._totalFiles; ++i )
        {
            items[ itemIndex ].SetType ( eEMUIFileListBoxItemType::File );
            items[ itemIndex ].SetName ( directoryInfo._fileNames[ i ] );
            ++itemIndex;
        }

        _fileListBox->AddItems ( items, totalItems );
        free ( items );
        _fileListBox->Redraw ();
    }
    else
    {
        GXLogA ( "EMUIOpenFile::UpdateDirectory::Error - Can't open directory %S\n", _currentDirectory );
        GXSafeFree ( _currentDirectory );
    }
}

const GXWChar* EMUIOpenFile::GetRelativePath () const
{
    if ( _rootDirectory == _currentDirectory )
        return _currentDirectory + _rootDirectoryPathOffset;

    return _currentDirectory + _rootDirectoryPathOffset + 1;
}

GXVoid GXCALL EMUIOpenFile::OnButton ( GXVoid* handler, GXUIButton& button, GXFloat /*x*/, GXFloat /*y*/, eGXMouseButtonState state )
{
    if ( state != eGXMouseButtonState::Up ) return;

    EMUIOpenFile* main = static_cast<EMUIOpenFile*> ( handler );

    if ( &button == main->_okButton->GetWidget () )
    {
        const EMUIFileListBoxItem* i = static_cast<const EMUIFileListBoxItem*> ( main->_fileListBox->GetSelectedItem () );

        if ( i && i->GetType () == eEMUIFileListBoxItemType::File )
            main->_onBrowseFile ( main->_filePathStaticText->GetText () );

        main->_mainPanel->Hide ();
    }
    else if ( &button == main->_cancelButton->GetWidget () )
    {
        main->_mainPanel->Hide ();
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
            if ( main->_rootDirectory == main->_currentDirectory )
            {
                main->_filePathStaticText->SetText ( element->GetName () );
            }
            else
            {
                GXString buf;
                buf.Format ( "%S/%S", main->GetRelativePath (), element->GetName () );
                main->_filePathStaticText->SetText ( buf );
            }
        }
        break;

        case eEMUIFileListBoxItemType::Folder:
            main->_filePathStaticText->SetText ( main->GetRelativePath () );
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
            OnButton ( main, *( static_cast<GXUIButton*> ( main->_okButton->GetWidget () ) ), 0.0f, 0.0f, eGXMouseButtonState::Up );
        break;

        case eEMUIFileListBoxItemType::Folder:
            main->UpdateDirectory ( element->GetName () );
        break;
    }
}

GXVoid GXCALL EMUIOpenFile::OnResize ( GXVoid* handler, GXUIDragableArea& /*area*/, GXFloat width, GXFloat height )
{
    EMUIOpenFile* main = static_cast<EMUIOpenFile*> ( handler );

    main->_cancelButton->Resize ( width - BTN_CANCEL_LEFT_BOTTOM_X * gx_ui_Scale, BTN_BOTTOM_Y * gx_ui_Scale, BTN_WIDTH * gx_ui_Scale, BTN_HEIGHT * gx_ui_Scale );
    main->_okButton->Resize ( width - BTN_OK_LEFT_BOTTOM_X * gx_ui_Scale, BTN_BOTTOM_Y * gx_ui_Scale, BTN_WIDTH * gx_ui_Scale, BTN_HEIGHT * gx_ui_Scale );

    main->_filePathStaticText->Resize ( STT_FILE_PATH_LEFT_BOTTOM_X * gx_ui_Scale, height - STT_FILE_PATH_LEFT_BOTTOM_Y * gx_ui_Scale, width - 2.0f * STT_FILE_PATH_LEFT_BOTTOM_X * gx_ui_Scale, STT_FILE_PATH_HEIGHT * gx_ui_Scale );

    main->_bottomSeparator->Resize ( SEP_OFFSET_X * gx_ui_Scale, SEP_BOTTOM_OFFSET_Y * gx_ui_Scale, width - 2.0f * SEP_OFFSET_X * gx_ui_Scale, SEP_HEIGHT * gx_ui_Scale );
    main->_topSeparator->Resize ( SEP_OFFSET_X * gx_ui_Scale, height - SEP_TOP_OFFSET_Y * gx_ui_Scale, width - 2.0f * SEP_OFFSET_X * gx_ui_Scale, SEP_HEIGHT * gx_ui_Scale );
    
    main->_fileListBox->Resize ( LST_OFFSET_LEFT * gx_ui_Scale, LST_OFFSET_BOTTOM * gx_ui_Scale, width - ( LST_OFFSET_LEFT + LST_OFFSET_RIGHT ) * gx_ui_Scale, height - ( LST_OFFSET_BOTTOM + LST_OFFSET_TOP ) * gx_ui_Scale );
}
