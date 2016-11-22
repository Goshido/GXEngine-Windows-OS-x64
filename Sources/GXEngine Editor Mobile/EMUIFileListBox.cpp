#include <GXEngine_Editor_Mobile/EMUIFileListBox.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXFontStorage.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXGlobals.h>
#include <GXCommon/GXStrings.h>


#define EM_DEFAULT_WIDTH					7.0f
#define EM_DEFAULT_HEIGHT					6.0f
#define EM_DEFAULT_LEFT_BOTTOM_X			0.1f
#define EM_DEFAULT_LEFT_BOTTOM_Y			0.1f

#define EM_DEFAULT_FONT						L"Fonts/trebuc.ttf"
#define EM_DEFAULT_FONT_SIZE				0.33f
#define EM_DEFAULT_ITEM_HEIGHT				0.85f
#define EM_DEFAULT_ITEM_NAME_OFFSET_X		0.9f

#define EM_DEFAULT_ITEM_NAME_COLOR_R		255
#define EM_DEFAULT_ITEM_NAME_COLOR_G		255
#define EM_DEFAULT_ITEM_NAME_COLOR_B		255
#define EM_DEFAULT_ITEM_NAME_COLOR_A		255

#define EM_DEFAULT_BACKGROUND_COLOR_R		49
#define EM_DEFAULT_BACKGROUND_COLOR_G		48
#define EM_DEFAULT_BACKGROUND_COLOR_B		48
#define EM_DEFAULT_BACKGROUND_COLOR_A		255

#define EM_DEFAULT_RECTANGLE				L"Textures/System/Default_Diffuse.tga"

#define EM_DEFAULT_FOLDER_ICON				L"Textures/Editor Mobile/gui_folder_icon.png"
#define EM_DEFAULT_FOLDER_ICON_ASPECT		1.3f
#define EM_DEFAULT_FOLDER_ICON_HEIGHT		0.54f

#define EM_DEFAULT_FILE_ICON				L"Textures/Editor Mobile/gui_file_icon.png"
#define EM_DEFAULT_FILE_ICON_ASPECT			0.75f
#define EM_DEFAULT_FILE_ICON_HEIGHT			0.6f

#define EM_DEFAULT_HIGHLIGHTED_COLOR_R		25
#define EM_DEFAULT_HIGHLIGHTED_COLOR_G		25
#define EM_DEFAULT_HIGHLIGHTED_COLOR_B		25
#define EM_DEFAULT_HIGHLIGHTED_COLOR_A		25

#define EM_DEFAULT_SELECTED_COLOR_R			115
#define EM_DEFAULT_SELECTED_COLOR_G			185
#define EM_DEFAULT_SELECTED_COLOR_B			0
#define EM_DEFAULT_SELECTED_COLOR_A			102

#define EM_DEFAULT_ICON_OFFSET_X			0.1f


EMUIFileListBoxItem::EMUIFileListBoxItem ( eEMUIFileListBoxItemType type, const GXWChar* name )
{
	this->type = type;
	GXWcsclone ( &this->name, name );
}

EMUIFileListBoxItem::~EMUIFileListBoxItem ()
{
	GXSafeFree ( name );
}

//---------------------------------------------------------

class EMUIFileListBoxRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXFont*				font;
		GXTexture			rectangle;
		GXTexture			fileIcon;
		GXTexture			folderIcon;

	public:
		EMUIFileListBoxRenderer ( GXUIListBox* widget );
		~EMUIFileListBoxRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIFileListBoxRenderer::EMUIFileListBoxRenderer ( GXUIListBox* widget ) :
GXWidgetRenderer ( widget )
{
	font = GXGetFont ( EM_DEFAULT_FONT, (GXUShort)( EM_DEFAULT_FONT_SIZE * gx_ui_Scale ) );
	GXLoadTexture ( EM_DEFAULT_RECTANGLE, rectangle );
	GXLoadTexture ( EM_DEFAULT_FOLDER_ICON, folderIcon );
	GXLoadTexture ( EM_DEFAULT_FILE_ICON, fileIcon );

	surface = nullptr;
	const GXAABB& bounds = widget->GetBoundsWorld ();
	OnResized ( 0.0f, 0.0f, (GXUShort)GXGetAABBWidth ( bounds ), (GXUShort)GXGetAABBHeight ( bounds ) );
	EMSetHudSurfaceLocationWorld ( *surface, bounds, EMGetNextGUIForegroundZ (), gx_Core->GetRenderer ()->GetWidth (), gx_Core->GetRenderer ()->GetHeight () );

	OnRefresh ();
}

