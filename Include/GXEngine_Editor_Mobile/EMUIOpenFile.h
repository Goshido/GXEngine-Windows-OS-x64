#ifndef EM_UI_OPEN_FILE
#define EM_UI_OPEN_FILE


#include "EMUIButton.h"
#include "EMUIDraggableArea.h"
#include "EMUIStaticText.h"
#include "EMUISeparator.h"
#include "EMUIFileListBox.h"
#include <GXCommon/GXFileSystem.h>


typedef GXVoid ( GXCALL* EMUIOpenFileBrowseFileHandler ) ( const GXWChar* filePath );

class EMUIOpenFile final : public EMUI
{
    private:
        GXWChar*                            _currentDirectory;

        const GXString                      _rootDirectory;
        GXUPointer                          _rootDirectoryPathOffset;

        EMUIDraggableArea*                  _mainPanel;
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

        GXWidget* GetWidget () const override;

        GXVoid Browse ( EMUIOpenFileBrowseFileHandler callback );

    private:
        GXVoid UpdateDirectory ( const GXWChar* folder );
        const GXWChar* GetRelativePath () const;

        static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton& button, GXFloat x, GXFloat y, eGXMouseButtonState state );
        static GXVoid GXCALL OnItemSelected ( GXVoid* handler, GXUIListBox& listBox, const GXVoid* item );
        static GXVoid GXCALL OnItemDoubleClicked ( GXVoid* handler, GXUIListBox& listBox, const GXVoid* item );
        static GXVoid GXCALL OnResize ( GXVoid* handler, GXUIDragableArea& area, GXFloat width, GXFloat height );

        EMUIOpenFile ( const EMUIOpenFile &other ) = delete;
        EMUIOpenFile& operator = ( const EMUIOpenFile &other ) = delete;
};


#endif // EM_UI_OPEN_FILE
