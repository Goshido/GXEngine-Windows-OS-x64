#ifndef EM_UI_FILE_LIST_BOX
#define EM_UI_FILE_LIST_BOX


#include "EMUI.h"
#include <GXEngine/GXUIListBox.h>


enum class eEMUIFileListBoxItemType : GXUByte
{
    Folder,
    File
};

class EMUIFileListBoxItem final : public GXMemoryInspector
{
    private:
        eEMUIFileListBoxItemType    _type;
        GXString                    _name;

    public:
        explicit EMUIFileListBoxItem ( eEMUIFileListBoxItemType type, const GXString &name );
        ~EMUIFileListBoxItem ();

        eEMUIFileListBoxItemType GetType () const;
        GXVoid SetType ( eEMUIFileListBoxItemType type );

        const GXString& GetName () const;
        GXVoid SetName ( const GXString &name );

    private:
        EMUIFileListBoxItem () = delete;
        EMUIFileListBoxItem ( const EMUIFileListBoxItem &other ) = delete;
        EMUIFileListBoxItem& operator = ( const EMUIFileListBoxItem &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

class EMUIFileListBox final : public EMUI
{
    private:
        GXUIListBox     _widget;

    public:
        explicit EMUIFileListBox ( EMUI* parent );
        ~EMUIFileListBox () override;

        GXWidget* GetWidget () override;

        GXVoid Resize ( GXFloat leftBottomX, GXFloat leftBottomY, GXFloat width, GXFloat height );
        GXVoid AddFolder ( const GXString &name );
        GXVoid AddFile ( const GXString &name );
        GXVoid AddItems ( const EMUIFileListBoxItem* itemArray, GXUInt items );
        GXVoid Clear ();
        GXVoid Redraw ();
        const GXVoid* GetSelectedItem () const;
        GXVoid SetOnItemSelectedCallback ( GXVoid* context, GXUIListBoxItemOnItemSelectHandler callback );
        GXVoid SetOnItemDoubleClickedCallbak ( GXVoid* context, GXUIListBoxItemOnItemDoubleClickHandler callback );

    private:
        static GXVoid GXCALL ItemDestructor ( GXVoid* itemData );

        EMUIFileListBox () = delete;
        EMUIFileListBox ( const EMUIFileListBox &other ) = delete;
        EMUIFileListBox& operator = ( const EMUIFileListBox &other ) = delete;
};


#endif // EM_UI_FILE_LIST_BOX
