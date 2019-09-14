#ifndef EM_UI_OPEN_FILE
#define EM_UI_OPEN_FILE


#include "EMUIButton.h"
#include "EMUIDraggableArea.h"
#include "EMUIStaticText.h"
#include "EMUISeparator.h"
#include "EMUIFileListBox.h"
#include <GXCommon/GXFileSystem.h>


typedef GXVoid ( GXCALL* EMUIOpenFileBrowseFileHandler ) ( GXString filePath );

class EMUIOpenFile final : public EMUI
{
    private:
        const GXString                      _rootDirectory;
        EMUIDraggableArea                   _mainPanel;

        GXUPointer                          _rootDirectoryPathOffset;

        GXString                            _currentDirectory;
        GXString                            _relativeDirectory;

        // for optimization purposes
        GXString                            _stringBuffer;

        EMUIButton*                         _okButton;
        EMUIButton*                         _cancelButton;
        EMUIStaticText*                     _filePathStaticText;
        EMUISeparator*                      _bottomSeparator;
        EMUISeparator*                      _topSeparator;
        EMUIFileListBox*                    _fileListBox;

        EMUIOpenFileBrowseFileHandler       _onBrowseFile;

    public:
        EMUIOpenFile ();
        ~EMUIOpenFile () override;

        GXWidget* GetWidget () override;

        GXVoid Browse ( EMUIOpenFileBrowseFileHandler callback );

    private:
        GXVoid UpdateDirectory ( GXString targetDirectory );

        static GXVoid GXCALL OnButton ( GXVoid* context, GXUIButton& button, GXFloat x, GXFloat y, eGXMouseButtonState state );
        static GXVoid GXCALL OnItemSelected ( GXVoid* context, GXUIListBox& listBox, const GXVoid* item );
        static GXVoid GXCALL OnItemDoubleClicked ( GXVoid* context, GXUIListBox& listBox, const GXVoid* item );
        static GXVoid GXCALL OnResize ( GXVoid* context, GXUIDragableArea& area, GXFloat width, GXFloat height );

        EMUIOpenFile ( const EMUIOpenFile &other ) = delete;
        EMUIOpenFile& operator = ( const EMUIOpenFile &other ) = delete;
};


#endif // EM_UI_OPEN_FILE
