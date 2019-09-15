#include <GXEngine_Editor_Mobile/EMUIOpenFile.h>
#include <GXCommon/GXDirectoryInfo.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXEngine/GXLocale.h>


#define PANEL_LEFT_BOTTOM_X             7.6f
#define PANEL_LEFT_BOTTOM_Y             5.8f
#define PANEL_WIDTH                     13.4f
#define PANEL_HEIGHT                    9.5f
#define PANEL_HEADER_HEIGHT             0.5f

#define BTN_OK_LEFT_BOTTOM_X            2.0f
#define BTN_OK_TEXT                     "OpenFile->OK"
#define BTN_CANCEL_LEFT_BOTTOM_X        3.9f
#define BTN_CANCEL_TEXT                 "OpenFile->Cancel"
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

//---------------------------------------------------------------------------------------------------------------------

EMUIOpenFile::EMUIOpenFile ():
    EMUI ( nullptr ),
    _rootDirectory ( GXGetCurrentDirectory () ),
    _mainPanel ( nullptr )
{
    _rootDirectoryPathOffset = _rootDirectory.GetSymbolCount () + 1u;

    _mainPanel.SetHeaderHeight ( PANEL_HEADER_HEIGHT * gx_ui_Scale );
    _mainPanel.SetOnResizeCallback ( this, &EMUIOpenFile::OnResize );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButton" )
    _okButton = new EMUIButton ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButton" )
    _cancelButton = new EMUIButton ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _filePathStaticText = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparator" )
    _bottomSeparator = new EMUISeparator ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparator" )
    _topSeparator = new EMUISeparator ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIFileListBox" )
    _fileListBox = new EMUIFileListBox ( &_mainPanel );

    const GXLocale& locale = GXLocale::GetInstance ();

    _okButton->SetCaption ( locale.GetString ( BTN_OK_TEXT ) );
    _okButton->SetOnLeftMouseButtonCallback ( this, &EMUIOpenFile::OnButton );

    _cancelButton->SetCaption ( locale.GetString ( BTN_CANCEL_TEXT ) );
    _cancelButton->SetOnLeftMouseButtonCallback ( this, &EMUIOpenFile::OnButton );

    _filePathStaticText->SetAlingment ( eGXUITextAlignment::Left );
    
    _fileListBox->SetOnItemSelectedCallback ( this, &EMUIOpenFile::OnItemSelected );
    _fileListBox->SetOnItemDoubleClickedCallbak ( this, &EMUIOpenFile::OnItemDoubleClicked );

    UpdateDirectory ( _rootDirectory );

    _mainPanel.Resize ( PANEL_LEFT_BOTTOM_X * gx_ui_Scale, PANEL_LEFT_BOTTOM_Y * gx_ui_Scale, PANEL_WIDTH * gx_ui_Scale, PANEL_HEIGHT * gx_ui_Scale );
    _mainPanel.Hide ();
}

EMUIOpenFile::~EMUIOpenFile ()
{
    delete _fileListBox;
    delete _bottomSeparator;
    delete _topSeparator;
    delete _filePathStaticText;
    delete _okButton;
    delete _cancelButton;
}

GXWidget* EMUIOpenFile::GetWidget ()
{
    return _mainPanel.GetWidget ();
}

GXVoid EMUIOpenFile::Browse ( EMUIOpenFileBrowseFileHandler callback )
{
    if ( !callback ) return;

    _onBrowseFile = callback;
    _mainPanel.Show ();
    _mainPanel.ToForeground ();
}

