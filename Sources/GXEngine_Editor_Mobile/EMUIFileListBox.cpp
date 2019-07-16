#include <GXEngine_Editor_Mobile/EMUIFileListBox.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXStrings.h>


#define DEFAULT_WIDTH                       7.0f
#define DEFAULT_HEIGHT                      6.0f
#define DEFAULT_LEFT_BOTTOM_X               0.1f
#define DEFAULT_LEFT_BOTTOM_Y               0.1f

#define FONT                                L"Fonts/trebuc.ttf"
#define FONT_SIZE                           0.33f
#define ITEM_HEIGHT                         0.85f
#define ITEM_NAME_OFFSET_X                  0.9f

#define ITEM_NAME_COLOR_R                   255u
#define ITEM_NAME_COLOR_G                   255u
#define ITEM_NAME_COLOR_B                   255u
#define ITEM_NAME_COLOR_A                   255u

#define BACKGROUND_COLOR_R                  49u
#define BACKGROUND_COLOR_G                  48u
#define BACKGROUND_COLOR_B                  48u
#define BACKGROUND_COLOR_A                  255u

#define RECTANGLE                           L"Textures/System/Default_Diffuse.tga"

#define FOLDER_ICON                         L"Textures/Editor Mobile/gui_folder_icon.png"
#define FOLDER_ICON_ASPECT                  1.3f
#define FOLDER_ICON_HEIGHT                  0.54f

#define FILE_ICON                           L"Textures/Editor Mobile/gui_file_icon.png"
#define FILE_ICON_ASPECT                    0.75f
#define FILE_ICON_HEIGHT                    0.6f

#define HIGHLIGHTED_COLOR_R                 25u
#define HIGHLIGHTED_COLOR_G                 25u
#define HIGHLIGHTED_COLOR_B                 25u
#define HIGHLIGHTED_COLOR_A                 25u

#define SELECTED_COLOR_R                    115u
#define SELECTED_COLOR_G                    185u
#define SELECTED_COLOR_B                    0u
#define SELECTED_COLOR_A                    102u

#define ICON_OFFSET_X                       0.1f

#define PIXEL_PERFECT_LOCATION_OFFSET_X     0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y     0.25f

//---------------------------------------------------------------------------------------------------------------------

EMUIFileListBoxItem::EMUIFileListBoxItem ( eEMUIFileListBoxItemType type, const GXWChar* name ):
    _type ( type )
{
    GXWcsclone ( &this->_name, name );
}

EMUIFileListBoxItem::~EMUIFileListBoxItem ()
{
    GXSafeFree ( _name );
}

eEMUIFileListBoxItemType EMUIFileListBoxItem::GetType () const
{
    return _type;
}

GXVoid EMUIFileListBoxItem::SetType ( eEMUIFileListBoxItemType newType )
{
    _type = newType;
}

const GXWChar* EMUIFileListBoxItem::GetName () const
{
    return _name;
}

GXVoid EMUIFileListBoxItem::SetName ( const GXWChar* newName )
{
    _name = const_cast<GXWChar*> ( newName );
}

//---------------------------------------------------------

class EMUIFileListBoxRenderer final : public GXWidgetRenderer
{
    private:
        GXFont              _font;
        GXTexture2D         _rectangle;
        GXTexture2D         _fileIcon;
        GXTexture2D         _folderIcon;
        GXHudSurface*       _surface;

    public:
        explicit EMUIFileListBoxRenderer ( GXUIListBox* widget );
        ~EMUIFileListBoxRenderer () override;

        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;

    protected:
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

    private:
        EMUIFileListBoxRenderer () = delete;
        EMUIFileListBoxRenderer ( const EMUIFileListBoxRenderer &other ) = delete;
        EMUIFileListBoxRenderer& operator = ( const EMUIFileListBoxRenderer &other ) = delete;
};

EMUIFileListBoxRenderer::EMUIFileListBoxRenderer ( GXUIListBox* widget ) :
    GXWidgetRenderer ( widget ),
    _font ( FONT, static_cast<GXUShort> ( FONT_SIZE * gx_ui_Scale ) ),
    _rectangle ( RECTANGLE, GX_FALSE, GX_FALSE ),
    _fileIcon ( FILE_ICON, GX_FALSE, GX_FALSE ),
    _folderIcon ( FOLDER_ICON, GX_FALSE, GX_FALSE )
{
    const GXAABB& boundsLocal = widget->GetBoundsWorld ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    _surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
}