EMUIFileListBoxRenderer::~EMUIFileListBoxRenderer ()
{
	delete surface;
	GXRemoveFont ( font );
	GXRemoveTexture ( rectangle );
	GXRemoveTexture ( folderIcon );
	GXRemoveTexture ( fileIcon );
}

GXVoid EMUIFileListBoxRenderer::OnRefresh ()
{
	GXUIListBox* listBoxWidget = (GXUIListBox*)widget;

	GXUInt totalItems = listBoxWidget->GetTotalItems ();
	GXUIListBoxItem* items = listBoxWidget->GetItems ();
	GXFloat itemHeight = listBoxWidget->GetItemHeight ();
	GXFloat width = GXGetAABBWidth ( widget->GetBoundsLocal () );
	GXFloat height = GXGetAABBHeight ( widget->GetBoundsLocal () );

	surface->Reset ();
	GXImageInfo ii;
	GXColorToVec4 ( ii.color, EM_DEFAULT_BACKGROUND_COLOR_R, EM_DEFAULT_BACKGROUND_COLOR_G, EM_DEFAULT_BACKGROUND_COLOR_B, EM_DEFAULT_BACKGROUND_COLOR_A );
	ii.insertX = ii.insertY = 0.0f;
	ii.insertWidth = width;
	ii.insertHeight = height;
	ii.overlayType = GX_SIMPLE_REPLACE;
	ii.texture = rectangle;

	surface->AddImage ( ii );

	GXFloat folderIconHeight = EM_DEFAULT_FOLDER_ICON_HEIGHT * gx_ui_Scale;
	GXFloat folderIconWidth = folderIconHeight * EM_DEFAULT_FOLDER_ICON_ASPECT;
	GXFloat folderIconYOffset = ( itemHeight - folderIconHeight ) * 0.5f;

	GXFloat fileIconHeight = EM_DEFAULT_FILE_ICON_HEIGHT * gx_ui_Scale;
	GXFloat fileIconWidth = fileIconHeight * EM_DEFAULT_FILE_ICON_ASPECT;
	GXFloat fileIconYOffset = ( itemHeight - fileIconWidth ) * 0.5f;

	GXPenInfo pi;
	GXColorToVec4 ( pi.color, EM_DEFAULT_ITEM_NAME_COLOR_R, EM_DEFAULT_ITEM_NAME_COLOR_G, EM_DEFAULT_ITEM_NAME_COLOR_B, EM_DEFAULT_ITEM_NAME_COLOR_A );
	pi.font = font;
	pi.insertX = EM_DEFAULT_ITEM_NAME_OFFSET_X * gx_ui_Scale;
	pi.overlayType = GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA;


	GXFloat iconXOffset = EM_DEFAULT_ICON_OFFSET_X * gx_ui_Scale;
	ii.overlayType = GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA;

	GXVec4 highlightedColor;
	GXColorToVec4 ( highlightedColor, EM_DEFAULT_HIGHLIGHTED_COLOR_R, EM_DEFAULT_HIGHLIGHTED_COLOR_G, EM_DEFAULT_HIGHLIGHTED_COLOR_B, EM_DEFAULT_HIGHLIGHTED_COLOR_A );

	GXVec4 selectedColor;
	GXColorToVec4 ( selectedColor, EM_DEFAULT_SELECTED_COLOR_R, EM_DEFAULT_SELECTED_COLOR_G, EM_DEFAULT_SELECTED_COLOR_B, EM_DEFAULT_SELECTED_COLOR_A );

	GXVec4 iconColor;
	GXColorToVec4 ( iconColor, 255, 255, 255, 255 );

	GXFloat itemNameYOffset = ( itemHeight - font->GetSize () * 0.5f ) * 0.5f;

	for ( GXUInt i = 0; i < totalItems; i++ )
	{
		if ( !listBoxWidget->IsItemVisible ( i ) )
		{
			items = items->next;
			continue;
		}

		EMUIFileListBoxItem* item = (EMUIFileListBoxItem*)items->data;
		GXFloat yOffset = listBoxWidget->GetItemLocalOffsetY ( i );

		if ( items->isSelected )
		{
			ii.insertY = yOffset;
			ii.insertX = 0.0f;
			ii.insertWidth = width;
			ii.insertHeight = itemHeight;
			ii.color = selectedColor;
			ii.overlayType = GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA;
			ii.texture = rectangle;

			surface->AddImage ( ii );
		}

		if ( items->isHighlighted )
		{
			ii.insertY = yOffset;
			ii.insertX = 0.0f;
			ii.insertWidth = width;
			ii.insertHeight = itemHeight;
			ii.color = highlightedColor;
			ii.overlayType = GX_ALPHA_ADD;
			ii.texture = rectangle;

			surface->AddImage ( ii );
		}

		switch ( item->type )
		{
			case EM_UI_FILE_BOX_ITEM_TYPE_FILE:
				ii.insertY = yOffset + fileIconYOffset;
				ii.insertWidth = fileIconWidth;
				ii.insertHeight = fileIconHeight;
				ii.texture = fileIcon;
			break;

			case EM_UI_FILE_BOX_ITEM_TYPE_FOLDER:
				ii.insertY = yOffset + folderIconYOffset;
				ii.insertWidth = folderIconWidth;
				ii.insertHeight = folderIconHeight;
				ii.texture = folderIcon;
			break;
		}

		ii.insertX = iconXOffset;
		ii.overlayType = GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA;
		ii.color = iconColor;

		surface->AddImage ( ii );

		pi.insertY = yOffset + itemNameYOffset;
		surface->AddText ( pi, 0, item->name );

		yOffset -= itemHeight;
		items = items->next;
	}
}