GXVoid EMUIOpenFile::UpdateDirectory ( GXString targetDirectory )
{
    if ( _currentDirectory.IsNull () )
    {
        _currentDirectory = targetDirectory;
    }
    else
    {
        _stringBuffer.Format ( "%s/%s", static_cast<const GXMBChar*> ( _currentDirectory ), static_cast<const GXMBChar*> ( targetDirectory ) );
        _currentDirectory = _stringBuffer;
    }

    GXUPointer totalItems = 0u;
    EMUIFileListBoxItem* items = nullptr;
    GXUPointer itemIndex = 0u;

    const GXDirectoryInfo directoryInfo ( _currentDirectory );
    const GXUPointer directories = directoryInfo.GetDirectoryCount ();
    const GXUPointer files = directoryInfo.GetFileCount ();

    _currentDirectory = directoryInfo.GetAbsolutePath ();
    _relativeDirectory = _currentDirectory.GetRight ( _rootDirectoryPathOffset );
    _fileListBox->Clear ();
    _filePathStaticText->SetText ( _relativeDirectory );

    if ( _rootDirectory == _currentDirectory )
    {
        _relativeDirectory.Clear ();
        totalItems = directories + files - 2u; // Exclude . and .. directories

        if ( totalItems == 0u ) return;

        items = static_cast<EMUIFileListBoxItem*> ( Malloc ( totalItems * sizeof ( EMUIFileListBoxItem ) ) );
        GXBool isRootNotExcluded = GX_TRUE;
        GXBool isWorkingDirectoryNotExcluded = GX_TRUE;

        for ( GXUPointer i = 0u; i < directories; ++i )
        {
            const GXString directory = directoryInfo.GetDirectoryName ( i );

            if ( isRootNotExcluded && directory == ".." )
            {
                isRootNotExcluded = GX_FALSE;
                continue;
            }

            if ( isWorkingDirectoryNotExcluded && directory == "." )
            {
                isWorkingDirectoryNotExcluded = GX_FALSE;
                continue;
            }

            // Note placement new syntax.
            ::new ( items + itemIndex ) EMUIFileListBoxItem ( eEMUIFileListBoxItemType::Folder, directory );
            ++itemIndex;
        }
    }
    else
    {
        totalItems = directories + files - 1u; // Exclude . directory

        if ( totalItems == 0u ) return;

        items = static_cast<EMUIFileListBoxItem*> ( Malloc ( totalItems * sizeof ( EMUIFileListBoxItem ) ) );
        GXBool isWorkingDirectoryNotExcluded = GX_TRUE;

        for ( GXUPointer i = 0u; i < directories; ++i )
        {
            const GXString directory = directoryInfo.GetDirectoryName ( i );

            if ( isWorkingDirectoryNotExcluded && directory == "." )
            {
                isWorkingDirectoryNotExcluded = GX_FALSE;
                continue;
            }

            // Note placement new syntax.
            ::new ( items + itemIndex ) EMUIFileListBoxItem ( eEMUIFileListBoxItemType::Folder, directory );
            ++itemIndex;
        }
    }

    for ( GXUPointer i = 0u; i < files; ++i )
    {
        // Note placement new syntax.
        ::new ( items + itemIndex ) EMUIFileListBoxItem ( eEMUIFileListBoxItemType::File, directoryInfo.GetFileName ( i ) );
        ++itemIndex;
    }

    _fileListBox->AddItems ( items, static_cast<GXUInt> ( totalItems ) );

    for ( GXUPointer i = 0u; i < itemIndex; ++i )
    {
        // Need to call destructor because placement new above.
        items[ i ].~EMUIFileListBoxItem ();
    }

    Free ( items );
    _fileListBox->Redraw ();
}

GXVoid GXCALL EMUIOpenFile::OnButton ( GXVoid* context, GXUIButton& button, GXFloat /*x*/, GXFloat /*y*/, eGXMouseButtonState state )
{
    if ( state != eGXMouseButtonState::Up ) return;

    EMUIOpenFile* uiOpenFile = static_cast<EMUIOpenFile*> ( context );

    if ( &button == uiOpenFile->_cancelButton->GetWidget () )
    {
        uiOpenFile->_mainPanel.Hide ();
        return;
    }

    if ( &button != uiOpenFile->_okButton->GetWidget () ) return;

    const EMUIFileListBoxItem* i = static_cast<const EMUIFileListBoxItem*> ( uiOpenFile->_fileListBox->GetSelectedItem () );

    if ( i && i->GetType () == eEMUIFileListBoxItemType::File )
        uiOpenFile->_onBrowseFile ( uiOpenFile->_filePathStaticText->GetText () );

    uiOpenFile->_mainPanel.Hide ();
}