EMUIFileListBoxRenderer::~EMUIFileListBoxRenderer ()
{
    delete _surface;
}

GXVoid EMUIFileListBoxRenderer::OnRefresh ()
{
    GXUIListBox* listBoxWidget = static_cast<GXUIListBox*> ( widget );

    const GXUInt totalItems = listBoxWidget->GetTotalItems ();
    const GXUIListBoxItem* items = listBoxWidget->GetItems ();
    const GXFloat itemHeight = listBoxWidget->GetItemHeight ();
    const GXFloat width = widget->GetBoundsLocal ().GetWidth ();
    const GXFloat height = widget->GetBoundsLocal ().GetHeight ();

    _surface->Reset ();
    GXImageInfo ii;
    ii._color.From ( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
    ii._insertX = ii._insertY = 0.0f;
    ii._insertWidth = width;
    ii._insertHeight = height;
    ii._overlayType = eGXImageOverlayType::SimpleReplace;
    ii._texture = &_rectangle;

    _surface->AddImage ( ii );

    const GXFloat folderIconHeight = FOLDER_ICON_HEIGHT * gx_ui_Scale;
    const GXFloat folderIconWidth = folderIconHeight * FOLDER_ICON_ASPECT;
    const GXFloat folderIconYOffset = ( itemHeight - folderIconHeight ) * 0.5f;

    const GXFloat fileIconHeight = FILE_ICON_HEIGHT * gx_ui_Scale;
    const GXFloat fileIconWidth = fileIconHeight * FILE_ICON_ASPECT;
    const GXFloat fileIconYOffset = ( itemHeight - fileIconWidth ) * 0.5f;

    GXPenInfo pi;
    pi._color.From ( ITEM_NAME_COLOR_R, ITEM_NAME_COLOR_G, ITEM_NAME_COLOR_B, ITEM_NAME_COLOR_A );
    pi._font = &_font;
    pi._insertX = ITEM_NAME_OFFSET_X * gx_ui_Scale;
    pi._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

    GXFloat iconXOffset = ICON_OFFSET_X * gx_ui_Scale;
    ii._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

    static const GXColorRGB highlightedColor ( static_cast<GXUByte> ( HIGHLIGHTED_COLOR_R ), static_cast<GXUByte> ( HIGHLIGHTED_COLOR_G ), static_cast<GXUByte> ( HIGHLIGHTED_COLOR_B ), static_cast<GXUByte> ( HIGHLIGHTED_COLOR_A ) );
    static const GXColorRGB selectedColor ( static_cast<GXUByte> ( SELECTED_COLOR_R ), static_cast<GXUByte> ( SELECTED_COLOR_G ), static_cast<GXUByte> ( SELECTED_COLOR_B ), static_cast<GXUByte> ( SELECTED_COLOR_A ) );
    static const GXColorRGB iconColor ( static_cast<GXUByte> ( 255u ), static_cast<GXUByte> ( 255u ), static_cast<GXUByte> ( 255u ), static_cast<GXUByte> ( 255u ) );

    const GXFloat itemNameYOffset = ( itemHeight - _font.GetSize () * 0.5f ) * 0.5f;

    for ( GXUInt i = 0u; i < totalItems; ++i )
    {
        if ( !listBoxWidget->IsItemVisible ( i ) )
        {
            items = items->_next;
            continue;
        }

        const EMUIFileListBoxItem* item = static_cast<EMUIFileListBoxItem*> ( items->_data );
        GXFloat yOffset = listBoxWidget->GetItemLocalOffsetY ( i );

        if ( items->_isSelected )
        {
            ii._insertY = yOffset;
            ii._insertX = 0.0f;
            ii._insertWidth = width;
            ii._insertHeight = itemHeight;
            ii._color = selectedColor;
            ii._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
            ii._texture = &_rectangle;

            _surface->AddImage ( ii );
        }

        if ( items->_isHighlighted )
        {
            ii._insertY = yOffset;
            ii._insertX = 0.0f;
            ii._insertWidth = width;
            ii._insertHeight = itemHeight;
            ii._color = highlightedColor;
            ii._overlayType = eGXImageOverlayType::AlphaAdd;
            ii._texture = &_rectangle;

            _surface->AddImage ( ii );
        }

        switch ( item->GetType () )
        {
            case eEMUIFileListBoxItemType::File:
                ii._insertY = yOffset + fileIconYOffset;
                ii._insertWidth = fileIconWidth;
                ii._insertHeight = fileIconHeight;
                ii._texture = &_fileIcon;
            break;

            case eEMUIFileListBoxItemType::Folder:
                ii._insertY = yOffset + folderIconYOffset;
                ii._insertWidth = folderIconWidth;
                ii._insertHeight = folderIconHeight;
                ii._texture = &_folderIcon;
            break;

            default:
                // NOTHING
            break;
        }

        ii._insertX = iconXOffset;
        ii._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
        ii._color = iconColor;

        _surface->AddImage ( ii );

        pi._insertY = yOffset + itemNameYOffset;
        _surface->AddText ( pi, 0u, item->GetName () );

        yOffset -= itemHeight;
        items = items->_next;
    }
}

GXVoid EMUIFileListBoxRenderer::OnDraw ()
{
    glDisable ( GL_DEPTH_TEST );
    _surface->Render ();
    glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIFileListBoxRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXSafeDelete ( _surface );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    _surface = new GXHudSurface ( width, height );

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

GXVoid EMUIFileListBoxRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

//---------------------------------------------------------------------------------------------------------------------

EMUIFileListBox::EMUIFileListBox ( EMUI* parent ):
    EMUI ( parent ),
    _widget ( new GXUIListBox ( parent ? parent->GetWidget () : nullptr, &EMUIFileListBox::ItemDestructor ) )
{
    _widget->Resize ( DEFAULT_LEFT_BOTTOM_X * gx_ui_Scale, DEFAULT_LEFT_BOTTOM_Y * gx_ui_Scale, DEFAULT_WIDTH * gx_ui_Scale, DEFAULT_HEIGHT * gx_ui_Scale );
    _widget->SetItemHeight ( ITEM_HEIGHT * gx_ui_Scale );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIFileListBoxRenderer" );
    _widget->SetRenderer ( new EMUIFileListBoxRenderer ( _widget ) );
}

EMUIFileListBox::~EMUIFileListBox ()
{
    delete _widget->GetRenderer ();
    delete _widget;
}

GXWidget* EMUIFileListBox::GetWidget () const
{
    return _widget;
}

GXVoid EMUIFileListBox::Resize ( GXFloat leftBottomX, GXFloat leftBottomY, GXFloat width, GXFloat height )
{
    _widget->Resize ( leftBottomX, leftBottomY, width, height );
}

GXVoid EMUIFileListBox::AddFolder ( const GXWChar* name )
{
    _widget->AddItem ( new EMUIFileListBoxItem ( eEMUIFileListBoxItemType::Folder, name ) );
}

GXVoid EMUIFileListBox::AddFile ( const GXWChar* name )
{
    _widget->AddItem ( new EMUIFileListBoxItem ( eEMUIFileListBoxItemType::File, name ) );
}

GXVoid EMUIFileListBox::Clear ()
{
    _widget->RemoveAllItems ();
}

GXVoid EMUIFileListBox::AddItems ( const EMUIFileListBoxItem* itemArray, GXUInt items )
{
    if ( items == 0u || !itemArray ) return;

    EMUIFileListBoxItem** elements = reinterpret_cast<EMUIFileListBoxItem**> ( malloc ( items * sizeof ( EMUIFileListBoxItem* ) ) );

    for ( GXUInt i = 0u; i < items; ++i )
        elements[ i ] = new EMUIFileListBoxItem ( itemArray[ i ].GetType (), itemArray[ i ].GetName () );
    
    _widget->AddItems ( reinterpret_cast<GXVoid**> ( elements ), items );

    free ( elements );
}

GXVoid EMUIFileListBox::Redraw ()
{
    _widget->Redraw ();
}

const GXVoid* EMUIFileListBox::GetSelectedItem () const
{
    return _widget->GetSelectedItem ();
}

GXVoid EMUIFileListBox::SetOnItemSelectedCallback ( GXVoid* handler, PFNGXUILISTBOXONITEMSELECTEDPROC callback )
{
    _widget->SetOnItemSelectedCallback ( handler, callback );
}

GXVoid EMUIFileListBox::SetOnItemDoubleClickedCallbak ( GXVoid* handler, PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC callback )
{
    _widget->SetOnItemDoubleClickedCallback ( handler, callback );
}

GXVoid GXCALL EMUIFileListBox::ItemDestructor ( GXVoid* itemData )
{
    EMUIFileListBoxItem* p = static_cast<EMUIFileListBoxItem*> ( itemData );
    delete p;
}