GXVoid EMUIFileListBoxRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Draw ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIFileListBoxRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height, GX_FALSE );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIFileListBoxRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

//---------------------------------------------------------

EMUIFileListBox::EMUIFileListBox ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXUIListBox ( parent ? parent->GetWidget () : nullptr, &EMUIFileListBox::ItemDestructor );
	widget->Resize ( EM_DEFAULT_LEFT_BOTTOM_X * gx_ui_Scale, EM_DEFAULT_LEFT_BOTTOM_Y * gx_ui_Scale, EM_DEFAULT_WIDTH * gx_ui_Scale, EM_DEFAULT_HEIGHT * gx_ui_Scale );
	widget->SetItemHeight ( EM_DEFAULT_ITEM_HEIGHT * gx_ui_Scale );
	widget->SetRenderer ( new EMUIFileListBoxRenderer ( widget ) );
}

EMUIFileListBox::~EMUIFileListBox ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUIFileListBox::GetWidget () const
{
	return widget;
}

GXVoid EMUIFileListBox::OnDrawMask ()
{
	//TODO
}

GXVoid EMUIFileListBox::Resize ( GXFloat leftBottomX, GXFloat leftBottomY, GXFloat width, GXFloat height )
{
	widget->Resize ( leftBottomX, leftBottomY, width, height );
}

GXVoid EMUIFileListBox::AddFolder ( const GXWChar* name )
{
	widget->AddItem ( new EMUIFileListBoxItem ( EM_UI_FILE_BOX_ITEM_TYPE_FOLDER, name ) );
}

GXVoid EMUIFileListBox::AddFile ( const GXWChar* name )
{
	widget->AddItem ( new EMUIFileListBoxItem ( EM_UI_FILE_BOX_ITEM_TYPE_FILE, name ) );
}

GXVoid EMUIFileListBox::Clear ()
{
	widget->RemoveAllItems ();
}

GXVoid EMUIFileListBox::AddItems ( const EMUIFileListBoxItem* itemArray, GXUInt items )
{
	if ( items == 0 || !itemArray ) return;

	EMUIFileListBoxItem** elements = (EMUIFileListBoxItem**)malloc ( items * sizeof ( EMUIFileListBoxItem* ) );
	for ( GXUInt i = 0; i < items; i++ )
		elements[ i ] = new EMUIFileListBoxItem ( itemArray[ i ].type, itemArray[ i ].name );
	
	widget->AddItems ( (GXVoid**)elements, items );

	free ( elements );
}

GXVoid EMUIFileListBox::Redraw ()
{
	widget->Redraw ();
}

const GXVoid* EMUIFileListBox::GetSelectedItem () const
{
	return widget->GetSelectedItem ();
}

GXVoid EMUIFileListBox::SetOnItemSelectedCallback ( GXVoid* handler, PFNGXUILISTBOXONITEMSELECTEDPROC callback )
{
	widget->SetOnItemSelectedCallback ( handler, callback );
}

GXVoid EMUIFileListBox::SetOnItemDoubleClickedCallbak ( GXVoid* handler, PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC callback )
{
	widget->SetOnItemDoubleClickedCallback ( handler, callback );
}

GXVoid GXCALL EMUIFileListBox::ItemDestructor ( GXVoid* itemData )
{
	EMUIFileListBoxItem* p = (EMUIFileListBoxItem*)itemData;
	delete p;
}