GXVoid GXCALL EMUIOpenFile::OnItemSelected ( GXVoid* context, GXUIListBox& /*listBox*/, const GXVoid* item )
{
    EMUIOpenFile* uiOpenFile = static_cast<EMUIOpenFile*> ( context );
    const EMUIFileListBoxItem* element = static_cast<const EMUIFileListBoxItem*> ( item );

    switch ( element->GetType () )
    {
        case eEMUIFileListBoxItemType::File:
        {
            if ( uiOpenFile->_rootDirectory == uiOpenFile->_currentDirectory )
            {
                uiOpenFile->_filePathStaticText->SetText ( element->GetName () );
            }
            else
            {
                uiOpenFile->_stringBuffer.Format ( "%s/%s", static_cast<const GXMBChar*> ( uiOpenFile->_relativeDirectory ), static_cast<const GXMBChar*> ( element->GetName () ) );
                uiOpenFile->_filePathStaticText->SetText ( uiOpenFile->_stringBuffer );
            }
        }
        break;

        case eEMUIFileListBoxItemType::Folder:
            uiOpenFile->_filePathStaticText->SetText ( uiOpenFile->_relativeDirectory );
        break;

        default:
            //NOTHING
        break;
    }
}

GXVoid GXCALL EMUIOpenFile::OnItemDoubleClicked ( GXVoid* context, GXUIListBox& /*listBox*/, const GXVoid* item )
{
    EMUIOpenFile* uiOpenFile = static_cast<EMUIOpenFile*> ( context );
    const EMUIFileListBoxItem* element = static_cast<const EMUIFileListBoxItem*> ( item );

    switch ( element->GetType () )
    {
        case eEMUIFileListBoxItemType::File:
            OnButton ( uiOpenFile, *( static_cast<GXUIButton*> ( uiOpenFile->_okButton->GetWidget () ) ), 0.0f, 0.0f, eGXMouseButtonState::Up );
        break;

        case eEMUIFileListBoxItemType::Folder:
            uiOpenFile->UpdateDirectory ( element->GetName () );
        break;
    }
}

GXVoid GXCALL EMUIOpenFile::OnResize ( GXVoid* context, GXUIDragableArea& /*area*/, GXFloat width, GXFloat height )
{
    EMUIOpenFile* uiOpenFile = static_cast<EMUIOpenFile*> ( context );

    uiOpenFile->_cancelButton->Resize ( width - BTN_CANCEL_LEFT_BOTTOM_X * gx_ui_Scale, BTN_BOTTOM_Y * gx_ui_Scale, BTN_WIDTH * gx_ui_Scale, BTN_HEIGHT * gx_ui_Scale );
    uiOpenFile->_okButton->Resize ( width - BTN_OK_LEFT_BOTTOM_X * gx_ui_Scale, BTN_BOTTOM_Y * gx_ui_Scale, BTN_WIDTH * gx_ui_Scale, BTN_HEIGHT * gx_ui_Scale );

    uiOpenFile->_filePathStaticText->Resize ( STT_FILE_PATH_LEFT_BOTTOM_X * gx_ui_Scale, height - STT_FILE_PATH_LEFT_BOTTOM_Y * gx_ui_Scale, width - 2.0f * STT_FILE_PATH_LEFT_BOTTOM_X * gx_ui_Scale, STT_FILE_PATH_HEIGHT * gx_ui_Scale );

    uiOpenFile->_bottomSeparator->Resize ( SEP_OFFSET_X * gx_ui_Scale, SEP_BOTTOM_OFFSET_Y * gx_ui_Scale, width - 2.0f * SEP_OFFSET_X * gx_ui_Scale, SEP_HEIGHT * gx_ui_Scale );
    uiOpenFile->_topSeparator->Resize ( SEP_OFFSET_X * gx_ui_Scale, height - SEP_TOP_OFFSET_Y * gx_ui_Scale, width - 2.0f * SEP_OFFSET_X * gx_ui_Scale, SEP_HEIGHT * gx_ui_Scale );
    
    uiOpenFile->_fileListBox->Resize ( LST_OFFSET_LEFT * gx_ui_Scale, LST_OFFSET_BOTTOM * gx_ui_Scale, width - ( LST_OFFSET_LEFT + LST_OFFSET_RIGHT ) * gx_ui_Scale, height - ( LST_OFFSET_BOTTOM + LST_OFFSET_TOP ) * gx_ui_Scale );
}
